/*
 * FeudalisticPlatooningService.cc
 *
 *      Author: rtobi
 */
#include <artery/application/platooning/FeudalisticPlatooningService.h>
#include "artery/traci/VehicleController.h"
#include "artery/application/Asn1PacketVisitor.h"
#include <omnetpp/checkandcast.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include "MessageHelpers.h"
#include "artery/application/CaPlatooningService.h"
#include <fstream>

namespace artery{
namespace platooning{
using namespace omnetpp;
using namespace vanetza;

static const simsignal_t scSignalBridgeExport = cComponent::registerSignal("BridgeExport");
static const simsignal_t scSignalBridgeImport = cComponent::registerSignal("BridgeImport");
static const simsignal_t scSignalLordShipping = cComponent::registerSignal("LordShipping");
static const simsignal_t scSignalLordOrdering = cComponent::registerSignal("LordOrdering");
static const simsignal_t scSignalLordTrading = cComponent::registerSignal("LordTrading");

static const simsignal_t scSignalTXInitWLAN = cComponent::registerSignal("TXInitWLAN");
static const simsignal_t scSignalRXInitWLAN = cComponent::registerSignal("RXInitWLAN");

static const simsignal_t scSignalLteAddr = cComponent::registerSignal("LteAddrVeh");
static const simsignal_t scSignalSubPlatoonRole = cComponent::registerSignal("FeudalRole");
static const simsignal_t scSignalSubPlatoonId = cComponent::registerSignal("VassalageId");

Define_Module(FeudalisticPlatooningService)

/*FeudalisticPlatooningService::FeudalisticPlatooningService(){

}*/
void FeudalisticPlatooningService::initialize(){
    ItsG5BaseService::initialize();
    mVehicleController = getFacilities().get_mutable_ptr<traci::VehicleController>();
    mVehicleDataProvider = &getFacilities().get_const<VehicleDataProvider>();
    if(getParentModule()->getSubmodule("PlatooningService", -1)){
    	mPlatooningService = check_and_cast<PlatooningService*>(getParentModule()->getSubmodule("PlatooningService", -1));
    }
    if(getParentModule()->getSubmodule("PlatooningServiceEnvmod", -1)){
    	mPlatooningServiceEnvmod = check_and_cast<PlatooningServiceEnvmod*>(getParentModule()->getSubmodule("PlatooningServiceEnvmod", -1));
	}

    if(getParentModule()->getSubmodule("DenService", -1)){
    	mDenService = check_and_cast<DenService*>(getParentModule()->getSubmodule("DenService", -1));
    }

    mTimer = &getFacilities().get_const<Timer>();
    mLocalDynamicMap = &getFacilities().get_mutable<artery::LocalDynamicMap>();  //init local awareness map

    // timers
    mFeudalisticPlatoonTimeOut.interval = uint64_t(SimTime(par("feudalisticTimeOut")).dbl());
    mFeudalisticPlatoonTimeOut.last = countTaiMilliseconds(mTimer->getCurrentTime());
    mVassalageTimeMerchant.interval = uint64_t(SimTime(par("vassalageMerchantInterval")).dbl());
    mVassalageTimeMerchant.last = countTaiMilliseconds(mTimer->getCurrentTime());
    mVassalageTimeLordShipping.interval = uint64_t(SimTime(par("vassalageLordShippingInterval")).dbl());
    mVassalageTimeLordShipping.last = countTaiMilliseconds(mTimer->getCurrentTime());
    mVassalageTimeLordOrdering.interval = uint64_t(SimTime(par("vassalageLordOrderingInterval")).dbl());
    mVassalageTimeLordOrdering.last = countTaiMilliseconds(mTimer->getCurrentTime());
    mSyncTiming.timing.interval = uint64_t(SimTime(par("feudalisticInitMaxBackoff")).dbl());
    mSyncTiming.timing.last = countTaiMilliseconds(mTimer->getCurrentTime());
    mSyncTiming.flag = false;

    // init FeudalisticPlatoonMap
    mVassalMap.platoonId = 0;
    mVassalMap.vassalageId = 0;
    mVassalMap.depth = par("vassalageSize");
    mVassalMap.divideFactor = par("vassalageDivideFactor");
    mVassalMap.ivDistanceInner = 0;
    mVassalMap.role = 0;
    mVassalMap.timeStamp = SimTime(0);
    mVassalMap.role = RoleFeudalisticPlatoon::Outlaw;
    mVassalMap.ref = -1; // no active ref

    // get LTE availability from envmod.car or symbiosis.car
    // can be set in .ini file
    mLteAvailable = getParentModule()->getParentModule()->par("lteAvailable").boolValue();

    // colorset
    mColorset = Colorsets().getColorSet(par("numberOfColorSet"));
    assert(RoleFeudalisticPlatoon::NumberOfRoles <= mColorset.size());

}

void FeudalisticPlatooningService::trigger(){
	uint64_t taiTimeNow = countTaiMilliseconds(mTimer->getTimeFor(simTime()));

	auto platService = (mPlatooningService > mPlatooningServiceEnvmod) ? mPlatooningService : mPlatooningServiceEnvmod;
	mPlatMap = platService->getPlatoonMap();

	// get own L3 address if not already present
	if(mLteAvailable && !mL3Address.toIPv4().getInt()){
		std::string nodepath = getParentModule()->getParentModule()->getFullPath();
		mL3Address = inet::L3AddressResolver().resolve(nodepath.c_str());
        // statistics: publish once the lte addresses of all vehicles to simplify tx/rx relations
        emit(scSignalLteAddr, mL3Address.toIPv4().getInt());
	}

	if(mPlatMap.depth >= mVassalMap.divideFactor*mVassalMap.depth
			&& mVassalMap.role == RoleFeudalisticPlatoon::Outlaw){
		setupFeudalisticPlatooning();
	}

	if(mSyncTiming.flag && syncReBroadcastMsg){
		if((taiTimeNow - mSyncTiming.timing.last) > mSyncTiming.timing.interval){
			mSyncTiming.flag = false;
			// create request
			btp::DataRequestB req;
			req.destination_port = host_cast<FeudalisticPlatooningService::port_type>(getPortNumber());
			req.gn.transport_type = geonet::TransportType::SHB;
			req.gn.maximum_hop_limit = 0;
			req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
			req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

			//emit(scSignalTXInitWLAN, syncReBroadcastMsg->dup());
			request(req, syncReBroadcastMsg->dup());
			delete syncReBroadcastMsg;
			std::cout << mVehicleDataProvider->station_id() << "(" << mVehicleController->getVehicleId() << ") "
					<< "sending syncReBroadcast ::Init message(" << mVassalMap.ref
					<< ")!" << std::endl;
		}
	}

	// check for timeout - synch failed
	//if(mVassalMap.role != RoleFeudalisticPlatoon::Outlaw){
		// check sync status
	/*bool synched = true;
	for(unsigned int i = 0; i < mVassalMap.sync.size(); i++){
		synched = synched & mVassalMap.sync.at(i);
	}*/
	/*if(mVassalMap.role == RoleFeudalisticPlatoon::Outlaw){
		//set time for timeout
		mSubPlatoonTimeOut.last = taiTimeNow;
	} else if(mVassalMap.role == RoleFeudalisticPlatoon::Claimant &&
			(taiTimeNow - mSubPlatoonTimeOut.last) > mSubPlatoonTimeOut.interval){
		// reset sync
		for(unsigned int i = 0; i < mVassalMap.sync.size(); i++){
			mVassalMap.sync.at(i) = false;
			mVassalMap.lteAddresses.at(i) = 0;
		}
		// reset ref
		mVassalMap.ref = -1;
		// set role to unemployed
		mVassalMap.role = RoleFeudalisticPlatoon::Outlaw;
		//set vassalageId to 0
		mVassalMap.vassalageId = 0;
	}*/
	//}

	/*if(mVassalMap.role != RoleFeudalisticPlatoon::Outlaw && mVassalMap.role != RoleFeudalisticPlatoon::Claimant){
		printSubPlatoonMapToFile();
	}*/

	// act according to ROLES
	switch (mVassalMap.role){
		case RoleFeudalisticPlatoon::Outlaw:
			mVehicleController->getLiteAPI().vehicle().setColor(
					mVehicleController->getVehicleId(), mColorset.at(RoleFeudalisticPlatoon::Outlaw));
			break;
		case RoleFeudalisticPlatoon::Claimant:{
			mVehicleController->getLiteAPI().vehicle().setColor(
								mVehicleController->getVehicleId(), mColorset.at(RoleFeudalisticPlatoon::Claimant));
			// determine feudalistic roles according to position
			unsigned int pos_imp = 0;
			unsigned int pos_exp = mVassalMap.depth - 1;
			unsigned int pos_merc = (unsigned int)(mVassalMap.depth / 2);

			if(mPlatMap.innerPosition % mVassalMap.depth == pos_merc){
				//mVassalMap.role = RoleFeudalisticPlatoon::Lord;
				if((mVassalMap.vassalageId == 0)){
					// init vassalage formation

					unsigned int m_pos = (unsigned int)(mVassalMap.depth / 2) + 1;

					// set feudalistic platoon members in local map
					int start_tmp = mPlatMap.innerPosition - m_pos;
					if(start_tmp < 0){
						start_tmp = 0;
					}
					unsigned int i_start = start_tmp;
					unsigned int i_end = i_start + mVassalMap.members.size();
					for(unsigned int i = i_start; i < i_end; i++){
						if(i > mPlatMap.members.size()-1){
							mVassalMap.members.at(i - i_start) = 0;
						}else{
							mVassalMap.members.at(i - i_start) = mPlatMap.members.at(i);
						}

						mVassalMap.lteAddresses.at(i - i_start) = 0; // addresses not know so far
					}

					start_tmp = mPlatMap.innerPosition - m_pos;
					if(start_tmp < 0){
						mVassalMap.members.pop_back();
						mVassalMap.lteAddresses.pop_back();
						mVassalMap.members.insert(mVassalMap.members.begin(), 0);
						mVassalMap.lteAddresses.insert(mVassalMap.lteAddresses.begin(), 0);
					}

					// calculate vassalageId
					uint32_t id = 0;
					for(unsigned int i = 0; i < mVassalMap.members.size(); i++){
						id = id + mVassalMap.members.at(i);
					}
					id = id % mPlatMap.platoonId;
					mVassalMap.platoonId = mPlatMap.platoonId;

					mVassalMap.ref = (uint16_t)rand();

					// prepare message
					FeudalMsg *feudalMsg = new FeudalMsg();
					feudalMsg->setMsgType(MessageTypeFeudalisticPlatoon::Init);
					feudalMsg->setPlatoonID(mVassalMap.platoonId);
					feudalMsg->setVassalageID(id);
					feudalMsg->setTxStationID(mVehicleDataProvider->station_id());
					FeudalSync *syncMsg = new FeudalSync();
					syncMsg->setRef(mVassalMap.ref);
					// init lte address vector
					syncMsg->setLteAddressesArraySize(mVassalMap.lteAddresses.size());
					for(unsigned int i = 0; i < mVassalMap.lteAddresses.size(); i++){
						syncMsg->setLteAddresses(i, 0);
					}
					// set own address
					syncMsg->setLteAddresses(m_pos, mL3Address.toIPv4().getInt());
					// do the same in local subPlatoonMap
					mVassalMap.lteAddresses.at(m_pos) = mL3Address.toIPv4().getInt();

					syncMsg->setMembersArraySize(mVassalMap.members.size());
					for(unsigned int i = 0; i < mVassalMap.members.size(); i++){
						syncMsg->setMembers(i, mVassalMap.members.at(i));
					}

					// append message to feudalMsg
					feudalMsg->setSyncMsgArraySize(1);
					feudalMsg->setSyncMsg(0, *syncMsg);

					// set time
					feudalMsg->setTimestamp(simTime());
					mVassalMap.timeStamp = feudalMsg->getTimestamp();

					// create request
					btp::DataRequestB req;
					req.destination_port = host_cast<FeudalisticPlatooningService::port_type>(getPortNumber());
					req.gn.transport_type = geonet::TransportType::SHB;
					req.gn.maximum_hop_limit = 0;
					req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
					req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

					emit(scSignalTXInitWLAN, feudalMsg->dup());
					request(req, feudalMsg);
				}
			}
			break;
		}

		case RoleFeudalisticPlatoon::Merchant:{
			mVehicleController->getLiteAPI().vehicle().setColor(
								mVehicleController->getVehicleId(), mColorset.at(RoleFeudalisticPlatoon::Merchant));
			if((taiTimeNow - mVassalageTimeMerchant.last) > mVassalageTimeMerchant.interval){

				VassalMsg bridgeMsg = packVassalagesCamsAndDenms();
				FeudalMsg* msg = createFeudalMsg(nullptr, &bridgeMsg,
						mVehicleDataProvider->station_id(), mPlatMap.platoonId, mVassalMap.vassalageId);

				emit(scSignalBridgeExport, msg->dup());

				if(!mLteAvailable){
					// create request
					btp::DataRequestB req;
					req.destination_port = host_cast<FeudalisticPlatooningService::port_type>(getPortNumber());
					req.gn.transport_type = geonet::TransportType::SHB;
					req.gn.maximum_hop_limit = 0;
					req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
					req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
					request(req, msg);

					std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
							<< ": sent BRIDGE message using WLAN!" << std::endl;
				}

				mVassalageTimeMerchant.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()));
			}

			break;
		}

