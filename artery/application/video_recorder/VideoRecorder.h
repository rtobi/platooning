/*
 * Videorecorder.h
 *
 *      Author: rtobi
 */
#ifndef VIDEORECORDER_H_
#define VIDEORECORDER_H_

#include "artery/application/ItsG5Service.h"
#include "artery/envmod/LocalEnvironmentModel.h"
#include <string>
#include "artery/application/VehicleDataProvider.h"
#include "artery/traci/VehicleController.h"
#include <omnetpp/csimulation.h>

namespace artery
{

/**
 * Prints currently visible objects (i.e. vehicles) as seen via Radar or CA messages.
 * This service is meant for debugging and demonstration purposes.
 */
class VideoRecorder : public ItsG5Service
{
    public:
        VideoRecorder();
        void initialize() override;
        void trigger() override;

    private:
        traci::VehicleController* mVehicleController;
        omnetpp::SimTime mLastRecord;
        omnetpp::SimTime mInterval;

};

} // namespace artery

#endif /* VIDEORECORDER_H_ */
