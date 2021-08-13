/*
 * Decentralized Platooning Service
 * Copyright 2018 Tobias Renzler
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

#include "PlatooningService.h"

#include "platoon_msgs/ReadyToPlatoonMsg_m.h"
#include "platoon_msgs/PlatoonMsg_m.h"
#include "artery/traci/VehicleController.h"
#include <vanetza/btp/data_request.hpp>
#include <vanetza/dcc/profile.hpp>
#include <vanetza/geonet/interface.hpp>
#include "artery/application/CaPlatooningService.h"

#include "artery/envmod/LocalEnvironmentModel.h"
#include "artery/envmod/service/EnvmodPrinter.h"
#include "artery/envmod/EnvironmentModelObject.h"

#include <cmath>
#include <boost/units/cmath.hpp>
#include <boost/units/systems/si/prefixes.hpp>

namespace artery{

using namespace omnetpp;
using namespace vanetza;

Define_Module(PlatooningService)

/*PlatooningService::PlatooningService(){

}*/
void PlatooningService::initialize()
{
    ItsG5BaseService::initialize();
    mVehicleController = getFacilities().get_mutable_ptr<traci::VehicleController>();
    mVehicleDataProvider = &getFacilities().get_const<VehicleDataProvider>();
    mLocalDynamicMap = &getFacilities().get_mutable<artery::LocalDynamicMap>();  //init local awareness map
    mlocalEnvmod = &getFacilities().get_const<artery::LocalEnvironmentModel>();
    mRTP = par("readyToPlatoonInterval");
    mLastRTP = simTime();
    mCTRL = par("controlInterval");
    mLastCTRL = simTime();


    //init platoon map
    mPlatoonMap.readyToPlatoon = true;
    mPlatoonMap.memberOfPlatoon = false;
    mPlatoonMap.pendingOperation.first = false;
    mPlatoonMap.members = std::vector<uint32_t>(MAX_VEHICLES_PER_PLATOON, 0);
    mPlatoonMap.timeStamp = simTime();
    mPlatoonMap.depth = 0;
    mPlatoonMap.platoonId = 0;

    //ASIL D
	if(PRR != 1.0){
		mAsilDCounter = ceil(-8/log10(1-PRR));
		double mAsilDTime = mAsilDCounter * STEP_SIZE; //actually multiply it with the minimum CAM generation rate instead of STEP_SIZE.
		//add STEP_SIZE to assure 0.1 seconds are added for the first lost CAM. Then multiply it with the current speed to get the distance.
		mAsilDDistance = (mAsilDTime + 0.1 + STEP_SIZE) * mVehicleDataProvider->speed()/boost::units::si::meter_per_second;
	} else{
		mAsilDCounter = 0;
		//mAsilDDistance = 7.5; // to achieve 10 m once we substract the length of the vehicles
		mAsilDDistance = (0.1 + STEP_SIZE) * mVehicleDataProvider->speed()/boost::units::si::meter_per_second;
	}

	//for scneario 1 (paper)
	mAsilDDistance = 10;

	//scenario 2
	//mAsilDDistance = mAsilDDistance + D_MIN_INIT;
	//mAsilDDistance = mAsilDDistance + pow(22.0, 2)/(2*5) - pow(22.0, 2)/(2*7);


	mPlatoonMap.ivDistance = mAsilDDistance + 16.5; //add length of vehicles

    mPlatoonMap.timeDistanceHorizon.current = -1;

    mState.distance = 0;
    mState.velocity = 0;
    mState.acceleration = 0;
    mState.jerk = 0;

    //controller gains
    k_a = 3.0;
    k_v = 3.0;
    k_s = 1.0;

    //register signals for logging
    ivDistanceId = registerSignal("ivDistance");
    vehicleVelocityId = registerSignal("vehicleVelocity");
    vehicleAccelerationId = registerSignal("vehicleAcceleration");

    //select vehicle modes
    mVehicleController->getLiteAPI().vehicle().setSpeedMode(mVehicleController->getVehicleId(), 6); //bound acceleration by min/max
    mVehicleController->getLiteAPI().vehicle().setLaneChangeMode(mVehicleController->getVehicleId(), 0b000000000000); //do not allow any lane change
}

/**
 * Triggers the Platooning Service.
 * 		Once the vehicle is ready to platoon and not part of a platoon, ::Ready messages are sent periodically.
 * 		The frequency can be set by mRTP.
 *
 * 		Once the vehicle is a member of a platoon, it spreads platoon information by sending ::Info messages periodically.
 * 		The frequency can be set by mRTP.
 * 		Furthermore, the platoonControl() function is called periodically. The frequency can be set by mCTRL.
 */
