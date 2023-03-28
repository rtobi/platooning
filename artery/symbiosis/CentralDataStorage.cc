/*
 * CentralDataStorage.cc
 *
 *      Author: rtobi
 */

#include "CentralDataStorage.h"
#include <vanetza/common/byte_buffer.hpp>
#include <algorithm>

namespace artery{

void CentralDataStorage::storeVassalageInfo(FeudalMsg& msg){
	uint32_t platoonId = msg.getPlatoonID();
	uint32_t vassalageId = msg.getVassalMsg(0).getVassalageID();

	if(platoonId == 0 || vassalageId == 0){
		// no valid platoonId and vassalageId indicated: no data can be stored
		return;
	}

	// process vassalage data
	FeudalDataStorage spds;
	spds.timestamp = msg.getTimestamp();
	spds.vassalageId = vassalageId;
	// extract number of delivered CAMs
	int number_of_CAMs = msg.getVassalMsg(0).getCamsArraySize();
	if(number_of_CAMs){
		for(unsigned int i = 0; i < number_of_CAMs; i++){
			// reconstruct CAM from byte buffer
			vanetza::asn1::Cam cam;
			vanetza::ByteBuffer buffer;
			//only consider non empty messages (empty messages may occur if not all CAMs were present at tx time)
			if(msg.getVassalMsg(0).getCams(i).getDataArraySize() > 0){
				for(unsigned int j = 0; j < msg.getVassalMsg(0).getCams(i).getDataArraySize(); j++){
					buffer.push_back(msg.getVassalMsg(0).getCams(i).getData(j));
				}
				bool decodingCAM = cam.decode(buffer);
				assert(decodingCAM == true);
				// update local map using the extracted cam
				spds.cams.push_back(cam);
				// store members
				spds.members.push_back(cam->header.stationID);
			}
		}
	}
	// extract number of delivered DENMs
	unsigned int number_of_DENMs = msg.getVassalMsg(0).getDenmsArraySize();
	if(number_of_DENMs){
		for(unsigned int i = 0; i < number_of_DENMs; i++){
			// reconstruct DENM from byte buffer
			vanetza::asn1::Denm denm;
			vanetza::ByteBuffer buffer;
			for(unsigned int j = 0; j < msg.getVassalMsg(0).getDenms(i).getDataArraySize(); j++){
				buffer.push_back(msg.getVassalMsg(0).getDenms(i).getData(j));
			}
			bool decodingDENM = denm.decode(buffer);
			assert(decodingDENM = true);
			// update local map using the extracted DENM
			spds.denms.push_back(denm);
		}
	}

	// process platoon data
	PlatoonDataStorage pds;
	pds.platoonId = platoonId;
	pds.vassalages.push_back(vassalageId);
	pds.storage.push_back(spds);

	std::vector<uint32_t>::iterator itPlat = std::find(platoons.begin(), platoons.end(), platoonId);
	if(itPlat != platoons.end()){
		// platoon is already present in storage: update
		int indexPlat = std::distance(platoons.begin(), itPlat);
		assert(indexPlat < cds.size());
		// look for vassalage within platoon
		std::vector<uint32_t>::iterator itSubPlat = std::find(cds.at(indexPlat).vassalages.begin(),
				cds.at(indexPlat).vassalages.end(), vassalageId);
		if(itSubPlat != cds.at(indexPlat).vassalages.end()){
			// vassalage is already present in storage: update
			int indexSubPlat = std::distance(cds.at(indexPlat).vassalages.begin(), itSubPlat);
			assert(indexSubPlat < cds.at(indexPlat).vassalages.size());
			if(cds.at(indexPlat).storage.at(indexSubPlat).timestamp < spds.timestamp){
				// update once new information is fresh
				cds.at(indexPlat).storage.at(indexSubPlat) = spds;
			} // else drop it
		} else{
			// vassalage not present: create new entry
			cds.at(indexPlat).vassalages.push_back(vassalageId);
			cds.at(indexPlat).storage.push_back(spds);
		}
	} else{
		// platoon not present: create new entry
		platoons.push_back(platoonId);
		cds.push_back(pds);
	}

}

FeudalMsg& CentralDataStorage::getPlatoonInfo(uint32_t platoonId, uint32_t vassalageId){
	FeudalMsg *msg = new FeudalMsg();
	// header information
	msg->setMsgType(MessageTypeFeudalisticPlatoon::Bridge);
	msg->setTxStationID(0);
	msg->setTimestamp(omnetpp::SimTime());
	msg->setPlatoonID(platoonId);

	// body information
	// look for platoon indicated by platoonId
	std::vector<uint32_t>::iterator itPlat = std::find(platoons.begin(), platoons.end(), platoonId);
	if(itPlat != platoons.end()){
		// platoon is present, get all vassalage data except indicated by vassalageId (requester's id)
		int indexPlat = std::distance(platoons.begin(), itPlat);
		assert(indexPlat < cds.size());
		int number_of_vassalages = cds.at(indexPlat).vassalages.size();
		// do not consider requester's vassalage
		msg->setVassalMsgArraySize(number_of_vassalages);
		// get data from all vassalages
		// for each vassalage create one bridgeMsg
		for(int i = 0; i < number_of_vassalages; i++){
			FeudalDataStorage spds = cds.at(indexPlat).storage.at(i);
			VassalMsg *bridgeMsg = new VassalMsg();
			bridgeMsg->setVassalageID(spds.vassalageId);
			// consider vassalages different from indicated id (requester's id)
			if(spds.vassalageId != vassalageId){
				// write CAMs
				bridgeMsg->setCamsArraySize(spds.cams.size());
				for(int j = 0; j < spds.cams.size(); j++){
					SimpleArrayMsg* simpleMsg = new SimpleArrayMsg();
					simpleMsg->setDataArraySize(spds.cams.at(j).encode().size());
					for(int k = 0; k < spds.cams.at(j).size(); k++){
						simpleMsg->setData(k, spds.cams.at(j).encode().at(k));
					}
					simpleMsg->setBitLength(simpleMsg->getDataArraySize() * 8);
					bridgeMsg->setCams(j, *simpleMsg);
				}
				// write DENMs
				bridgeMsg->setDenmsArraySize(spds.denms.size());
				for(int j = 0; j < spds.denms.size(); j++){
					SimpleArrayMsg* simpleMsg = new SimpleArrayMsg();
					simpleMsg->setDataArraySize(spds.denms.at(j).encode().size());
					for(int k = 0; k < spds.denms.at(j).size(); k++){
						simpleMsg->setData(k, spds.denms.at(j).encode().at(k));
					}
					simpleMsg->setBitLength(simpleMsg->getDataArraySize() * 8);
					bridgeMsg->setDenms(j, *simpleMsg);
				}
			}
			// determine packet size
			uint32_t tmp_byte_size = 0;
			for(unsigned int i = 0; i < bridgeMsg->getCamsArraySize(); i++){
				tmp_byte_size = tmp_byte_size + bridgeMsg->getCams(i).getByteLength();
			}
			for(unsigned int i = 0; i < bridgeMsg->getDenmsArraySize(); i++){
				tmp_byte_size = tmp_byte_size + bridgeMsg->getDenms(i).getByteLength();
			}
			bridgeMsg->setBitLength((sizeof(uint32_t) + tmp_byte_size)*8);

			// write vassalage's CAMs and DENMs. Multiple bridgeMsgs are possible
			msg->setVassalMsg(i, *bridgeMsg);
		}
	} else{
		// platoon not found: no data available
	}
	uint32_t bridges_byte_size = 0;
	for(unsigned int i = 0; i < msg->getVassalMsgArraySize(); i++){
		bridges_byte_size = bridges_byte_size + msg->getVassalMsg(i).getByteLength();
	}
	msg->setBitLength((sizeof(uint8_t) + 3*sizeof(uint32_t) + sizeof(omnetpp::simtime_t) +
			bridges_byte_size) * 8);

	return *msg;
}


} // namespace artery
