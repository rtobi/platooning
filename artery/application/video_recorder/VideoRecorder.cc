/*
 * Videorecorder.cc
 *
 *      Author: rtobi
 */

#include "artery/application/video_recorder/VideoRecorder.h"
using namespace omnetpp;
using namespace vanetza;

namespace artery
{

Define_Module(VideoRecorder);

VideoRecorder::VideoRecorder():
        mVehicleController(nullptr),
        mInterval(0.1)
        {
        }

void VideoRecorder::initialize()
{
    ItsG5Service::initialize();
    mVehicleController = getFacilities().get_mutable_ptr<traci::VehicleController>();
    mInterval = par("frameInterval");
    mLastRecord = simTime();
}

void VideoRecorder::trigger()
{
    Enter_Method("trigger");
    if(simTime() - mLastRecord > mInterval){
        //store screenshots
        int digits = 10;
        std::string tmp = std::to_string((long)((simTime().raw()/(double)simTime().getScale())*1000000));
        std::string pictureName = "images/" + std::string(digits - tmp.length(), '0') + tmp + ".png";

        mVehicleController->getLiteAPI().gui().screenshot("View #0", pictureName);
    }

}


} // namespace artery