void PlatooningService::trigger()
{
	//Spread Platooning Info / Readiness
    if(mPlatoonMap.readyToPlatoon && !mPlatoonMap.memberOfPlatoon){
        //std::pair <std::string, double> predecessor =
        //                mVehicleController->getLiteAPI().vehicle().getLeaderPhysDistance(mVehicleController->getVehicleId(), MAX_LOOKAHEAD_DISTANCE);
    	if(simTime() - mLastRTP >= mRTP){
    		mLastRTP = simTime();
    		sendPlatoonMessage(MessageTypePlatoon::Ready, 0);
    		std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
    		            		<< ") sent MessageTypePlatoon::Ready!" << std::endl;
    	}
    } else if(mPlatoonMap.memberOfPlatoon){
        //spread platoon data
        if(simTime() - mLastRTP >= mRTP){
        	mLastRTP = simTime();
        	sendPlatoonMessage(MessageTypePlatoon::Info, 0);
        	std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
						<< ") sent MessageTypePlatoon::Info!" << std::endl;
        }
    }

    // control algorithm once in platoon
    if(mPlatoonMap.memberOfPlatoon && (simTime() - mLastCTRL >= mCTRL)){
    	mLastCTRL = simTime();
    	platoonControl();
    } else if (!mPlatoonMap.memberOfPlatoon && (simTime() - mLastCTRL >= mCTRL)){ // else log velocities and accelerations of vehicles
    	//mLastCTRL = simTime();
    	emit(vehicleVelocityId, mVehicleController->getSpeed().value());
    	emit(vehicleAccelerationId, mVehicleDataProvider->acceleration().value());
    }

    //if invite was not accepted (packet loss of CAM), dissolve previously created platoon if only ego vehicle is part of it
    if(mPlatoonMap.pendingOperation.first && (mPlatoonMap.pendingOperationTimer + SimTime(5*STEP_SIZE) < simTime())){
    	mPlatoonMap.pendingOperation.first = false;
    	std::cout << mVehicleController->getVehicleId()<< ": Platoon Invitation due of missing response revoked!" << std::endl;
    	if(mPlatoonMap.memberOfPlatoon && mPlatoonMap.innerPosition == 0){
    		mPlatoonMap.platoonId = 0;
			mPlatoonMap.memberOfPlatoon = false;
			mPlatoonMap.readyToPlatoon = true;
			mPlatoonMap.depth = 1;
			mPlatoonMap.members = std::vector<uint32_t>(MAX_VEHICLES_PER_PLATOON, 0);
    	}
    }

}

/**
 * Dependent on the inner position of a vehicle in a platoon, the speed of the vehicle within the next time step is set here.
 *
 * The vehicle at innerPosition 0 is the leader and decides the pace for the platoon. Emergency breaking scenarios may be
 * simulated as well there.
 *
 * Vehicles at a different innerPosition are part of the platoon and follow a ceratin control strategy, usually dependent on
 * the preceeding vehicle. If there is no recent CAM of that vehicle available in the locally stored dynamic map, then the
 * function returns and the vehicle keeps the velocity of the previous step.
 *
 * ATTENTION: The CAM structure only allows acceleration values in [m/s] of type long! This means, no fractions can be sent to
 * other vehicles. Consider this, when selecting the maximum acceleration/deceleration in .rou.xml
 */
void PlatooningService::platoonControl(){
	double v_ref;
	vanetza::asn1::Cam camPredecessor;
	StateVector predecessor;
	switch(mPlatoonMap.innerPosition){
		case 0:
			considerScenario(predecessor, camPredecessor, "0004");

			std::cout << mVehicleController->getVehicleId() << " SPEED control: " << mVehicleController->getSpeed().value() << std::endl;
			emit(vehicleVelocityId, mVehicleController->getSpeed().value()); //log velocity for statistics
			emit(vehicleAccelerationId, mVehicleDataProvider->acceleration().value()); //log acceleration for statistics
			break;
		default:
			considerScenario(predecessor, camPredecessor, "0004");
			double my_v = mVehicleDataProvider->speed() / boost::units::si::meter_per_second;

			std::tuple<double, double> mytuple = timeHeadwayPredecessorLeaderPolicy(predecessor, camPredecessor, my_v);

			double u_i = std::get<0>(mytuple);
			double distance = std::get<1>(mytuple);

			if(my_v + u_i > 0){
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), my_v + u_i, STEP_SIZE*1000); //care: this is in ms!
			} else{
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), 0, STEP_SIZE*1000); //care: this is in ms!
			}

			std::cout << mVehicleController->getVehicleId() << " SPEED control: " << my_v << " with distance "
					<< distance << " at time " << simTime() << std::endl;

			//log distance for statistics: substract predecessors vehicle length
		    emit(ivDistanceId, distance - camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue/10.0);
		    emit(vehicleVelocityId, my_v); //log velocity for statistics
			emit(vehicleAccelerationId, mVehicleDataProvider->acceleration().value()); //log acceleration for statistics
		    break;
	}

	mState.velocity = mVehicleDataProvider->speed().value();
	mState.acceleration = mVehicleDataProvider->acceleration().value();
	//jerk is set in control function
}

/**
 * Handles incoming messages, dedicated to the Platooning Service (messages corresponding to this service, need to use the
 * dedicated port, specified in services.xml.
 *
 * If the message is a valid PlatoonMsg, then the stationID of the sender (contained in the message) is used to search the
 * vehicle in the local dynamic map. A vehicle is present if a CAM was received previously from that vehicle (care lifetime!).
 * Dependent of the messageType, the packet is processed:
 *
 * 		Info: the packet is of the type ::Info and informs other vehicles about the current structure in the platoon. A vehicle
 * 			only needs to consider packets of that type if it belongs to the platoon and the packet was sent more recently than
 * 			the locally map was updated. If so, a vehicle shall update its local platoon map.
 *
 * 		Ready: the packet is of the type ::Ready and informs other vehicles about the own readiness to platoon. A vehicle only
 * 			needs to consider packets of that type that are sent from vehicles from behind (use position to check) and if the
 * 			vehicle itself is ready to platoon. Once the vehicle is not member of a platoon or the tail of a platoon it may send
 * 			and ::Invite to the sender of the ::Ready packet by specifying the reference nonce appropriately.
 * 			A vehicle is not allowed to send an invite if an invite was sent previously and no reply was received so far
 * 			(pendingOperation). This could cause platoon inconsistencies.
 *
 * 		Invite: the packet is of the type ::Invite and invites the vehicle by replying to a Ready packet (appropriate reference
 * 			of nonce). A vehicle receiving this message only needs to consider the packet if it is not already member of a platoon
 * 			and a vehicle in front sent the message. Furthermore, the invite is only valid if the reference nonce corresponds
 * 			to the nonce sent within the own ::Ready message. This nonce shall be stored locally in pendingOperation in order to
 * 			compare it at this point in time. If all points hold, a vehicle can accept the invite, adding itself to the platoon
 * 			by updating the own locally stored platoon map accordingly. To inform the inviting vehicle that may be part of a platoon
 * 			a ::InviteAccept has to be sent, using the nonce of the ::Invite message as reference nonce.
 *
 * 		InviteAccept: the packet is of the type ::InviteAccept and confirms that the sending vehicle accepted the ::Invite received
 * 			and is now by the tail of the platoon. A vehicle only needs to consider packets of that type if they belong to the same
 * 			platoon. Once a message of that type is received, the own local platoon map is updated.
 *
 *		@param ind: not used so far.
 *		@param packet: received message, shall be of type PlatoonMsg
 */
