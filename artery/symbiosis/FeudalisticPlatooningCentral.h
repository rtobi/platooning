/*
 * FeudalisticPlatooningCentral.h
 *
 *      Author: rtobi
 */

#ifndef FEUDALISTICPLATOONINGCENTRAL_H_
#define FEUDALISTICPLATOONINGCENTRAL_H_

#include <inet/networklayer/common/L3Address.h>
#include <inet/transportlayer/contract/udp/UDPSocket.h>
#include <omnetpp/csimplemodule.h>
#include <list>
#include "platoon_msgs/FeudalMsg_m.h"
#include "CentralDataStorage.h"

namespace artery{

class FeudalisticPlatooningCentral : public omnetpp::cSimpleModule
{
public:
    ~FeudalisticPlatooningCentral();

protected:
    void initialize() override;
    void finish() override;
    void handleMessage(omnetpp::cMessage*) override;

private:
    void processPacket(omnetpp::cPacket*);
    void processReport(FeudalMsg&);
    void processQuery(FeudalMsg&, const inet::L3Address&, int port);
    void disseminateWarning();

    int reportPort;
    int queryPort;
    int numReceivedWarnings;
    int numReceivedQueries;
    inet::UDPSocket reportSocket;
    inet::UDPSocket querySocket;
    std::list<FeudalMsg> reports;
    inet::L3Address mL3Address;
    bool mL3AddressIsSet;

    CentralDataStorage cds;
};

} // namespace artery
#endif /* FEUDALISTICPLATOONINGCENTRAL_H_ */