		case RoleFeudalisticPlatoon::Knight:{
			mVehicleController->getLiteAPI().vehicle().setColor(
								mVehicleController->getVehicleId(), mColorset.at(RoleFeudalisticPlatoon::Knight));
			// this role becomes relevant within the indicate function
			break;
		}

		case RoleFeudalisticPlatoon::Lord:{
			mVehicleController->getLiteAPI().vehicle().setColor(
								mVehicleController->getVehicleId(), mColorset.at(RoleFeudalisticPlatoon::Lord));


			// Lord either ships data to the central (server), where it is stored and ready for others to order,
			// or orders that data indicating its own platoonId and vassalageId.
			// Both operations are only performed once LTE is available.

			// Shipping
			if((taiTimeNow - mVassalageTimeLordShipping.last) > mVassalageTimeLordShipping.interval){
				VassalMsg bridgeMsg = packVassalagesCamsAndDenms();
				FeudalMsg* msg = createFeudalMsg(nullptr, &bridgeMsg, mVehicleDataProvider->station_id(),
						mPlatMap.platoonId, mVassalMap.vassalageId);
				emit(scSignalLordShipping, msg->dup());

				// reset timer
				mVassalageTimeLordShipping.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()));
			}

			// Ordering
			if((taiTimeNow - mVassalageTimeLordOrdering.last) > mVassalageTimeLordOrdering.interval){
				// Use bridgeMsg: Important to contain platoonId and vassalageId
				VassalMsg *bridgeMsg = new VassalMsg();
				bridgeMsg->setVassalageID(mVassalMap.vassalageId);
				bridgeMsg->setBitLength(sizeof(uint32_t) * 8);
				FeudalMsg* msg = createFeudalMsg(nullptr, bridgeMsg, mVehicleDataProvider->station_id(), 
						mPlatMap.platoonId, mVassalMap.vassalageId);
				msg->setMsgType(MessageTypeFeudalisticPlatoon::Ordering);
				emit(scSignalLordOrdering, msg->dup());
				// reset timer
				mVassalageTimeLordOrdering.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()));
			}

			break;
		}

		case RoleFeudalisticPlatoon::Peasant:
			mVehicleController->getLiteAPI().vehicle().setColor(
								mVehicleController->getVehicleId(), mColorset.at(RoleFeudalisticPlatoon::Peasant));
			break;
		default:
			break;
	}
}

