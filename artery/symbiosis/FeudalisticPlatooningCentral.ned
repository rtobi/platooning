//
// FeudalisticPlatooningCentral.ned
//
//      Author: rtobi
//

package artery.symbiosis;

import inet.applications.contract.IUDPApp;

simple FeudalisticPlatooningCentral like IUDPApp
{
    parameters:
        int reportPort = default(9333);
        int queryPort = default(9334);
        
        // SIGNALS and STATISTICS
        // address
        @signal[LteAddrCentral](type=unsigned long);
        @statistic[lteAddressCentral](source=LteAddrCentral; record=vector?);
        // transmission
        @signal[TXDeliveringLTE](type=omnetpp::cPacket);
        @statistic[transmissionDeliveringLTE](source=packetBytes(TXDeliveringLTE); record=vector?);
        @signal[TXDeliveringLTEtarget](type=unsigned long);
        @statistic[transmissionDeliveringLTEtarget](source=TXDeliveringLTEtarget; record=vector?);
        // reception
        @signal[RXShippingLTE](type=omnetpp::cPacket);
        @statistic[receptionShippingLTE](source=packetBytes(RXShippingLTE); record=vector?);

    gates:
        output udpOut;
        input udpIn;
}