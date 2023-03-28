/*
 * FeudalisticPlatooningCentral.cc
 *
 *      Author: rtobi
 */

#include <artery/symbiosis/FeudalisticPlatooningCentral.h>
#include <inet/networklayer/common/L3AddressResolver.h>
#include <inet/transportlayer/contract/udp/UDPControlInfo.h>
#include <fstream>

namespace artery{

using namespace omnetpp;

Define_Module(FeudalisticPlatooningCentral)

static const simsignal_t scSignalLteAddrCentral = cComponent::registerSignal("LteAddrCentral");
static const simsignal_t scSignalTXDeliveringLTE = cComponent::registerSignal("TXDeliveringLTE");
static const simsignal_t scSignalTXDeliveringLTEtarget = cComponent::registerSignal("TXDeliveringLTEtarget");
static const simsignal_t scSignalRXShippingLTE = cComponent::registerSignal("RXShippingLTE");

FeudalisticPlatooningCentral::~FeudalisticPlatooningCentral()
{
}

void FeudalisticPlatooningCentral::initialize()
{
    reportPort = par("reportPort");
    reportSocket.setOutputGate(gate("udpOut"));
    reportSocket.bind(inet::L3Address(), reportPort);

    queryPort = par("queryPort");
    querySocket.setOutputGate(gate("udpOut"));
    querySocket.bind(inet::L3Address(), queryPort);

    numReceivedWarnings = 0;
    numReceivedQueries = 0;
    WATCH(numReceivedWarnings);
    WATCH(numReceivedQueries);

    mL3AddressIsSet = false;
}

void FeudalisticPlatooningCentral::finish()
{
    reportSocket.close();
    querySocket.close();

    recordScalar("numReceivedWarnings", numReceivedWarnings);
    recordScalar("numReceivedQueries", numReceivedQueries);
}

void FeudalisticPlatooningCentral::handleMessage(cMessage* msg)
{
    if (msg->getKind() == inet::UDP_I_DATA) {
        processPacket(PK(msg));
    } else if (msg->getKind() == inet::UDP_I_ERROR) {
        EV_ERROR << "UDP error occurred\n";
        delete msg;
    } else {
        throw cRuntimeError("Unrecognized message (%s)%s", msg->getClassName(), msg->getName());
    }
}

void FeudalisticPlatooningCentral::processPacket(cPacket* pkt)
{
    auto ctrl = pkt->getControlInfo();
    if (auto udp = dynamic_cast<inet::UDPDataIndication*>(ctrl)) {
        if (udp->getDestPort() == reportPort) {
            processReport(*check_and_cast<FeudalMsg*>(pkt));
        } else if (udp->getDestPort() == queryPort) {
            processQuery(*check_and_cast<FeudalMsg*>(pkt), udp->getSrcAddr(), udp->getSrcPort());
        } else {
            throw cRuntimeError("Unknown UDP destination port %d", udp->getDestPort());
        }

        if(!mL3AddressIsSet){
        	mL3Address = udp->getDestAddr();
        	mL3AddressIsSet = true;
        	emit(scSignalLteAddrCentral, mL3Address.toIPv4().getInt());
        }
    }
    delete pkt;
}

void FeudalisticPlatooningCentral::processReport(FeudalMsg& report)
{
	std::cout << "[CENTRAL] received FeudalMsg from " << report.getTxStationID() << std::endl;
	// log reception of shipping
	emit(scSignalRXShippingLTE, report.dup());
	// store info within central data storage
	cds.storeVassalageInfo(report);
}

void FeudalisticPlatooningCentral::processQuery(FeudalMsg& query, const inet::L3Address& addr, int port)
{
	FeudalMsg response;
	// once no bridgeMsg is present, the query can not be processed. Answer with an Empty message
	response.setMsgType(MessageTypeFeudalisticPlatoon::Empty);

	if(query.getVassalMsgArraySize()){
		// once bridgeMsg is present, use it vassalageId together with the platoonId from the vassalageMsg
		// to look for all vassalage data except vassalageId
		response = cds.getPlatoonInfo(query.getPlatoonID(), query.getVassalMsg(0).getVassalageID());
		// set type to ordering
		response.setMsgType(MessageTypeFeudalisticPlatoon::Ordering);
		// log transmission of answer to order
		emit(scSignalTXDeliveringLTE, response.dup());
		emit(scSignalTXDeliveringLTEtarget, addr.toIPv4().getInt());
	}

	// send response to the addr and port from where the query came
	querySocket.sendTo(response.dup(), addr, port);
}

} // namespace artery