void FeudalisticPlatooningService::indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket* packet){
	auto msg = check_and_cast<const FeudalMsg*>(packet);

	auto platService = (mPlatooningService > mPlatooningServiceEnvmod) ? mPlatooningService : mPlatooningServiceEnvmod;

	if(msg->getPlatoonID() != mPlatMap.platoonId){
		delete msg;
		return;
	}

	// act according to message type
	switch (msg->getMsgType()){
		case MessageTypeFeudalisticPlatoon::Empty:
			// nothing to do
			break;
		case MessageTypeFeudalisticPlatoon::Init:{
			bool message_of_relevance = true;
			if(mPlatMap.pendingOperation.first){
				message_of_relevance = false;
			}

			std::vector<uint32_t> memb;

			if(message_of_relevance){
				// read out members vector
				message_of_relevance = false;
				for (int i = 0; i < msg->getSyncMsg(0).getMembersArraySize(); i++){
					memb.push_back(msg->getSyncMsg(0).getMembers(i));
					if(memb.at(i) == mVehicleDataProvider->station_id()){
						message_of_relevance = true;
					}
				}
			}

			if(message_of_relevance){
				emit(scSignalRXInitWLAN, msg->dup());

				// read out addresses
				std::vector<uint32_t> addrs;
				for (int i = 0; i < msg->getSyncMsg(0).getLteAddressesArraySize(); i++){
					addrs.push_back(msg->getSyncMsg(0).getLteAddresses(i));
				}

				// memory check
				assert(memb.size() == addrs.size());

				bool active_member = true;
				bool passive_member_new_data = false;
				// in case no active member of vassalage: only add lte address to list and broadcast info.
				if(mVehicleDataProvider->station_id() == memb.at(0)){
					active_member = false;
					if(addrs.at(0) == 0){
						passive_member_new_data = true;
					}
					// vehicle is merchant of previous vassalage
					addrs.at(0) = mL3Address.toIPv4().getInt();
				} else if(mVehicleDataProvider->station_id() == memb.at(memb.size()-1)){
					active_member = false;
					if(addrs.at(memb.size()-1) == 0){
						passive_member_new_data = true;
					}
					// vehicle is knight of next vassalage
					addrs.at(memb.size()-1) = mL3Address.toIPv4().getInt();
				}


				if(!active_member && passive_member_new_data){
					FeudalSync* syncMsg = createSyncMsg(msg->getSyncMsg(0).getRef(), memb, addrs);
					FeudalMsg* feudalMsg = createFeudalMsg(syncMsg, nullptr, mVehicleDataProvider->station_id(), 
							msg->getPlatoonID(), msg->getVassalageID());

					// create request
					btp::DataRequestB req;
					req.destination_port = host_cast<FeudalisticPlatooningService::port_type>(getPortNumber());
					req.gn.transport_type = geonet::TransportType::SHB;
					req.gn.maximum_hop_limit = 0;
					req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
					req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

					emit(scSignalTXInitWLAN, feudalMsg->dup());
					request(req, feudalMsg);
					std::cout << mVehicleDataProvider->station_id() << "(" << mVehicleController->getVehicleId() << ") "
							<< "sending passive member ::Init message (" << msg->getSyncMsg(0).getRef()
							<< ")!" << std::endl;


				} else if(active_member &&
						(mVassalMap.ref < 0 || mVassalMap.ref == msg->getSyncMsg(0).getRef()) &&
						mVassalMap.role >= RoleFeudalisticPlatoon::Claimant){
					// check if no feudalistic request is ongoing (then ref is -1) or if it is the same request.

					mVassalMap.ref = msg->getSyncMsg(0).getRef();

					// memory check
					assert(mVassalMap.members.size() == mVassalMap.lteAddresses.size());
					assert(mVassalMap.members.size() == memb.size());

					bool my_new_data = false;
					// update local map
					mVassalMap.timeStamp = simTime();
					for(unsigned int i = 0; i < mVassalMap.members.size(); i++){
						if(mVassalMap.members.at(i) == 0){
							mVassalMap.members.at(i) = memb.at(i);
						}
					}
					// update local addresses
					for(unsigned int i = 0; i < mVassalMap.lteAddresses.size(); i++){
						if(addrs.at(i) > 0){
							mVassalMap.lteAddresses.at(i) = addrs.at(i);
						} else if(mVassalMap.members.at(i) == mVehicleDataProvider->station_id()){
							mVassalMap.lteAddresses.at(i) = mL3Address.toIPv4().getInt();
							if(addrs.at(i) == 0){
								my_new_data = true;
							}
						}
					}

					if(mVassalMap.role == RoleFeudalisticPlatoon::Claimant){
						//determine roles according to position
						unsigned int pos_imp = 0;
						unsigned int pos_exp = mVassalMap.depth - 1;
						unsigned int pos_merc = (unsigned int)(mVassalMap.depth / 2);

						if(mPlatMap.innerPosition % mVassalMap.depth == pos_merc){
							mVassalMap.role = RoleFeudalisticPlatoon::Lord;
							mVassalMap.innerPosition = pos_merc + 1;
						} else if(mPlatMap.innerPosition % mVassalMap.depth == pos_imp){
							mVassalMap.role = RoleFeudalisticPlatoon::Knight;
							mVassalMap.innerPosition = pos_imp + 1;
						} else if(mPlatMap.innerPosition % mVassalMap.depth == pos_exp){
							mVassalMap.role = RoleFeudalisticPlatoon::Merchant;
							mVassalMap.innerPosition = pos_exp + 1;
						} else{
							mVassalMap.role = RoleFeudalisticPlatoon::Peasant;
						}


						if(mPlatMap.depth % 2){
							//for uneven platoon lengths, the last vassalage is bigger. Adapt roles accordingly
							if(mPlatMap.innerPosition == mPlatMap.depth - 1){
								mVassalMap.role = RoleFeudalisticPlatoon::Merchant;
							} else if(mPlatMap.innerPosition == mPlatMap.depth -2){
								mVassalMap.role = RoleFeudalisticPlatoon::Peasant;
							}
						}
					}

					if(my_new_data){
						FeudalSync* syncMsg = createSyncMsg(msg->getSyncMsg(0).getRef(), mVassalMap.members,
								mVassalMap.lteAddresses);
						FeudalMsg* feudalMsg = createFeudalMsg(syncMsg, nullptr,
															mVehicleDataProvider->station_id(), msg->getPlatoonID(),
															msg->getVassalageID());

						// create request
						btp::DataRequestB req;
						req.destination_port = host_cast<FeudalisticPlatooningService::port_type>(getPortNumber());
						req.gn.transport_type = geonet::TransportType::SHB;
						req.gn.maximum_hop_limit = 0;
						req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
						req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;

						emit(scSignalTXInitWLAN, feudalMsg->dup());
						request(req, feudalMsg);

						std::cout << mVehicleDataProvider->station_id() << "(" << mVehicleController->getVehicleId() << ") "
								<< "sending active member ::Init message(" << mVassalMap.ref
								<< ")!" << std::endl;
					}
					std::cout << mVehicleController->getVehicleId() << std::endl;
					for(unsigned int i = 0; i < mVassalMap.members.size(); i++){
						std::cout << i << " " << mVassalMap.members.at(i) << " " << mVassalMap.lteAddresses.at(i) << std::endl;
					}
					std::cout << std::endl;

					// check if everyone is ready
					bool synched = true;
					for(unsigned int i = 0; i < mVassalMap.lteAddresses.size(); i++){
						if(mVassalMap.members.at(i) != 0 && mVassalMap.lteAddresses.at(i) == 0){
							synched = false;
							std::cout << "Not ready!" << std::endl;
							break;
						}
					}

					if((synched && mVassalMap.vassalageId == 0)){
						// get part of the vassalage
						assert(msg->getVassalageID() > 0);
						mVassalMap.vassalageId = msg->getVassalageID();

						uint32_t m_pos = 0;
						for(unsigned int i = 0; i < mVassalMap.members.size(); i++){
							if(mVehicleDataProvider->station_id() == mVassalMap.members.at(i)){
								m_pos = i;
								break;
							}
						}

						switch (mVassalMap.role) {
							case RoleFeudalisticPlatoon::Lord:
								// randomize odering/shipping time over vassalages
								mVassalageTimeLordOrdering.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()))
										- (rand() % mVassalageTimeLordOrdering.interval);
								mVassalageTimeLordShipping.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()))
										- (rand() % mVassalageTimeLordShipping.interval);
								mVassalMap.ivDistanceInner = 8.0;
								platService->setNewIvDistance(mVassalMap.ivDistanceInner);
								break;

							case RoleFeudalisticPlatoon::Knight:
								// select iv distance according to personal needs
								mVassalMap.ivDistanceInner = par("feudalisticIvDistanceBetween");
								platService->setNewIvDistance(mVassalMap.ivDistanceInner);
								break;

							case RoleFeudalisticPlatoon::Merchant:
								// randomize distributing time over vassalages
								mVassalageTimeMerchant.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()))
										- (rand() % mVassalageTimeMerchant.interval);
								mVassalMap.ivDistanceInner = par("feudalisticIvDistanceInner");
								platService->setNewIvDistance(mVassalMap.ivDistanceInner);
								break;

							case RoleFeudalisticPlatoon::Peasant:
								mVassalMap.ivDistanceInner = par("feudalisticIvDistanceInner");
								platService->setNewIvDistance(mVassalMap.ivDistanceInner);
								break;

							default:
								break;
						}

						// all vehicles log their role
						emit(scSignalSubPlatoonRole, mVassalMap.role);
						// all vehicles log their vassalageId
						emit(scSignalSubPlatoonId, mVassalMap.vassalageId);

						//printVassalMapToFile();

					} else if(!synched){
						if(!mSyncTiming.flag){
							FeudalSync* syncMsg = createSyncMsg(msg->getSyncMsg(0).getRef(), mVassalMap.members,
									mVassalMap.lteAddresses);
							syncReBroadcastMsg = createFeudalMsg(syncMsg, nullptr,
																mVehicleDataProvider->station_id(), msg->getPlatoonID(),
																msg->getVassalageID());
							mSyncTiming.flag = true;
							mSyncTiming.timing.last = countTaiMilliseconds(mTimer->getTimeFor(simTime()))
									- (rand() % mSyncTiming.timing.interval);
						}

					}
				}
			}
			break;
		}

		case MessageTypeFeudalisticPlatoon::Bridge:{
			if(mVassalMap.role == RoleFeudalisticPlatoon::Knight){
				// only import CAMs and DENMs once role is knight
				if(msg->getVassalMsgArraySize()){

					// get bridge Msg. There should be only one.
					VassalMsg bridgeMsg = msg->getVassalMsg(0);

					// only consider message once vassalageId is different.
					// only consider message once its exported from predecessor.
					if(bridgeMsg.getVassalageID() != mVassalMap.vassalageId &&
							mPlatMap.innerPosition &&
							msg->getTxStationID() == mPlatMap.members.at(mPlatMap.innerPosition -1)){

						// update map and memory.
						updateLocalMapAndMemory(bridgeMsg.dup());

						std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
								<< ": distribute BRIDGE message!" << std::endl;
						// Distribute: forward message to vassalage members
						// copy bridge message and modify tx station
						FeudalMsg* distroMsg = createFeudalMsg(nullptr, bridgeMsg.dup(), 
								mVehicleDataProvider->station_id(), mPlatMap.platoonId, mVassalMap.vassalageId);
						// modify message type and vassalageId within bridge message
						distroMsg->setMsgType(MessageTypeFeudalisticPlatoon::Distribution);
						distroMsg->getVassalMsg(0).setVassalageID(mVassalMap.vassalageId);

						emit(scSignalBridgeImport, distroMsg->dup());

						if(!mLteAvailable){
							btp::DataRequestB req;
							req.destination_port = host_cast<FeudalisticPlatooningService::port_type>(getPortNumber());
							req.gn.transport_type = geonet::TransportType::SHB;
							req.gn.maximum_hop_limit = 0;
							req.gn.traffic_class.tc_id(static_cast<unsigned>(dcc::Profile::DP1));
							req.gn.communication_profile = geonet::CommunicationProfile::ITS_G5;
							request(req, distroMsg);
						}
					}
				}
			}
			break;
		}

		case MessageTypeFeudalisticPlatoon::Distribution:{
			// independent from role, adapt local map and denm memory,
			// once message is from the same vassalage (knight distributed message)
			if(msg->getVassalMsgArraySize() && mVassalMap.vassalageId == msg->getVassalMsg(0).getVassalageID()){
				VassalMsg bridgeMsg = msg->getVassalMsg(0);
				updateLocalMapAndMemory(bridgeMsg.dup());
				std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
						<< ": updating using distributed BRIDGE (" << bridgeMsg.getVassalageID() << ")!" << std::endl;
			}
			break;
		}

		case MessageTypeFeudalisticPlatoon::Ordering:{
			if(mVassalMap.role == RoleFeudalisticPlatoon::Lord){
				// response to submitted order.
				// use it to update own local map and distribute it among other vassalage members.
				if(msg->getVassalMsgArraySize()){
					// multiple bridge messages may be present (one for each vassalage).
					// message of own vassalage does not contain any CAMs and DENMs
					std::vector<VassalMsg*> bridges;
					for(int i = 0; i < msg->getVassalMsgArraySize(); i++){
						VassalMsg bridgeMsg = msg->getVassalMsg(i);
						// exclude empty message from own vassalage
						if(bridgeMsg.getVassalageID() != mVassalMap.vassalageId){
							// update map and memory
							updateLocalMapAndMemory(bridgeMsg.dup());
							std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
									<< ": [LORD] distributing order (" << bridgeMsg.getVassalageID() << ")!" << std::endl;
							// copy message to vector. It will be send to members of vassalage
							bridges.push_back(bridgeMsg.dup());
						}
					}

					std::ofstream myfile;
                    myfile.open ("/home/tobiasrenzler/Desktop/ordering_response.txt", std::ios_base::app);
                    myfile << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                    myfile << "RESPONSE Indicate [" << mL3Address.toIPv4().getInt() << "]"<< std::endl;
                    myfile << simTime() << std::endl;
                    myfile << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
                    myfile.close();

					// Distribute message among vassalage
					// copy bridge message from other vassalages and modify tx station
					if(bridges.size()){
						FeudalMsg* distroMsg = createFeudalMsgMultipleBridge(bridges,
								mVehicleDataProvider->station_id(),mPlatMap.platoonId,
								mVassalMap.vassalageId);
						// modify message type within bridge message
						distroMsg->setMsgType(MessageTypeFeudalisticPlatoon::Ordering);
						//only send once LTE is available
						emit(scSignalLordTrading, distroMsg->dup());
						std::cout << "Distributing: "
								<< distroMsg->getByteLength() << std::endl;
					}
				}
			} else{
				// all non merchants of the vassalage use the distributed message to update their local map
				if(msg->getVassalMsgArraySize()){
					for(int i = 0; i < msg->getVassalMsgArraySize(); i++){
						VassalMsg bridgeMsg = msg->getVassalMsg(i);
						updateLocalMapAndMemory(bridgeMsg.dup());
						std::cout << mVehicleDataProvider->station_id()
								<< " (" << mVehicleController->getVehicleId() << ") "
								<< ": updating using distributed ORDER (" << bridgeMsg.getVassalageID() << ")!"
								<< std::endl;
					}
				}
			}
			break;
		}

		default:
			break;
	}
	delete msg;
}