void PlatooningService::indicate(const vanetza::btp::DataIndication& ind, omnetpp::cPacket* packet){
    Enter_Method("PlatoonService indicate");
    auto msg = check_and_cast<const PlatoonMsg*>(packet);

    auto mLocalDynamicMap = &getFacilities().get_mutable<artery::LocalDynamicMap>();
    std::pair<bool, vanetza::asn1::Cam> temp = mLocalDynamicMap->getCamByStationId(msg->getTxStationID());
    if(!temp.first){
    	//TODO: no CAM message from predecessor received so far, or CAM is outdated and was removed (lifetime in localdynamicmap)
    	std::cout << "[ATTENTION] " << mVehicleController->getVehicleId() << ": No CAM from predecessor available!" << std::endl;
    	return;
    }
    vanetza::asn1::Cam txcam = temp.second;

    std::string myLane = mVehicleController->getLiteAPI().vehicle().getLaneID(mVehicleController->getVehicleId());
    std::string myEdge = mVehicleController->getLiteAPI().lane().getEdgeID(myLane);

    ReferencePosition_t txpos = (*txcam).cam.camParameters.basicContainer.referencePosition;

    double headingtxrx = calculateHeadingToPostition(txpos);
    double myheading = mVehicleDataProvider->heading().value() * 180 / PI;

    std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId() << ") heading: " << myheading << ", calculated heading (" << (*txcam).header.stationID << ") " << headingtxrx << std::endl;

    switch (msg->getMsgType()){
		case MessageTypePlatoon::Info:
			if(!mPlatoonMap.pendingOperation.first && msg->getTimestamp() > mPlatoonMap.timeStamp && msg->getPlatoonID() == mPlatoonMap.platoonId){

				//check if any changes in map occured. If not, drop packet.
				if(mPlatoonMap.depth == msg->getDepth()){
					bool orderChanged = false;
					for(int i = 0; i < mPlatoonMap.depth; i++){
						if(mPlatoonMap.members.at(i) != msg->getMembers(i)){
							orderChanged = true;
						}
					}
					if(!orderChanged){
						//discard packet
						break;
					}
				}


				std::pair<bool, vanetza::asn1::Cam> temp_1;
				std::pair<bool, vanetza::asn1::Cam> temp_2;
				temp_1.first = false;
				temp_2.first = false;
				//check current predecessor according to map
				if(mPlatoonMap.innerPosition > 0){
					temp_1 = mLocalDynamicMap->getCamByStationId(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
					if(!temp_1.first){
						break;
					}
				}

				//TODO: do not simply override map but current and new map smartly in order to guarantee consistency. (e.g., change in
				//platoon occurs at head and tail. -> race condition in map override.

				//get new map
			    mPlatoonMap.depth = msg->getDepth();
			    for(int i = 0; i < mPlatoonMap.depth; i ++){
					mPlatoonMap.members.at(i) = msg->getMembers(i);
					if(mPlatoonMap.members.at(i) == mVehicleDataProvider->station_id()){
						mPlatoonMap.innerPosition = i;
					}
				}
				std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
						<< "), Received MessageTypePlatoon::Info, Platoon (" << mPlatoonMap.platoonId << ") updated, vehicle "
						<< msg->getTxStationID() << " at position " << msg->getInnerPosition() << std::endl;
				mPlatoonMap.timeStamp = msg->getTimestamp();

				//check predecessor according to updated map.
				if(mPlatoonMap.innerPosition > 0){
					temp_2 = mLocalDynamicMap->getCamByStationId(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
					if(!temp_2.first){
						break;
					}
					vanetza::asn1::Cam camPreviousPredecessor = temp_1.second;
					vanetza::asn1::Cam camPredecessor = temp_2.second;

					//plan new distance once predecessor changed
					if(camPreviousPredecessor->header.stationID != camPredecessor->header.stationID){
						//calculate distance to predecessor.
						StateVector predecessor;
						estimatePredecessorStateBySensorAndCam(predecessor, camPredecessor);
						double predecessor_vehicleLength = camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue / 10.0;
						//TODO: perform dynamic distance planning. For now use initial distance.
						mPlatoonMap.ivDistance = 7.5 + 2.5 + predecessor_vehicleLength; //min distance + mingap + length of predecessor
						double distance_time = (int)(abs(predecessor.distance - mPlatoonMap.ivDistance)/2) + 1;
						//plan trajectory to predecessor.
						mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, distance_time, STEP_SIZE);
						mPlatoonMap.timeDistanceHorizon.current = 0;
					}
				}
			}
			break;

    	case MessageTypePlatoon::Ready:
    	    if(!isOnSameEdgeAndLane(myEdge, myLane, msg->getEdgeID(), msg->getLaneID())){
    	        std::cout << "[ATTENTION] " << mVehicleController->getVehicleId() << ": Not on same edge or lane!" << std::endl;
    	        return;
    	    }
    		if(!mPlatoonMap.pendingOperation.first && mPlatoonMap.readyToPlatoon){
    			//create new Platoon if not already member of one
    			if(!mPlatoonMap.memberOfPlatoon && !isFrontVehicle(myheading, headingtxrx)){
					mPlatoonMap.platoonId = rand();
					mPlatoonMap.memberOfPlatoon = true;
					mPlatoonMap.innerPosition = 0;
					mPlatoonMap.depth = 1;
					mPlatoonMap.members.at(0) = mVehicleDataProvider->station_id();
					mPlatoonMap.pendingOperation.first = true;
					mPlatoonMap.pendingOperationTimer = simTime();
					sendPlatoonMessage(MessageTypePlatoon::Invite, msg->getNonce());
					std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
						<< "), Platoon (" << mPlatoonMap.platoonId << ") Invite sent!" << std::endl;
    			} else if(mPlatoonMap.memberOfPlatoon && 
    					isFrontVehicle(myheading, headingtxrx) && mPlatoonMap.innerPosition > 0
						&& (mPlatoonMap.innerPosition != mPlatoonMap.depth -1)){ //i am not the leader and not the tail- check if another platoon vehicle is closer
    				std::tuple<bool, vanetza::asn1::Cam, omnetpp::SimTime> temp = mLocalDynamicMap->getCamByStationIdWithExpiry(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
    				if(std::get<0>(temp)){ //CAM of pre found
    					vanetza::asn1::Cam camPredecessor = std::get<1>(temp);
    					StateVector predecessor = camToStateVector(camPredecessor);
    					StateVector txVehicle = camToStateVector(txcam);
    					std::cout << "tx distance: " << txVehicle.distance << "follower distance: " << std::endl;
    					if(txVehicle.distance < predecessor.distance){
    						mPlatoonMap.pendingOperation.first = true; //multiple pending invites are not possible -> avoids platoon inconsistency
							mPlatoonMap.pendingOperationTimer = simTime();
							sendPlatoonMessage(MessageTypePlatoon::Invite, msg->getNonce());
							std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
									<< "), Platoon (" << mPlatoonMap.platoonId << ") Invite sent!" << std::endl;
    					}
    				}
    			} else if(mPlatoonMap.memberOfPlatoon &&
    					!isFrontVehicle(myheading, headingtxrx) && mPlatoonMap.innerPosition == mPlatoonMap.depth - 1){ //i am tail - invite the vehicle
        			mPlatoonMap.pendingOperation.first = true; //multiple pending invites are not possible -> avoids platoon inconsistency
        			mPlatoonMap.pendingOperationTimer = simTime();
        			sendPlatoonMessage(MessageTypePlatoon::Invite, msg->getNonce());
        			std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
        					<< "), Platoon (" << mPlatoonMap.platoonId << ") Invite sent!" << std::endl;
    			}
    		}
    		break;

    	case MessageTypePlatoon::Invite:
    		if(!mPlatoonMap.memberOfPlatoon && mPlatoonMap.readyToPlatoon
    			&& msg->getNonceRef() == mPlatoonMap.pendingOperation.second){

    			mPlatoonMap.platoonId = msg->getPlatoonID();
    			mPlatoonMap.memberOfPlatoon = true;
    			if(isFrontVehicle(myheading, headingtxrx)){
    				mPlatoonMap.innerPosition = msg->getInnerPosition() + 1;
    			} else{
    				mPlatoonMap.innerPosition = msg->getInnerPosition();
    			}

    			mPlatoonMap.depth = msg->getDepth() + 1;

    			//update local map
    			//insert vehicles before me
    			for(int i = 0; i < mPlatoonMap.innerPosition; i++){
    			    mPlatoonMap.members.at(i) = msg->getMembers(i);
    			}
    			//insert myself
    			mPlatoonMap.members.at(mPlatoonMap.innerPosition) = mVehicleDataProvider->station_id();
    			//insert all vehicles after me
    			for(int i = mPlatoonMap.innerPosition + 1; i < mPlatoonMap.depth ; i++){
    			    mPlatoonMap.members.at(i) = msg->getMembers(i-1);
    			}


    			sendPlatoonMessage(MessageTypePlatoon::InviteAccept, msg->getNonce());
    			std::cout << "(" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId()
    			    					<< "), Platoon (" << mPlatoonMap.platoonId << ") MessageTypePlatoon::InviteAccept!" << std::endl;

    			std::pair<bool, vanetza::asn1::Cam> temp = mLocalDynamicMap->getCamByStationId(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
				if(!temp.first){
					//TODO: restart platooning creation procedure after a certain timeout if PRR is too high!
					break;
				}
				vanetza::asn1::Cam camPredecessor = temp.second;

				//calculate distance to predecessor and plan trajectory (set ivDistance accordingly).
				StateVector predecessor;
				estimatePredecessorStateBySensorAndCam(predecessor, camPredecessor);
				//plan trajectory to tail of platoon.
				double distance_time = (int)(abs(predecessor.distance - mPlatoonMap.ivDistance)/2) + 1;
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, distance_time, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0;
    		}
    		break;

    	case MessageTypePlatoon::InviteAccept:
    		//once the accept comes from a vehicle of the same platoon it has to be added to the local map
    		if(mPlatoonMap.memberOfPlatoon && mPlatoonMap.platoonId == msg->getPlatoonID()
    				&& mPlatoonMap.pendingOperation.first && msg->getNonceRef() == mPlatoonMap.pendingOperation.second){

    			//TODO: do not simply override map but current and new map smartly in order to guarantee consistency. (e.g., change in
    			//platoon occurs at head and tail. -> race condition in map override.

    			std::pair<bool, vanetza::asn1::Cam> temp_1;
				std::pair<bool, vanetza::asn1::Cam> temp_2;
				temp_1.first = false;
				temp_2.first = false;
				//check current predecessor according to map
				if(mPlatoonMap.innerPosition > 0){
					temp_1 = mLocalDynamicMap->getCamByStationId(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
					if(!temp_1.first){
						break;
					}
				}

				mPlatoonMap.depth = msg->getDepth();
				mPlatoonMap.timeStamp = msg->getTimestamp();

				for(int i = 0; i < mPlatoonMap.depth; i ++){
				    mPlatoonMap.members.at(i) = msg->getMembers(i);
				    if(mPlatoonMap.members.at(i) == mVehicleDataProvider->station_id()){
				        mPlatoonMap.innerPosition = i;
				    }
				}

				mPlatoonMap.pendingOperation.first = false;

				//check predecessor according to updated map.
				if(mPlatoonMap.innerPosition > 0){
					temp_2 = mLocalDynamicMap->getCamByStationId(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
					if(!temp_2.first){
						break;
					}
					vanetza::asn1::Cam camPreviousPredecessor = temp_1.second;
					vanetza::asn1::Cam camPredecessor = temp_2.second;

					//plan new distance once predecessor changed
					if(camPreviousPredecessor->header.stationID != camPredecessor->header.stationID){
						//calculate distance to predecessor.
						StateVector predecessor;
						estimatePredecessorStateBySensorAndCam(predecessor, camPredecessor);
						double predecessor_vehicleLength = camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue / 10.0;
						//TODO: perform dynamic distance planning. For now use initial distance.
						mPlatoonMap.ivDistance = 7.5 + 2.5 + predecessor_vehicleLength; //min distance + mingap + length of predecessor
						double distance_time = (int)(abs(predecessor.distance - mPlatoonMap.ivDistance)/2) + 1;
						//plan trajectory to predecessor.
						mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, distance_time, STEP_SIZE);
						mPlatoonMap.timeDistanceHorizon.current = 0;
					}
				}
    		}

    		break;
    }

    printPlatoonMap();

    delete msg;
}

/**
 * Calculates the own heading based on the own position regarding the position pos.
 *
 * 		@param pos: Position to which the heading is calculated, e.g. position of another vehicle.
 *
 * 		@return: heading angle between own position and position pos in degrees (0° - 360°)
 */
double PlatooningService::calculateHeadingToPostition(ReferencePosition_t pos){
	DoublePosition myPos = geoAngleToReferencePosition(mVehicleDataProvider->latitude(), mVehicleDataProvider->longitude());
	DoublePosition otherPos;
	//convert to radians as atan2 takes radians
	otherPos.latitude = pos.latitude/10000000.0 * PI/180;
	otherPos.longitude = pos.longitude/10000000.0 * PI/180;
	myPos.latitude = myPos.latitude * PI/180;
	myPos.longitude = myPos.longitude * PI/180;


	//heading calculation
	double X = cos(otherPos.latitude) * sin(otherPos.longitude - myPos.longitude);
	double Y = cos(myPos.latitude) * sin(otherPos.latitude) - sin(myPos.latitude) * cos(otherPos.latitude) * cos(otherPos.longitude - myPos.longitude);
	double heading = atan2(X,Y)* 180 / PI;
	//convert into range from 0° - 360°
	if(heading < 0){
		heading = heading + 360;
	}
	return heading;
}

/**
 * Prepares the platoon message of the type msgType and sends it as Single Hop Broadcast message (SHB).
 * Thus, the message is filled with current information about the platoon the vehicle belongs to,
 * together with a timestamp.
 * If a reference nonce different than 0 is defined, the message is answering the message with the same nonce.
 *
 * 		@param msgType: Specifies the type of the message
 * 		@param nonceRef: If different than 0, the message is the answer to a previously send message with the same nonce.
 */
void PlatooningService::sendPlatoonMessage(MessageTypePlatoon msgType, uint32_t nonceRef){
	auto msg = new PlatoonMsg();
	msg->setMsgType(msgType);
	msg->setTxStationID(mVehicleDataProvider->station_id());
	msg->setPlatoonID(mPlatoonMap.platoonId);
	msg->setInnerPosition(mPlatoonMap.innerPosition);
	msg->setDepth(mPlatoonMap.depth);
	if(msgType == MessageTypePlatoon::Info){
		msg->setTimestamp(mPlatoonMap.timeStamp);
	}else{
		msg->setTimestamp(simTime());
	}
	msg->setIvDistance(mPlatoonMap.ivDistance);
	msg->setLaneID((mVehicleController->getLiteAPI().vehicle().getLaneID(mVehicleController->getVehicleId())).c_str());
	msg->setEdgeID((mVehicleController->getLiteAPI().lane().getEdgeID(msg->getLaneID())).c_str());
	msg->setMembersArraySize(mPlatoonMap.depth);
	for(int i = 0; i < mPlatoonMap.depth; i ++){
	    msg->setMembers(i, mPlatoonMap.members.at(i));
	}

	mPlatoonMap.pendingOperation.second = rand();
	msg->setNonce(mPlatoonMap.pendingOperation.second);
	msg->setNonceRef(nonceRef);

	btp::DataRequestB req;
	req.destination_port = host_cast<PlatooningService::port_type>(getPortNumber());
	req.gn.transport_type = geonet::TransportType::SHB;
	req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
	req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

	request(req, msg);
}

/**
 * Prints the currently locally available platoon map of the vehicle.
 */
void PlatooningService::printPlatoonMap(){
	std::cout << "-------------------------------------------------" << std::endl;
	std::cout << "[" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId() << "] \t PlatoonID: "
			<< mPlatoonMap.platoonId << "\t InnerPosition: " << mPlatoonMap.innerPosition << "\t Depth: "
			<< mPlatoonMap.depth << "\t Timestamp: " << mPlatoonMap.timeStamp << "\t ivDistance: "
			<< mPlatoonMap.ivDistance << std::endl;
	for(int i = 0; i < mPlatoonMap.depth; i++){
		std::cout << "\t" << i << ": " << mPlatoonMap.members.at(i) << std::endl;
	}
	std::cout << "-------------------------------------------------" << std::endl;
}

/**
 * Applies timeheadway policy with CAM from predecessor and leader.
 * u_i = u_0/pos_i + u_i-1 + kd(v_i-1 -v_i) + kp(x_i-1 - x_i - th*v_i)
 */
std::tuple<double, double> PlatooningService::timeHeadwayPredecessorLeaderPolicy(StateVector &predecessor, vanetza::asn1::Cam &camPredecessor, double my_v){
	double e_distance = 0;
	double predecessor_length = camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.vehicleLength.vehicleLengthValue / 10.0;
	if(mPlatoonMap.timeDistanceHorizon.current != -1){
		e_distance = predecessor.distance - mPlatoonMap.timeDistanceHorizon.mapping.at(mPlatoonMap.timeDistanceHorizon.current*STEP_SIZE) - my_v*TH;
		mPlatoonMap.timeDistanceHorizon.current = mPlatoonMap.timeDistanceHorizon.current + 1;
		if(mPlatoonMap.timeDistanceHorizon.current >= mPlatoonMap.timeDistanceHorizon.mapping.size()){
			mPlatoonMap.timeDistanceHorizon.current = -1; //once all steps are processed, set time horizon invalid.
		}
	} else{
		e_distance = predecessor.distance - mPlatoonMap.ivDistance - my_v*TH;
	}
	double e_velocity = predecessor.velocity - my_v;
	double e_acceleration = 0;

	e_acceleration = predecessor.acceleration/STEP_SIZE; //remove this once if statement is commented in (consider leader acceleration and predecessor)

	double u_i = e_acceleration + (e_distance)*(KP*(exp(-1*(predecessor.distance - 11.0 - predecessor_length))) + 0.1)+ (e_velocity)*KD;
	std::cout << "u_i = " << u_i << "  acc = " << e_acceleration << "  e_distance = "<< e_distance <<"  e_velocity = " << e_velocity << std::endl;

	u_i = u_i * STEP_SIZE; //transform acceleration to velocity by integrating it over time.

	std::tuple<double, double> rettuple = std::make_tuple(u_i, predecessor.distance);
	return rettuple;
}

std::map<double, double> PlatooningService::createRefTrajectory(double x0, double xt, double t0, double t, double ts){
	std::map<double, double> x_ref;
	x_ref.insert(std::pair<double,double>(t0,x0));
	unsigned int number_of_elements = (unsigned int)(t-t0)/ts + 1;
	for(int i = 1; i < number_of_elements; i++){
		double current_time = t0 + i*ts;
		double scale = current_time/(t-t0);
		double current_value = x0 + (xt-x0)*(35*pow(scale, 4) - 84*pow(scale, 5) + 70*pow(scale, 6) - 20*pow(scale, 7));
		x_ref.insert(std::pair<double, double>(current_time, current_value));
	}
	return x_ref;
}

StateVector PlatooningService::camToStateVector(vanetza::asn1::Cam camPredecessor){
	StateVector predecessor;
	DoublePosition myPos = geoAngleToReferencePosition(mVehicleDataProvider->latitude(), mVehicleDataProvider->longitude());
	DoublePosition prePos;
	prePos.latitude = camPredecessor->cam.camParameters.basicContainer.referencePosition.latitude/10000000.0;
	prePos.longitude = camPredecessor->cam.camParameters.basicContainer.referencePosition.longitude/10000000.0;
	predecessor.distance = distanceEarth(myPos.latitude, myPos.longitude, prePos.latitude, prePos.longitude);
	predecessor.velocity = camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.speed.speedValue *0.01;
	predecessor.acceleration = camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.longitudinalAcceleration.longitudinalAccelerationValue;
	predecessor.heading = camPredecessor->cam.camParameters.highFrequencyContainer.choice.basicVehicleContainerHighFrequency.heading.headingValue * 0.1;

	return predecessor;
}

void PlatooningService::considerScenario(StateVector &predecessor, vanetza::asn1::Cam &camPredecessor, std::string scenario){
	int scenario_val = atoi(scenario.c_str());
	double currentSimTime = simTime().raw()/(double)simTime().getScale();
	double v_ref;

	if(mPlatoonMap.memberOfPlatoon && mPlatoonMap.innerPosition != 0){
		estimatePredecessorStateBySensorAndCam(predecessor, camPredecessor);
	}

	switch (scenario_val){
	case 1: //scenario_0001
		if(mVehicleController->getVehicleId() == "veh0"){
			//emergency braking scenario
			if(currentSimTime > 80 && currentSimTime <= 85){
				v_ref = 0;
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), v_ref, STEP_SIZE*1000); //care it is in ms!
			} else if(currentSimTime > 85){
				v_ref = 10.0;
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), v_ref, STEP_SIZE*1000); //care it is in ms!
			}
		} else if(mVehicleController->getVehicleId() == "veh1"){
			if(currentSimTime > 30 && currentSimTime <= 30 + STEP_SIZE){
				mPlatoonMap.ivDistance = 20; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 10, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			} else if(currentSimTime > 60 && currentSimTime <= 60 + STEP_SIZE){
				mPlatoonMap.ivDistance = 15; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 5, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			}
		}
		break;
	case 2: //scenario_0002
		if(mVehicleController->getVehicleId() == "veh0"){
			//emergency braking scenario
			if(currentSimTime > 80 && currentSimTime <= 85){
				v_ref = 0;
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), v_ref, STEP_SIZE*1000); //care it is in ms!
			} else if(currentSimTime > 85){
				v_ref = 10.0;
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), v_ref, STEP_SIZE*1000); //care it is in ms!
			}
		} else if(mVehicleController->getVehicleId() == "veh1"){
			if(currentSimTime > 30 && currentSimTime <= 30 + STEP_SIZE){
				mPlatoonMap.ivDistance = 20; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 10, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			} else if(currentSimTime > 60 && currentSimTime <= 60 + STEP_SIZE){
				mPlatoonMap.ivDistance = 15; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 5, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			}
		} else if(mVehicleController->getVehicleId() == "veh2"){
			if(currentSimTime > 25 && currentSimTime <= 25 + STEP_SIZE){
				mPlatoonMap.ivDistance = 15; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 5, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			} else if(currentSimTime > 45 && currentSimTime <= 45 + STEP_SIZE){
				mPlatoonMap.ivDistance = 12; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 3, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			}
		} else if(mVehicleController->getVehicleId() == "veh3"){
			if(currentSimTime > 35 && currentSimTime <= 35 + STEP_SIZE){
				mPlatoonMap.ivDistance = 18; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 8, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			} else if(currentSimTime > 60 && currentSimTime <= 60 + STEP_SIZE){
				mPlatoonMap.ivDistance = 10; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 8, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			}
		}
		break;

	case 3:{ //scenario_0003
		CaPlatooningService* caPlatService = check_and_cast<CaPlatooningService*>(getParentModule()->getSubmodule("CaPlatooningService", -1));
		double momentOfFullBrake = 10.0;
		if(mVehicleController->getVehicleId() == "veh0"){
			//emergency braking scenario
			if(currentSimTime > momentOfFullBrake && currentSimTime <= momentOfFullBrake + 10.0){
				v_ref = 0;
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), v_ref, STEP_SIZE*1000); //care it is in ms!
			} else if(currentSimTime > 45){
				v_ref = 14.0;
				mVehicleController->getLiteAPI().vehicle().slowDown(mVehicleController->getVehicleId(), v_ref, STEP_SIZE*1000); //care it is in ms!
			}
		} else if(mVehicleController->getVehicleId() != "veh0"){
			if(currentSimTime < momentOfFullBrake){
				caPlatService->setMomentOfFullBrake(SimTime(momentOfFullBrake));
			}

			if(currentSimTime > 80 && currentSimTime <= 80 + STEP_SIZE){
				mPlatoonMap.ivDistance = 20; //target distance
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, 10, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			}
		}
		break;
	} //case 3 break to limit scope

	case 4: //scenario_0004
		if(mVehicleController->getVehicleId() == "veh0"){

		} else if(mVehicleController->getVehicleId() == "veh3"){
			if(currentSimTime > 11 && currentSimTime <= 11 + STEP_SIZE){
				mPlatoonMap.ivDistance = 66.5; //target distance
				double distance_time = (int)(abs(predecessor.distance - mPlatoonMap.ivDistance)/2) + 1;
				mPlatoonMap.timeDistanceHorizon.mapping = createRefTrajectory(predecessor.distance, mPlatoonMap.ivDistance, 0, distance_time, STEP_SIZE);
				mPlatoonMap.timeDistanceHorizon.current = 0; //set time horizon valid.
			}
		}
		break;

	default:
		break;
	}

}

