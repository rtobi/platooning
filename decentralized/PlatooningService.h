/*
 * Decentralized Platooning Service
 * Copyright 2018 Tobias Renzler
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */
 
#ifndef PLATOONINGSERVICE_H_
#define PLATOONINGSERVICE_H_

#include "artery/application/ItsG5Service.h"
#include "artery/application/VehicleDataProvider.h"

#define MAX_LOOKAHEAD_DISTANCE 100
#define MAX_VEHICLES_PER_PLATOON 100
#define D_MIN_INIT 5.0
#define KP 0.1
#define KD 1.0
#define TH 0.0

// forward declaration
namespace traci { class VehicleController; }


//define outside namespace artery to avoid autocompilation conflicts
/**
 * Definition of different message types of a PlatoonMsg.
 * 		Info: Status information of the platoon
 * 		Ready: Sender is ready to platoon
 * 		Invite: Sender invites the receiver (nonceRef to target only one receiver) to join the platoon
 * 		InviteAccept: Sender accepts the previously received Invite.
 */
enum MessageTypePlatoon{
	Info,
	Ready,
	Invite,
	InviteAccept
};

namespace artery{

class LocalEnvironmentModel;

/**
 * Datatype that allows easy comparison between ReferencePosition_t (ego position) and longs (contained in CAM of other vehicle)
 */
struct DoublePosition{
	double longitude;
	double latitude;
};

struct TimeDistanceHorizon{
		int current;
		std::map<double, double> mapping;
		std::map<double, double> kaMapping;
		std::map<double, double> kvMapping;
		std::map<double, double> ksMapping;
};

/**
 * Datatype that manages the locally stored platoon information. Its freshness is determined by timeStamp. Updates can be
 * selftriggered or may be triggered by the reception of CAMs of other vehicles.
 * 		platoonId: id of the platoon, (randomly) selected by the leader on platoon generation time.
 * 		depth: length of the platoon, equal number of members
 * 		innerPosition: position of the ego vehicle in the platoon, position l0 stands for the leader
 * 		timeStamp: time when the last update took place
 * 		readyToPlatoon: flag to indicate readiness to platoon (true)
 * 		memberOfPlatoon: flag to indicate that vehicle is part of a platoon (true)
 * 		pendingOperation: bool to indicate if a invite is pending  (true), meaning no inviteAccept was receieved so far
 * 						  uint32_t to store the nonce of the last sent message. Once reply is received, a comparison with
 * 						  nonceRef can take place.
 * 		members: storage for the station ids of all member of the platoon. It has the size of depth. Once a new vehicle joined
 * 				 a platoon recently it may occur, that station id is not know. This will be updated with Info messages such
 * 				 that after a certain time the platoon map is up to date.
 */
struct PlatoonMap{
	uint32_t platoonId;
	uint32_t depth;
	uint32_t innerPosition;
	omnetpp::SimTime timeStamp;
	bool readyToPlatoon;
	bool memberOfPlatoon;
	std::pair<bool, uint32_t> pendingOperation;
	omnetpp::SimTime pendingOperationTimer;
	std::vector<uint32_t> members;
	double ivDistance;
	struct TimeDistanceHorizon timeDistanceHorizon;
	std::vector<double> u_i_values;
};

struct StateVector{
	double distance;
	double velocity;
	double acceleration;
	double jerk;
	double heading;
};

class PlatooningService : public artery::ItsG5Service
{
    public:
        //PlatooningService();
        void trigger() override;
        unsigned int getAsilDCounter();
        double getAsilDDistance();


    protected:
        void initialize() override;
        traci::VehicleController* mVehicleController = nullptr;

        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
		double calculateHeadingToPostition(ReferencePosition_t pos);
		void sendPlatoonMessage(MessageTypePlatoon msgType, uint32_t nonceRef);
		void printPlatoonMap();
		void platoonControl();
		std::tuple<double, double> timeHeadwayPredecessorLeaderPolicy(StateVector &predecessor, vanetza::asn1::Cam &camPredecessor, double my_v);
		std::map<double, double> createRefTrajectory(double x0, double xt, double t0, double t, double ts);
		StateVector camToStateVector(vanetza::asn1::Cam camPredecessor);
		void considerScenario(StateVector &predecessor, vanetza::asn1::Cam &camPredecessor, std::string scenario);
		void estimatePredecessorStateBySensorAndCam(StateVector &predecessor, vanetza::asn1::Cam &camPredecessor);

    private:
        const VehicleDataProvider* mVehicleDataProvider;
        omnetpp::SimTime mRTP;
        omnetpp::SimTime mLastRTP;
        omnetpp::SimTime mCTRL;
        omnetpp::SimTime mLastCTRL;
        PlatoonMap mPlatoonMap;
        LocalDynamicMap* mLocalDynamicMap;
        StateVector mState;
        unsigned int mAsilDCounter;
        double mAsilDDistance;

        //signals
        omnetpp::simsignal_t ivDistanceId;
        omnetpp::simsignal_t vehicleVelocityId;
        omnetpp::simsignal_t vehicleAccelerationId;

        //controller gains
    	double k_a;
    	double k_v;
    	double k_s;

    	const artery::LocalEnvironmentModel* mlocalEnvmod;

};


DoublePosition geoAngleToReferencePosition(vanetza::units::GeoAngle lat, vanetza::units::GeoAngle lon);
bool isFrontVehicle(double myheading, double itsheading);
bool isOnSameEdgeAndLane(std::string myEdge, std::string myLane, std::string itsEdge, std::string itsLane);
std::vector<double> ivDistancePlanning(double d_0, double d_des, double timehorizon, double t_s);
std::map<double,double> derive(std::map<double, double> fx);

} //namespace artery

#endif /* PLATOONINGSERVICE_H_ */