void FeudalisticPlatooningService::updateLocalMapAndMemory(VassalMsg *bridgeMsg){
	// get CAMs
	unsigned int number_of_CAMs = bridgeMsg->getCamsArraySize();
	if(number_of_CAMs){
		for(unsigned int i = 0; i < number_of_CAMs; i++){
			// reconstruct cam from byte buffer
			vanetza::asn1::Cam cam;
			ByteBuffer buffer;
			for(unsigned int j = 0; j < bridgeMsg->getCams(i).getDataArraySize(); j++){
				buffer.push_back(bridgeMsg->getCams(i).getData(j));
			}
			bool decodingCAM = cam.decode(buffer);
			if(decodingCAM){
				// update local map using the extracted cam
				//std::cout << "Distributed/Ordered CAM: " << std::endl;
				//printCAM(cam);
				mLocalDynamicMap->updateAwareness(cam);
			} else{
				//assert(decodingCAM == true);
				std::cout << "[ATTENTION] Corrupt CAM! Dropping..." << std::endl;
			}

		}
		std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
				<< ": updated local map with " << number_of_CAMs << " CAMs" << std::endl;
	}

	// get DENMs
	unsigned int number_of_DENMs = bridgeMsg->getDenmsArraySize();
	if(number_of_DENMs){
		for(unsigned int i = 0; i < number_of_DENMs; i++){
			vanetza::asn1::Denm denm;
			ByteBuffer buffer;
			for(unsigned int j = 0; j < bridgeMsg->getDenms(i).getDataArraySize(); j++){
				buffer.push_back(bridgeMsg->getDenms(i).getData(j));
			}
			bool decodingDENM = denm.decode(buffer);
			assert(decodingDENM == true);
			// get Service
			if(!mDenService){
				if(getParentModule()->getSubmodule("DenService", -1)){
					mDenService = check_and_cast<DenService*>(getParentModule()->getSubmodule("DenService", -1));
				}
			}
			assert(mDenService != nullptr);

			// store object in memory (only once not already present)
			DenmObject denmObj(denm);
			mDenService->receiveObjectForMemory(denmObj);
		}
		std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
				<< ": updated local map with " << number_of_DENMs << " DENMs" << std::endl;
	}
}