void PlatooningService::estimatePredecessorStateBySensorAndCam(StateVector &predecessor, vanetza::asn1::Cam &camPredecessor){
	std::tuple<bool, vanetza::asn1::Cam, omnetpp::SimTime> temp = mLocalDynamicMap->getCamByStationIdWithExpiry(mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1));
	if(!std::get<0>(temp)){ //if no CAM is present (lifetime expired)
		//assume vehicle in front of you has no acceleration and a velocity that is the same as yours (no influence on controller)
		predecessor.velocity = mVehicleDataProvider->speed() / boost::units::si::meter_per_second;
		predecessor.acceleration = 0;

		//check if sensor data is available
		for(auto& obj : mlocalEnvmod->allObjects()){
			const std::weak_ptr<artery::EnvironmentModelObject> obj_ptr = obj.first;
			if (obj_ptr.expired()) continue;
			const auto& vd = obj_ptr.lock()->getVehicleData();
			if(vd.station_id() == mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1)){ //is it the predecessor?
				DoublePosition myPos = geoAngleToReferencePosition(mVehicleDataProvider->latitude(), mVehicleDataProvider->longitude());
				DoublePosition prePos = geoAngleToReferencePosition(vd.latitude(), vd.longitude());
				predecessor.distance = distanceEarth(myPos.latitude, myPos.longitude, prePos.latitude, prePos.longitude);
				break;
			} else{
				//TODO: it may makes sense to set this to max. sensor range
				predecessor.distance = D_MIN_INIT; // distance is not known
			}
		}
	} else{
		//TODO: check for vehicles in front that may not be part of the platoon

		camPredecessor = std::get<1>(temp);
		predecessor = camToStateVector(camPredecessor);

		DoublePosition myPos = geoAngleToReferencePosition(mVehicleDataProvider->latitude(), mVehicleDataProvider->longitude());

		//get all objects and calculate their distance
		std::vector<std::pair<double, uint32_t>> objectsAroundMe;
		for(auto& obj : mlocalEnvmod->allObjects()){
			const std::weak_ptr<artery::EnvironmentModelObject> obj_ptr = obj.first;
			if (obj_ptr.expired()) continue;
			const VehicleDataProvider& vd = obj_ptr.lock()->getVehicleData();
			DoublePosition prePos = geoAngleToReferencePosition(vd.latitude(), vd.longitude());
			double tmp_distance = distanceEarth(myPos.latitude, myPos.longitude, prePos.latitude, prePos.longitude);
			objectsAroundMe.push_back(std::make_pair(tmp_distance, vd.station_id()));
		}

		//sort by distance
		std::sort(objectsAroundMe.begin(), objectsAroundMe.end());

		bool predecessorSensed = false;
		for(std::vector<std::pair<double, uint32_t>>::iterator it = objectsAroundMe.begin(); it != objectsAroundMe.end(); ++it) {
			if(it->second == mPlatoonMap.members.at(mPlatoonMap.innerPosition - 1)){
				predecessor.distance = it->first;
				predecessorSensed = true;
				std::cout << mVehicleDataProvider->station_id() << " found predecessor!" << std::endl;
				break;
			}
		}

		if(!predecessorSensed){
			//once predecessor could not be sensed, eliminate distance influence of controller.
			//TODO: what to do if vehicle in between does not want to platoon?
			//e.g. increase distance to 70m?

			double camTime = (mVehicleDataProvider->updated() - (std::get<2>(temp) - SimTime(1.100))).dbl();
			std::cout << mVehicleDataProvider->updated().dbl() << " " << std::get<2>(temp).dbl() << " " << (std::get<2>(temp) - SimTime(1.100)).dbl() << std::endl;
			double ds = 0.5 * predecessor.acceleration * pow(camTime, 2) + predecessor.velocity * camTime;
			double dx = ds*cos(predecessor.heading*M_PI/180);
			double dy = ds*sin(predecessor.heading*M_PI/180);
			DoublePosition prePos;
			prePos.latitude = camPredecessor->cam.camParameters.basicContainer.referencePosition.latitude/10000000.0;
			prePos.longitude = camPredecessor->cam.camParameters.basicContainer.referencePosition.longitude/10000000.0;
			prePos.latitude = prePos.latitude + (dy / 6378137) * (180 / M_PI);
			prePos.longitude = prePos.longitude + (dx / 6378137) * (180 / M_PI) / cos(prePos.latitude * M_PI/180);

			std::cout << "Predecessor distance corrected from " << predecessor.distance;
			predecessor.distance = distanceEarth(myPos.latitude, myPos.longitude, prePos.latitude, prePos.longitude);
			std::cout << " to " << predecessor.distance << " because of time "<< camTime << std::endl;

		}
	}
}

