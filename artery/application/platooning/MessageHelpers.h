/*
 * MessageHelpers.h
 *
 *      Author: rtobi
 */

#ifndef SRC_ARTERY_APPLICATION_PLATOONING_MESSAGEHELPERS_H_
#define SRC_ARTERY_APPLICATION_PLATOONING_MESSAGEHELPERS_H_

#include "platoon_msgs/FeudalMsg_m.h"
#include "omnetpp.h"

FeudalSync* createSyncMsg(uint16_t ref, std::vector<uint32_t> members, std::vector<uint32_t> addresses){
	FeudalSync *syncMsg = new FeudalSync();
	syncMsg->setRef(ref);
	syncMsg->setMembersArraySize(members.size());
	for(unsigned int i = 0; i < members.size(); i++){
		syncMsg->setMembers(i, members.at(i));
	}

	syncMsg->setLteAddressesArraySize(addresses.size());
	for(unsigned int i = 0; i < addresses.size(); i++){
		syncMsg->setLteAddresses(i, addresses.at(i));
	}

	syncMsg->setBitLength((sizeof(uint16_t) + syncMsg->getMembersArraySize() + syncMsg->getLteAddressesArraySize()) * 8);

	return syncMsg;
}

FeudalMsg* createFeudalMsg(FeudalSync* syncMsg, VassalMsg* bridgeMsg, 
	uint32_t txStationId, uint32_t platoonId, uint32_t vassalageId){

	FeudalMsg *msg = new FeudalMsg();
	msg->setTxStationID(txStationId);
	msg->setTimestamp(simTime());
	msg->setPlatoonID(platoonId);
	msg->setVassalageID(vassalageId);
	uint32_t tmp_byte_size = 0;
	if(syncMsg){
		msg->setMsgType(MessageTypeFeudalisticPlatoon::Init);
		msg->setSyncMsgArraySize(1);
		msg->setSyncMsg(0, *syncMsg);
		msg->setVassalMsgArraySize(0);
		tmp_byte_size = sizeof(uint16_t) + sizeof(uint32_t)*syncMsg->getMembersArraySize()
				+ sizeof(uint32_t)*syncMsg->getLteAddressesArraySize();
	} else if(bridgeMsg){
		msg->setMsgType(MessageTypeFeudalisticPlatoon::Bridge);
		msg->setSyncMsgArraySize(0);
		msg->setVassalMsgArraySize(1);
		msg->setVassalMsg(0, *bridgeMsg);
		tmp_byte_size = bridgeMsg->getByteLength();
	} else{
		assert(syncMsg == nullptr && bridgeMsg == nullptr);
	}

	msg->setBitLength((sizeof(uint8_t) + 3*sizeof(uint32_t) + sizeof(omnetpp::simtime_t) +
			tmp_byte_size) * 8);
	return msg;
}

/*
 * Creates feudalMsg containing multiple bridge messages, indicated by bridges
 */
FeudalMsg* createFeudalMsgMultipleBridge(std::vector<VassalMsg*> bridges,
		uint32_t txStationId, uint32_t platoonId, uint32_t vassalageId){
	FeudalMsg *msg = new FeudalMsg();
	msg->setTxStationID(txStationId);
	msg->setTimestamp(simTime());
	msg->setPlatoonID(platoonId);
	msg->setVassalageID(vassalageId);
	msg->setMsgType(MessageTypeFeudalisticPlatoon::Bridge);
	msg->setSyncMsgArraySize(0);
	msg->setVassalMsgArraySize(bridges.size());
	uint32_t tmp_byte_size = 0;
	for(int i = 0; i < bridges.size(); i++){
		msg->setVassalMsg(i, *bridges.at(i));
		tmp_byte_size = tmp_byte_size + bridges.at(i)->getByteLength();
	}

	msg->setBitLength((sizeof(uint8_t) + 2*sizeof(uint32_t) + sizeof(omnetpp::simtime_t) +
				tmp_byte_size) * 8);

	return msg;
}

omnetpp::cPacket* emptySignalPacket(uint32_t size){
    omnetpp::cPacket* cpak = new omnetpp::cPacket();
    cpak->setByteLength((long)size);
    cpak->setBitLength(cpak->getByteLength()*8);
    return cpak;
}



#endif /* SRC_ARTERY_APPLICATION_PLATOONING_MESSAGEHELPERS_H_ */