void FeudalisticPlatooningService::setupFeudalisticPlatooning(){
	mVassalMap.members.resize(mVassalMap.depth + 2, 0);
	mVassalMap.lteAddresses.resize(mVassalMap.members.size(), 0);
	mVassalMap.timeStamp = simTime();
	auto platService = (mPlatooningService > mPlatooningServiceEnvmod) ? mPlatooningService : mPlatooningServiceEnvmod;
	//check if PlatooningService operation is currently pending (e.g., join/leave). In that case, do not proceed further.
	mPlatMap = platService->getPlatoonMap();
    mVassalMap.platoonId = mPlatMap.platoonId;
	if(mPlatMap.pendingOperation.first){
		return;
	}

	mVassalMap.role = RoleFeudalisticPlatoon::Claimant;
}

void FeudalisticPlatooningService::printVassalMap(){
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "[" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId() << "]"
			<< "\t LTE: " << mL3Address.toIPv4().getInt() << "\t PlatoonID: "
			<< mVassalMap.platoonId << "\t SubPlatoonID: " << mVassalMap.vassalageId
			<< "\t Role: " << (int)mVassalMap.role << "\t Depth: "
			<< mVassalMap.depth << "\t Timestamp: " << mVassalMap.timeStamp
			<< "\t ivDistanceInner: " << mVassalMap.ivDistanceInner << std::endl;
	unsigned int end_idx = mVassalMap.depth + 2;
	for(unsigned int i = 0; i < end_idx; i++){
		std::cout << "\t" << i << ": " << mVassalMap.members.at(i)  << "\t IPv4: "
				<< mVassalMap.lteAddresses.at(i);
		if(i == 0){ // merchant from previous vassalage
			std::cout << "(exp)";
		}else if(i == end_idx - 1){ // knight from preceeding vassalage
			std::cout << "(imp)";
		}
		std::cout << std::endl;
	}
	std::cout << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
}