unsigned int PlatooningService::getAsilDCounter(){
	return mAsilDCounter;
}

double PlatooningService::getAsilDDistance(){
	return mAsilDDistance;
}


/**
 * Transforms the position expressed as geo angles (latitude, longitude) in a double position (same unit as it is used in
 * CAM messages). Therefore, direct comparisons are possible.
 *
 * 		@param lat: latitude value
 * 		@param lon: longitude value
 *
 * 		@return: transformed position as a DoublePosition.
 */
DoublePosition geoAngleToReferencePosition(vanetza::units::GeoAngle lat, vanetza::units::GeoAngle lon){
	auto microdegree = vanetza::units::degree * boost::units::si::micro;
	DoublePosition myPos;
	myPos.latitude = ((round(lat, microdegree) * Latitude_oneMicrodegreeNorth))/10000000.0;
	myPos.longitude = ((round(lon, microdegree) * Longitude_oneMicrodegreeEast))/10000000.0;
	return myPos;
}

/**
 * Comparing headings of the ego vehicle and a second vehicle, deciding if the second vehicle is in front or not.
 * The vehicle is considered to be in front of the ego vehicle, once the the second vehicle's heading is in range of
 * (-90° and + 90°) of the ego vehicle's heading.
 *
 * 		@param myheading: heading of the ego vehicle
 * 		@param itsheading: heading of the second vehicle
 *
 * 		@return: true, if the second vehicle is in front of the ego vehicle
 * 				 false, if the second vehicle is behind the ego vehicle
 */
