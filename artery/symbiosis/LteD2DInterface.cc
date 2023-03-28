/*
 * LteD2DInterface.cc
 *
 *      Author: rtobi
 */
#include <artery/symbiosis/LteD2DInterface.h>
#include "lte_msgs/BlackIceWarning_m.h"
#include "artery/application/Middleware.h"
#include "artery/application/StoryboardSignal.h"
#include "artery/traci/VehicleController.h"
#include "artery/utility/PointerCheck.h"
#include <inet/common/ModuleAccess.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/networklayer/configurator/ipv4/IPv4NetworkConfigurator.h>
#include <omnetpp/checkandcast.h>
#include "artery/application/Asn1PacketVisitor.h"
#include "artery/application/CaObject.h"
#include "artery/application/DenmObject.h"
#include <omnetpp/clistener.h>
#include "artery/application/CaPlatooningService.h"
#include <fstream>

using namespace omnetpp;

namespace artery{

Define_Module(LteD2DInterface)

static const simsignal_t storyboardSignal = cComponent::registerSignal("StoryboardSignal");
static const simsignal_t scSignalTXCamLTE = cComponent::registerSignal("TXCamLTE");
static const simsignal_t scSignalTXCamLTEtarget = cComponent::registerSignal("TXCamLTEtarget");
static const simsignal_t scSignalTXDenmLTE = cComponent::registerSignal("TXDenmLTE");
static const simsignal_t scSignalTXDenmLTEtarget = cComponent::registerSignal("TXDenmLTEtarget");
static const simsignal_t scSignalTXBridgeExportLTE = cComponent::registerSignal("TXBridgeExportLTE");
static const simsignal_t scSignalTXBridgeExportLTEtarget = cComponent::registerSignal("TXBridgeExportLTEtarget");
static const simsignal_t scSignalTXBridgeImportLTE = cComponent::registerSignal("TXBridgeImportLTE");
static const simsignal_t scSignalTXBridgeImportLTEtarget = cComponent::registerSignal("TXBridgeImportLTEtarget");
static const simsignal_t scSignalTXLordTradeLTE = cComponent::registerSignal("TXLordTradeLTE");
static const simsignal_t scSignalTXLordTradeLTEtarget = cComponent::registerSignal("TXLordTradeLTEtarget");
static const simsignal_t scSignalTXShippingLTE = cComponent::registerSignal("TXShippingLTE");
static const simsignal_t scSignalTXShippingLTEtarget = cComponent::registerSignal("TXShippingLTEtarget");
static const simsignal_t scSignalTXOrderingLTE = cComponent::registerSignal("TXOrderingLTE");
static const simsignal_t scSignalTXOrderingLTEtarget = cComponent::registerSignal("TXOrderingLTEtarget");
static const simsignal_t scSignalRXCamLTE = cComponent::registerSignal("RXCamLTE");
static const simsignal_t scSignalRXDenmLTE = cComponent::registerSignal("RXDenmLTE");
static const simsignal_t scSignalRXBridgeLTE = cComponent::registerSignal("RXBridgeLTE");
static const simsignal_t scSignalRXDistributionLTE = cComponent::registerSignal("RXDistributionLTE");
static const simsignal_t scSignalRXOrderingLTE = cComponent::registerSignal("RXOrderingLTE");
static const simsignal_t scSignalTX = cComponent::registerSignal("tx");
static const simsignal_t scSignalRX = cComponent::registerSignal("rx");
static const simsignal_t scSignalLteAddrMulticast = cComponent::registerSignal("LteAddressMulticast");


LteD2DInterface::~LteD2DInterface()
{

}

int LteD2DInterface::numInitStages() const
{
    return inet::NUM_INIT_STAGES;
}

void LteD2DInterface::initialize(int stage)
{
    cSimpleModule::initialize(stage);
    if (stage != inet::INITSTAGE_APPLICATION_LAYER) {
        return;
    }

    mcastAddress = inet::L3AddressResolver().resolve(par("mcastAddress"));
    mcastPort = par("mcastPort");
    socket.setOutputGate(gate("udpOut"));
    socket.bind(mcastPort);

    // LTE multicast support
    inet::IInterfaceTable *ift = inet::getModuleFromPar<inet::IInterfaceTable>(par("interfaceTableModule"), this);
    inet::InterfaceEntry *ie = ift->getInterfaceByName("wlan");
    if (!ie)
        throw cRuntimeError("Wrong multicastInterface setting: no interface named wlan");
    socket.setMulticastOutputInterface(ie->getInterfaceId());
    socket.joinMulticastGroup(mcastAddress);

    emit(scSignalLteAddrMulticast, mcastAddress.toIPv4().getInt());

    // multicast without preconfigured params
    broadcastSinglePort = par("broadcastSinglePort");
    broadcastSingleSocket.setOutputGate(gate("udpOut"));
    broadcastSingleSocket.bind(broadcastSinglePort);

    // LTE D2D bridge
    bridgePort = par("bridgePort");
    bridgeSocket.setOutputGate(gate("udpOut"));
    bridgeSocket.bind(bridgePort);

    // LTE lord report/query
    lordPortShipping = par("lordPortShipping");
    lordPortOrdering = par("lordPortOrdering");
    lordSocketShipping.setOutputGate(gate("udpOut"));
    lordSocketShipping.bind(lordPortShipping);
    lordSocketOrdering.setOutputGate(gate("udpOut"));
    lordSocketOrdering.bind(lordPortOrdering);

    // application's supporting code
    auto mw = inet::getModuleFromPar<artery::Middleware>(par("middlewareModule"), this);
    mw->subscribe(storyboardSignal, this);
    mVehicleController = artery::notNullPtr(mw->getFacilities().get_mutable_ptr<traci::VehicleController>());
    mVehicleDataProvider = &mw->getFacilities().get_const<VehicleDataProvider>();

    // subscribe to all external signals
    mw->subscribe("CamSent", this);
    mw->subscribe("DenmSent", this);
    mw->subscribe("BridgeExport", this);
    mw->subscribe("BridgeImport", this);
    mw->subscribe("LordShipping", this);
    mw->subscribe("LordOrdering", this);
    mw->subscribe("LordTrading", this);
    std::vector<simsignal_t> signals = mw->getLocalListenedSignals();
    scSignalCamSent80211p = 0;
    scSignalDenmSent80211p = 0;
    scSignalBridgeExport = 0;
    scSignalBridgeImport = 0;
    scSignalLordShipping = 0;
    scSignalLordOrdering = 0;
    scSignalLordTrading = 0;

    for(int i = 0; i < signals.size(); i++){
    	if(std::strcmp(mw->getSignalName(signals.at(i)), "CamSent") == 0){
    		scSignalCamSent80211p = signals.at(i);
    	} else if(std::strcmp(mw->getSignalName(signals.at(i)), "DenmSent") == 0){
			scSignalDenmSent80211p = signals.at(i);
		} else if(std::strcmp(mw->getSignalName(signals.at(i)), "BridgeExport") == 0){
			scSignalBridgeExport = signals.at(i);
		} else if(std::strcmp(mw->getSignalName(signals.at(i)), "BridgeImport") == 0){
			scSignalBridgeImport = signals.at(i);
		} else if(std::strcmp(mw->getSignalName(signals.at(i)), "LordShipping") == 0){
            scSignalLordShipping = signals.at(i);
        } else if(std::strcmp(mw->getSignalName(signals.at(i)), "LordOrdering") == 0){
            scSignalLordOrdering = signals.at(i);
        } else if(std::strcmp(mw->getSignalName(signals.at(i)), "LordTrading") == 0){
            scSignalLordTrading = signals.at(i);
        }

    }

    assert(scSignalCamSent80211p != 0);
    assert(scSignalDenmSent80211p != 0);
    assert(scSignalBridgeExport != 0);
    assert(scSignalBridgeImport != 0);
    assert(scSignalLordShipping != 0);
    assert(scSignalLordOrdering != 0);
    assert(scSignalLordTrading != 0);

    mLocalDynamicMap = artery::notNullPtr(mw->getFacilities().get_mutable_ptr<artery::LocalDynamicMap>());
    if(getParentModule()->getSubmodule("middleware", -1)->getSubmodule("DenService", -1)){
    	mDenService =
    			check_and_cast<artery::DenService*>(getParentModule()->
    					getSubmodule("middleware", -1)->getSubmodule("DenService", -1));
    }

    // get SubPlatooning Service
    if(getParentModule()->getSubmodule("middleware", -1)->getSubmodule("FeudalisticPlatooningService", -1)){
		mFeudalisticPlatooningService =
				check_and_cast<artery::platooning::FeudalisticPlatooningService*>(getParentModule()->
						getSubmodule("middleware", -1)->getSubmodule("FeudalisticPlatooningService", -1));
	}

    // get own address
	std::string nodepath = getParentModule()->getFullPath();
	mL3Address = inet::L3AddressResolver().resolve(nodepath.c_str());

    // application logic
}

void LteD2DInterface::finish()
{
    socket.close();
    bridgeSocket.close();
    lordSocketShipping.close();
    lordSocketOrdering.close();
}

void LteD2DInterface::receiveSignal(cComponent*, simsignal_t sig, cObject* obj, cObject*)
{
	if(sig == scSignalCamSent80211p){
		vanetza::asn1::Cam cam = check_and_cast<CaObject*>(obj)->asn1();
		sendCam(cam);
	} else if(sig == scSignalDenmSent80211p){
		vanetza::asn1::Denm denm = check_and_cast<DenmObject*>(obj)->asn1();
		sendDenm(denm);
	} else if(sig == scSignalBridgeExport){
		FeudalMsg *msg = check_and_cast<FeudalMsg*>(obj);
		if(mFeudalisticPlatooningService){
			// send message to following vehicle (knight of following vassalage)
		    VassalMap mySubPlatMap = mFeudalisticPlatooningService->getVassalMap();
		    int32_t destAddr = mySubPlatMap.lteAddresses.at(mySubPlatMap.members.size()-1);
		    sendFeudalMsg(*msg, destAddr);
		}
	} else if(sig == scSignalBridgeImport){
		FeudalMsg *msg = check_and_cast<FeudalMsg*>(obj);
		if(mFeudalisticPlatooningService){
			// distribute message for all vehicles of the same vassalage
			VassalMap mySubPlatMap = mFeudalisticPlatooningService->getVassalMap();
			for(unsigned int i = 1; i < mySubPlatMap.members.size() - 1; i++){
				int32_t destAddr = 0;
				// reminder: first entry is merchant address of previous vassalage
				assert(i < mySubPlatMap.lteAddresses.size());
				destAddr = mySubPlatMap.lteAddresses.at(i);
				sendFeudalMsg(*msg, destAddr);
			}
		}
	} else if(sig == scSignalLordShipping){
		FeudalMsg *msg = check_and_cast<FeudalMsg*>(obj);
		sendShipping(*msg);
		delete msg;
	} else if(sig == scSignalLordOrdering){
		FeudalMsg *msg = check_and_cast<FeudalMsg*>(obj);
		sendOrdering(*msg);
		delete msg;
	} else if(sig == scSignalLordTrading){
		FeudalMsg *msg = check_and_cast<FeudalMsg*>(obj);
		// distribute message between all members of the same vassalage
		VassalMap mySubPlatMap = mFeudalisticPlatooningService->getVassalMap();
		for(unsigned int i = 1; i < mySubPlatMap.members.size() - 1; i++){
			int32_t destAddr = 0;
			// reminder: first entry is merchant address of previous vassalage
			assert(i < mySubPlatMap.lteAddresses.size());
			destAddr = mySubPlatMap.lteAddresses.at(i);
			sendFeudalMsg(*msg, destAddr);
		}
		delete msg;
	}

    if (sig == storyboardSignal) {
        auto sigobj = check_and_cast<artery::StoryboardSignal*>(obj);
        /* You may add storyboard signals here */
    }
}

void LteD2DInterface::handleMessage(cMessage* msg)
{
    // for statistics: get address of transmitter
    auto ctrl = PK(msg)->getControlInfo();
    auto udp = dynamic_cast<inet::UDPDataIndication*>(ctrl);
	inet::L3Address tx_L3Address = udp->getSrcAddr();

    if (msg->isSelfMessage()) {
        mVehicleController->setSpeedFactor(1.0);
    } else if (msg->getKind() == inet::UDP_I_DATA) {
    	if(std::strcmp(msg->getClassName(), "WrappedCam") == 0){
    		WrappedCam *wCam = check_and_cast<WrappedCam*>(msg->dup());
    		artery::Asn1PacketVisitor<vanetza::asn1::Cam> visitor;
    		vanetza::ByteBuffer myBuffer;
    		for(int i = 0; i < wCam->getMsgArraySize(); i++){
    			myBuffer.push_back(wCam->getMsg(i));
    		}
    		visitor.deserialize(myBuffer);
    		artery::CaObject obj = visitor.shared_wrapper;

    		//statistics
    		emit(scSignalRXCamLTE, wCam->dup());
    		// statistics - transmitter/receiver
    		emit(scSignalRX, tx_L3Address.toIPv4().getInt());

    		mLocalDynamicMap->updateAwareness(obj);
    	} else if (std::strcmp(msg->getClassName(), "WrappedDenm") == 0){
    		if(mDenService){
				WrappedDenm *wDenm = check_and_cast<WrappedDenm*>(msg);
				artery::Asn1PacketVisitor<vanetza::asn1::Denm> visitor;
				vanetza::ByteBuffer myBuffer;
				for(int i = 0; i < wDenm->getMsgArraySize(); i++){
					myBuffer.push_back(wDenm->getMsg(i));
				}
				visitor.deserialize(myBuffer);
				artery::DenmObject obj = visitor.shared_wrapper;

				//statistics
				emit(scSignalRXDenmLTE, wDenm->dup());
	    		// statistics - transmitter/receiver
	    		emit(scSignalRX, tx_L3Address.toIPv4().getInt());

				// update DENM memory and trigger use cases.
				mDenService->receiveObjectForMemory(obj);
			}
    	} else if (std::strcmp(msg->getClassName(), "FeudalMsg") == 0){
    		// ::Bridge, ::Distribution, ::Ordering messages are handled here
			std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
					<< ": received FeudalMsg message over LTE interface!" << std::endl;
    		omnetpp::cPacket* cPack = check_and_cast<omnetpp::cPacket*>(msg);

    		auto rec_msg = check_and_cast<const FeudalMsg*>(msg);

    		if(mFeudalisticPlatooningService){
                mFeudalisticPlatooningService->indicateBridge(cPack->dup());
            }

    		// statistics: determine type and emit signals
    		switch (rec_msg->getMsgType()){
    			case MessageTypeFeudalisticPlatoon::Bridge:
    				emit(scSignalRXBridgeLTE, cPack->dup());
    	    		// statistics - transmitter/receiver
    	    		emit(scSignalRX, tx_L3Address.toIPv4().getInt());
    			case MessageTypeFeudalisticPlatoon::Distribution:
    				emit(scSignalRXDistributionLTE, cPack->dup());
    	    		// statistics - transmitter/receiver
    	    		emit(scSignalRX, tx_L3Address.toIPv4().getInt());
    			//case MessageTypeFeudalisticPlatoon::OrderingServerResponse:
    			case MessageTypeFeudalisticPlatoon::Ordering:
    				emit(scSignalRXOrderingLTE, cPack->dup());
    	    		// statistics - transmitter/receiver
    	    		emit(scSignalRX, tx_L3Address.toIPv4().getInt());
    		}

    	}
    } else {
        throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
    }
    delete msg;
}

void LteD2DInterface::sendReport()
{

}

void LteD2DInterface::sendCam(vanetza::asn1::Cam &cam){
	Enter_Method_Silent();

	WrappedCam *wCam = new WrappedCam();
	wCam->setMsgArraySize(cam.encode().size());
	for(int i = 0; i < cam.encode().size(); i ++){
		wCam->setMsg(i, cam.encode().at(i));
	}
	wCam->setBitLength(wCam->getMsgArraySize() * 8);

	sendAccordingToNic(wCam, nullptr);
}

void LteD2DInterface::sendDenm(vanetza::asn1::Denm &denm){
	Enter_Method_Silent();
	WrappedDenm *wDenm = new WrappedDenm();
	wDenm->setMsgArraySize(denm.encode().size());
	for(int i = 0; i < denm.encode().size(); i ++){
		wDenm->setMsg(i, denm.encode().at(i));
	}
	wDenm->setBitLength(wDenm->getMsgArraySize() * 8);

	sendAccordingToNic(nullptr, wDenm);
}

void LteD2DInterface::sendFeudalMsg(FeudalMsg &msg, uint32_t lteDestAddr){
	Enter_Method_Silent();
	// only send with valid address
	if(lteDestAddr){
		inet::L3Address destAddr = inet::L3Address(inet::IPv4Address(lteDestAddr));
		// only send once specified address is not own address
		if (destAddr != mL3Address){

		    // statistics
			if(msg.getMsgType() == MessageTypeFeudalisticPlatoon::Bridge){
				emit(scSignalTXBridgeExportLTE, msg.dup());
				emit(scSignalTXBridgeExportLTEtarget, destAddr.toIPv4().getInt());
			} else if(msg.getMsgType() == MessageTypeFeudalisticPlatoon::Distribution){
				emit(scSignalTXBridgeImportLTE, msg.dup());
				emit(scSignalTXBridgeImportLTEtarget, destAddr.toIPv4().getInt());
			} else if(msg.getMsgType() == MessageTypeFeudalisticPlatoon::Ordering){
				emit(scSignalTXLordTradeLTE, msg.dup());
				emit(scSignalTXLordTradeLTEtarget, destAddr.toIPv4().getInt());
				std::cout << "Distribute " <<  mVehicleController->getVehicleId() << std::endl;
			}
			// statistics - transmitter/receiver
			emit(scSignalTX, mL3Address.toIPv4().getInt());
			emit(scSignalTX, destAddr.toIPv4().getInt());

			bridgeSocket.sendTo(msg.dup(), destAddr, bridgePort);
			std::cout << mVehicleDataProvider->station_id() << " (" << mVehicleController->getVehicleId() << ") "
					<< ": sent FeudalMsg message to " << lteDestAddr << " over LTE interface!" << std::endl;
		}
	}
}

void LteD2DInterface::sendShipping(FeudalMsg &msg){
	Enter_Method_Silent();
	inet::L3Address centralAddress = inet::L3AddressResolver().resolve(par("centralAddress"));
	// statistics
	emit(scSignalTXShippingLTE, msg.dup());
	emit(scSignalTXShippingLTEtarget, centralAddress.toIPv4().getInt());
	// statistics - transmitter/receiver
	emit(scSignalTX, mL3Address.toIPv4().getInt());
	emit(scSignalTX, centralAddress.toIPv4().getInt());
	lordSocketShipping.sendTo(msg.dup(), centralAddress, lordPortShipping);
}

void LteD2DInterface::sendOrdering(FeudalMsg &msg){
	Enter_Method_Silent();
	inet::L3Address centralAddress = inet::L3AddressResolver().resolve(par("centralAddress"));
	// statistics
	emit(scSignalTXOrderingLTE, msg.dup());
	emit(scSignalTXOrderingLTEtarget, centralAddress.toIPv4().getInt());
	// statistics - transmitter/receiver
	emit(scSignalTX, mL3Address.toIPv4().getInt());
	emit(scSignalTX, centralAddress.toIPv4().getInt());
	lordSocketOrdering.sendTo(msg.dup(), centralAddress, lordPortOrdering);
}

void LteD2DInterface::sendAccordingToNic(WrappedCam *wCam, WrappedDenm *wDenm){
	std::string lteNic = getParentModule()->par("nicType");
	if(lteNic.compare("LteNicUeD2D") == 0){ // D2D selected (preconfigured tx params)
		if(wCam){
			// statistics
			emit(scSignalTXCamLTE, wCam->dup());
			emit(scSignalTXCamLTEtarget, mcastAddress.toIPv4().getInt());
			// statistics - transmitter/receiver
			emit(scSignalTX, mcastAddress.toIPv4().getInt());

			socket.sendTo(wCam->dup(), mcastAddress, mcastPort);

		} else if(wDenm){
			// statistics
			emit(scSignalTXDenmLTE, wDenm->dup());
			emit(scSignalTXDenmLTEtarget, mcastAddress.toIPv4().getInt());
			// statistics - transmitter/receiver
			emit(scSignalTX, mcastAddress.toIPv4().getInt());

			socket.sendTo(wDenm->dup(), mcastAddress, mcastPort);
		}
	} else if(lteNic.compare("LteNicUe") == 0){ // no D2D (no preconfigured tx params)
		std::string nodepath_root = getParentModule()->getParentModule()->getFullPath(); // "World"
		nodepath_root = nodepath_root + ".node[";
		int node_idx = 0;
		std::string nodepath = "";
		do{
			nodepath = nodepath_root + std::to_string(node_idx) + "]";
			if(getModuleByPath(nodepath.c_str())){
				inet::L3Address rx_addr = inet::L3AddressResolver().resolve(nodepath.c_str());
				if(rx_addr != mL3Address){
					if(wCam){
						// statistics
						emit(scSignalTXCamLTE, wCam->dup());
						emit(scSignalTXCamLTEtarget, rx_addr.toIPv4().getInt());
						// statistics - transmitter/receiver
						emit(scSignalTX, rx_addr.toIPv4().getInt());

						// only send to others, not own address
						broadcastSingleSocket.sendTo(wCam->dup(), rx_addr, broadcastSinglePort);

					} else if(wDenm){
						// statistics
						emit(scSignalTXDenmLTE, wDenm->dup());
						emit(scSignalTXDenmLTEtarget, rx_addr.toIPv4().getInt());
						// statistics - transmitter/receiver
						emit(scSignalTX, rx_addr.toIPv4().getInt());

						broadcastSingleSocket.sendTo(wDenm->dup(), rx_addr, broadcastSinglePort);
					}
				}
			}
			node_idx = node_idx + 1;
		} while(getModuleByPath(nodepath.c_str()));
	}
}

} //namespace artery