void FeudalisticPlatooningService::printVassalMapToFile(){
	std::ofstream myfile;
	// define filename and location
	myfile.open ("your_path/test.txt", std::ios_base::app);

	myfile << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	myfile << "[" << mVehicleDataProvider->station_id() << ", " << mVehicleController->getVehicleId() << "]"
			<< "\t LTE: " << mL3Address.toIPv4().getInt() << "\t PlatoonID: "
			<< mVassalMap.platoonId << "\t SubPlatoonID: " << mVassalMap.vassalageId
			<< "\t Role: " << (int)mVassalMap.role << "\t Depth: "
			<< mVassalMap.depth << "\t Timestamp: " << mVassalMap.timeStamp
			<< "\t ivDistanceInner: " << mVassalMap.ivDistanceInner << std::endl;
	unsigned int end_idx = mVassalMap.depth + 2;
	for(unsigned int i = 0; i < end_idx; i++){
		myfile << "\t" << i << ": " << mVassalMap.members.at(i)  << "\t IPv4: "
				<< mVassalMap.lteAddresses.at(i);
		if(i == 0){ // merchant from previous vassalage
			myfile << "(exp)";
		}else if(i == end_idx - 1){ // knight from preceeding vassalage
			myfile << "(imp)";
		}
		myfile << std::endl;
	}
	myfile << "+++++++++++++++++++++++++++++++++++++++++++++++++" << std::endl;
	myfile.close();
}