bool isFrontVehicle(double myheading, double itsheading){
	//TODO: eventually add here comparison of driving direction and laneID
	if((myheading + 90 > itsheading) && (myheading - 90 < itsheading)){
		return true;
	} else{
		return false;
	}
}

bool isOnSameEdgeAndLane(std::string myEdge, std::string myLane, std::string itsEdge, std::string itsLane){
    bool retval = false;
    if (myEdge.compare(itsEdge) == 0 && myLane.compare(itsLane) == 0){
        retval = true;
    }
    return retval;
}

std::vector<double> ivDistancePlanning(double d_0, double d_des, double timehorizon, double t_s){
	std::vector<double> d_planned (floor(timehorizon/t_s) + 1);
	d_planned.at(0) = d_0;
	for(int i = 1; i < d_planned.size(); i++){
		double t = (i-1)*t_s;
		d_planned.at(i) = d_0 + (d_des-d_0)*(10*(pow(t/timehorizon, 3)) - 15*(pow(t/timehorizon, 4)) + 6*(pow(t/timehorizon, 5)));
	}
	return d_planned;
}

std::map<double,double> derive(std::map<double, double> fx){
	std::map<double,double> fxdot;
	fxdot.insert(std::pair<double,double>(0,0));
	for(int i = 1; i < fx.size(); i++){
		double derivative = (fx.at(i*STEP_SIZE) - fx.at((i-1)*STEP_SIZE))/STEP_SIZE;
		fxdot.insert(std::pair<double,double>(i*STEP_SIZE, derivative));
		std::cout << derivative << std::endl;
	}
	std::cout << std::endl;
	return fxdot;
}


} //namespace artery