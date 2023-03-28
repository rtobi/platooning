//
// LteD2DInterface.ned
//
//      Author: rtobi
//
package artery.symbiosis;

import inet.applications.contract.IUDPApp;

simple LteD2DInterface like IUDPApp
{
    parameters:
        int localPort = default(9330);
        int mcastPort = default(9331);
        int bridgePort = default(9332); // importing exporting of vassalage msgs
        int lordPortShipping = default(9333); // reporting vassalage's cams and denms
        int lordPortOrdering = default(9334); // asking for all platoon's cams and denms
        int broadcastSinglePort = default(9335);
        string mcastAddress = default("224.0.0.23");
        string middlewareModule;
        string interfaceTableModule;
        
        string centralAddress;
        
        // SIGNALS and STATISTICS
        // TX - transmission
        @signal[TXCamLTE](type=omnetpp::cPacket);
        @statistic[transmissionCamLTE](source=packetBytes(TXCamLTE); record=vector?);
        @signal[TXCamLTEtarget](type=unsigned long);
        @statistic[transmissionCamLTEtarget](source=TXCamLTEtarget; record=vector?);
        @signal[TXDenmLTE](type=omnetpp::cPacket);
        @statistic[transmissionDenmLTE](source=packetBytes(TXDenmLTE); record=vector?);
        @signal[TXDenmLTEtarget](type=unsigned long);
        @statistic[transmissionDenmLTEtarget](source=TXDenmLTEtarget; record=vector?);
        @signal[TXBridgeExportLTE](type=omnetpp::cPacket);
        @statistic[transmissionExportBridgeLTE](source=packetBytes(TXBridgeExportLTE); record=vector?);
        @signal[TXBridgeExportLTEtarget](type=unsigned long);
        @statistic[transmissionExportBridgeLTEtarget](source=TXBridgeExportLTEtarget; record=vector?);
        @signal[TXBridgeImportLTE](type=omnetpp::cPacket);
        @statistic[transmissionImportBridgeLTE](source=packetBytes(TXBridgeImportLTE);record=vector?);
        @signal[TXBridgeImportLTEtarget](type=unsigned long);
        @statistic[transmissionImportBridgeLTEtarget](source=TXBridgeImportLTEtarget; record=vector?);
        @signal[TXLordTradeLTE](type=omnetpp::cPacket);
        @statistic[transmissionTradeLTE](source=packetBytes(TXLordTradeLTE);record=vector?);
        @signal[TXLordTradeLTEtarget](type=unsigned long);
        @statistic[transmissionTradeLTEtarget](source=TXLordTradeLTEtarget; record=vector?);
        @signal[TXShippingLTE](type=omnetpp::cPacket);
        @statistic[transmissionShippingLTE](source=packetBytes(TXShippingLTE); record=vector?);
        @signal[TXShippingLTEtarget](type=unsigned long);
        @statistic[transmissionShippingLTEtarget](source=TXShippingLTEtarget; record=vector?);
        @signal[TXOrderingLTE](type=omnetpp::cPacket);
        @statistic[transmissionOrderingLTE](source=packetBytes(TXOrderingLTE); record=vector?);
        @signal[TXOrderingLTEtarget](type=unsigned long);
        @statistic[transmissionOrderingLTEtarget](source=TXOrderingLTEtarget; record=vector?);
        // general tx
        @signal[tx](type=unsigned long);
        @statistic[transmission](source=tx; record=vector?);
        
        //RX - reception
        @signal[RXCamLTE](type=omnetpp::cPacket);
        @statistic[receptionCamLTE](source=packetBytes(RXCamLTE);record=vector?);
        @signal[RXDenmLTE](type=omnetpp::cPacket);
        @statistic[receptionDenmLTE](source=packetBytes(RXDenmLTE);record=vector?);
        @signal[RXBridgeLTE](type=omnetpp::cPacket);
        @statistic[receptionBridgeLTE](source=packetBytes(RXBridgeLTE);record=vector?);
        @signal[RXDistributionLTE](type=omnetpp::cPacket);
        @statistic[receptionDistributionLTE](source=packetBytes(RXDistributionLTE);record=vector?);
        @signal[RXOrderingLTE](type=omnetpp::cPacket);
        @statistic[receptionOrderingLTE](source=packetBytes(RXOrderingLTE);record=vector?);
        // general rx
        @signal[rx](type=unsigned long);
        @statistic[reception](source=rx; record=vector?);
        
        // address
        @signal[LteAddressMulticast](type=unsigned long);
        @statistic[lteAddressMulticast](source=LteAddressMulticast; record=vector?);
        
    gates:
        output udpOut;
        input udpIn;
}