/*
 * Send all CAMs that are FROM the vassalage.
 * Send ALL DENMs.
 * */
VassalMsg& FeudalisticPlatooningService::packVassalagesCamsAndDenms(){
    VassalMsg *bridgeMsg = new VassalMsg();
    bridgeMsg->setVassalageID(mVassalMap.vassalageId);
    // GET CAMs - exclude merchant/knight from neighboring vassalages
	bridgeMsg->setCamsArraySize(mVassalMap.members.size() - 2);
	for(unsigned int i = 1; i < mVassalMap.members.size() - 1; i++){
		// read from store
		std::pair<bool, vanetza::asn1::Cam> temp =
				mLocalDynamicMap->getCamByStationId(mVassalMap.members.at(i));
		// assuming at least one CAM from each vassalage member was received in the past
		SimpleArrayMsg* simpleMsg = new SimpleArrayMsg();
		if(temp.first == true){ // use present cam
			vanetza::asn1::Cam cam = temp.second;
			simpleMsg->setDataArraySize(cam.encode().size());
			for(unsigned int j = 0; j < cam.encode().size(); j ++){
				simpleMsg->setData(j, cam.encode().at(j));
			}
			simpleMsg->setBitLength(simpleMsg->getDataArraySize() * 8);
		} else{ // no cam available. Send empty package
			simpleMsg->setDataArraySize(0);
			simpleMsg->setBitLength(0);
		}
		bridgeMsg->setCams(i - 1, *simpleMsg);

	}

	// GET DENMs
	if(!mDenService){
		if(getParentModule()->getSubmodule("DenService", -1)){
			mDenService = check_and_cast<DenService*>(getParentModule()->getSubmodule("DenService", -1));
		}
	}
	assert(mDenService != nullptr);
	std::shared_ptr<const artery::den::Memory> denMemory = mDenService->getMemory();
	auto denmmessage_range = denMemory->messages();
	if(denmmessage_range.empty()){
		bridgeMsg->setDenmsArraySize(0);
	} else{
		unsigned int i = 0;
		do{
			// reserve space
			bridgeMsg->setDenmsArraySize(i+1);
			den::Reception reception = denmmessage_range.front();
			const vanetza::asn1::Denm* denm = reception.message.get();

			SimpleArrayMsg* simpleMsg = new SimpleArrayMsg();
			simpleMsg->setDataArraySize(denm->encode().size());
			for(unsigned int j = 0; j < denm->encode().size(); j++){
				simpleMsg->setData(j, denm->encode().at(j));
			}
			simpleMsg->setBitLength(simpleMsg->getDataArraySize() * 8);
			bridgeMsg->setDenms(i, *simpleMsg);

			i = i + 1;
			denmmessage_range.drop_front(); // deletes only from range, not from den memory.

		} while(!denmmessage_range.empty());
	}

	uint32_t tmp_byte_size = 0;
	for(unsigned int i = 0; i < bridgeMsg->getCamsArraySize(); i++){
		tmp_byte_size = tmp_byte_size + bridgeMsg->getCams(i).getByteLength();
	}
	for(unsigned int i = 0; i < bridgeMsg->getDenmsArraySize(); i++){
		tmp_byte_size = tmp_byte_size + bridgeMsg->getDenms(i).getByteLength();
	}
	bridgeMsg->setBitLength((sizeof(uint32_t) + tmp_byte_size)*8);
	return *bridgeMsg;
}

} //namespace platooning
} //namespace artery
