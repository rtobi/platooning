/*
 * CentralDataStorage.h
 *
 *      Author: rtobi
 */

#ifndef SRC_ARTERY_SYMBIOSIS_CENTRALDATASTORAGE_H_
#define SRC_ARTERY_SYMBIOSIS_CENTRALDATASTORAGE_H_

#include <map>
#include <vector>
#include <vanetza/asn1/cam.hpp>
#include <vanetza/asn1/denm.hpp>
#include <omnetpp/simtime.h>
#include "platoon_msgs/FeudalMsg_m.h"

namespace artery{

struct FeudalDataStorage{
	uint32_t vassalageId;
	std::vector<uint32_t> members;
	std::vector<vanetza::asn1::Cam> cams;
	std::vector<vanetza::asn1::Denm> denms;
	omnetpp::SimTime timestamp;
};

struct PlatoonDataStorage{
	uint32_t platoonId;
	std::vector<uint32_t> vassalages;
	std::vector<FeudalDataStorage> storage;
};

class CentralDataStorage{
	public:
		void storeVassalageInfo(FeudalMsg& msg);
		FeudalMsg& getPlatoonInfo(uint32_t platoonId, uint32_t vassalageId);

	private:
		std::vector<uint32_t> platoons;
		std::vector<PlatoonDataStorage> cds;
};

} // namespace artery

#endif /* SRC_ARTERY_SYMBIOSIS_CENTRALDATASTORAGE_H_ */
