/*
 * FeudalisticPlatooningService.h
 *
 *      Author: rtobi
 */
#ifndef FEUDALISTICPLATOONINGSERVICE_H_
#define FEUDALISTICPLATOONINGSERVICE_H_

#include "artery/application/ItsG5Service.h"
#include "artery/application/VehicleDataProvider.h"
#include "artery/application/platooning/PlatooningService.h"
#include "artery/application/platooning/PlatooningServiceEnvmod.h"
#include "artery/application/DenService.h"
#include "FeudalisticPlatooningDefinitions.h"
#include "platoon_msgs/FeudalMsg_m.h"
#include <omnetpp/simtime.h>
#include <inet/networklayer/common/L3Address.h>
#include "misc/colorset.h"

// forward declaration
namespace traci { class VehicleController; }

namespace artery{

struct VassalMap{
	uint32_t platoonId;
	uint32_t vassalageId;
	uint32_t depth;
	uint32_t divideFactor;
	std::vector<uint32_t> members;
	std::vector<uint32_t> lteAddresses;
	uint32_t ivDistanceInner;
	uint8_t role;
	uint32_t innerPosition;
	omnetpp::SimTime timeStamp;
	int32_t ref;
	std::vector<bool> sync;
};

struct Timing{
	uint64_t last;
	uint64_t interval;
};

struct SyncTiming{
	bool flag;
	Timing timing;
};

namespace platooning
{
class FeudalisticPlatooningService : public artery::ItsG5Service
{
    public:
        //FeudalisticPlatooningService();
        void trigger() override;
        const VassalMap& getVassalMap(){return mVassalMap;};
        void indicateBridge(omnetpp::cPacket* cPack){
        	vanetza::btp::DataIndication tmp;
        	indicate(tmp, cPack->dup());
        	delete cPack;
        };

    protected:
        void initialize() override;
        void indicate(const vanetza::btp::DataIndication&, omnetpp::cPacket*) override;
        traci::VehicleController* mVehicleController = nullptr;

    private:
        const VehicleDataProvider* mVehicleDataProvider;
        PlatooningService* mPlatooningService = nullptr;
        PlatooningServiceEnvmod* mPlatooningServiceEnvmod = nullptr;
        DenService* mDenService = nullptr;
        PlatoonMap mPlatMap;
        VassalMap mVassalMap;
        FeudalMsg* syncReBroadcastMsg;
        Timing mFeudalisticPlatoonTimeOut;
        Timing mVassalageTimeMerchant;
        Timing mVassalageTimeLordShipping;
        Timing mVassalageTimeLordOrdering;
        SyncTiming mSyncTiming;
        const Timer* mTimer;
        LocalDynamicMap* mLocalDynamicMap;
        bool mLteAvailable;
        inet::L3Address mL3Address;

        std::vector<libsumo::TraCIColor> mColorset;

        void updateLocalMapAndMemory(VassalMsg *bridgeMsg);
        void setupFeudalisticPlatooning();
        void printVassalMap();
        void printVassalMapToFile();
        VassalMsg& packVassalagesCamsAndDenms();
};

} //namespace platooning
} //namespace artery

#endif /* FEUDALISTICPLATOONINGSERVICE_H_ */
