/*
 * LteD2DInterface.h
 *
 *      Author: rtobi
 */
#ifndef LTED2DINTERFACE_H
#define LTED2DINTERFACE_H

#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <omnetpp/clistener.h>
#include <omnetpp/csimplemodule.h>
#include <vanetza/asn1/cam.hpp>
#include <vanetza/asn1/denm.hpp>
#include <artery/application/LocalDynamicMap.h>
#include <artery/application/platooning/FeudalisticPlatooningService.h>
#include "artery/application/DenService.h"
#include "platoon_msgs/FeudalMsg_m.h"
#include "symbiosis_msgs/Wrapper_m.h"
#include "artery/application/platooning/PlatooningService.h"

// forward declaration
namespace traci { class VehicleController; }

namespace artery
{

class LteD2DInterface : public omnetpp::cSimpleModule, public omnetpp::cListener
{
public:
    ~LteD2DInterface();

protected:
    int numInitStages() const override;
    void initialize(int stage) override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;
    void receiveSignal(omnetpp::cComponent*, omnetpp::simsignal_t, omnetpp::cObject*, omnetpp::cObject*) override;

private:
    void sendReport();
    void sendCam(vanetza::asn1::Cam &cam);
    void sendDenm(vanetza::asn1::Denm &denm);
    void sendFeudalMsg(FeudalMsg &msg, uint32_t lteDestAddr);
    void sendShipping(FeudalMsg &msg);
    void sendOrdering(FeudalMsg &msg);
    void sendAccordingToNic(WrappedCam *wCam, WrappedDenm *wDenm);

    inet::UDPSocket socket;
    inet::L3Address mcastAddress;
    inet::L3Address mL3Address;
    int mcastPort;
    int numWarningsPeer;
    int tractionLosses;

    inet::UDPSocket broadcastSingleSocket;
    int broadcastSinglePort;

    inet::UDPSocket bridgeSocket;
    int bridgePort;

    inet::UDPSocket lordSocketShipping;
    inet::UDPSocket lordSocketOrdering;
    int lordPortShipping;
    int lordPortOrdering;

    traci::VehicleController* mVehicleController = nullptr;
    const VehicleDataProvider* mVehicleDataProvider;
    artery::LocalDynamicMap* mLocalDynamicMap = nullptr;
    artery::DenService* mDenService = nullptr;
    artery::platooning::FeudalisticPlatooningService* mFeudalisticPlatooningService = nullptr;

    simsignal_t scSignalCamSent80211p;
    simsignal_t scSignalDenmSent80211p;
    simsignal_t scSignalBridgeExport;
    simsignal_t scSignalBridgeImport;
    simsignal_t scSignalLordShipping;
    simsignal_t scSignalLordOrdering;
    simsignal_t scSignalLordTrading;
};

} //namespace artery
#endif /* LTED2DINTERFACE_H */

