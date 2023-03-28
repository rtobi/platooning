//
// FeudalisticPlatooningService.ned
//
//      Author: rtobi
//
 package artery.application.platooning;

import artery.application.ItsG5Service;

simple FeudalisticPlatooningService like ItsG5Service
{   	
    parameters:
		@class(platooning::FeudalisticPlatooningService);
		
		// SIGNALS for LteD2DInterface
		@signal[BridgeExport](type=FeudalMsg);
		@signal[BridgeImport](type=FeudalMsg);
		@signal[LordShipping](type=FeudalMsg);
		@signal[LordOrdering](type=FeudalMsg);
		@signal[LordTrading](type=FeudalMsg);
		
		int vassalageSize = default(3);
		int vassalageDivideFactor = default(4);
		int feudalisticTimeOut @unit(ms) = default(5000ms);
		int vassalageMerchantInterval @unit(ms) = default(1000ms);
		int vassalageLordShippingInterval @unit(ms) = default(2000ms);
		int vassalageLordOrderingInterval @unit(ms) = default(2000ms);
		int feudalisticInitMaxBackoff @unit(ms) = default(500ms);
		int feudalisticIvDistanceInner @unit(m) = default(8m);
		int feudalisticIvDistanceBetween @unit(m) = default(70m);
		
		// SIGNALS for STATISTICS
		@signal[TXInitWLAN](type=omnetpp::cPacket);
		@statistic[transmissionInitWLAN](source=packetBytes(TXInitWLAN); record=vector?);
		@signal[RXInitWLAN](type=omnetpp::cPacket);
		@statistic[receptionInitWLAN](source=packetBytes(RXInitWLAN); record=vector?);
		// for lte addresses
		@signal[LteAddrVeh](type=unsigned long);
		@statistic[lteAddressVehicle](source=LteAddrVeh; record=vector?);
		// for role
		@signal[FeudalRole](type=unsigned long);
		@statistic[feudalRole](source=FeudalRole; record=vector?);
		// for vassalageId
		@signal[VassalageId](type=unsigned long);
		@statistic[vassalageId](source=VassalageId; record=vector?);
		
		// colorset for vehicles
		int numberOfColorSet = default(1);
}
