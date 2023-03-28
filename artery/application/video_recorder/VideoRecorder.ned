//
// Videorecorder.ned
//
//      Author: rtobi
//
// Use this recorder to create screenshot of sumo every frameInterval.
// After the simulation, use the command line tool ffmpeg to merge it to a video.
// Use this command: Assuming you are using a framerate of 25, navigate to the folder
// where the images are stored, and execute ***ffmpeg -framerate 25 -pattern_type glob -i '*.png' sumo.mp4***


package artery.application.video_recorder;
import artery.application.ItsG5Service;

simple VideoRecorder like ItsG5Service
{
    parameters:
        double frameInterval @unit(s) = default(0.04s); //FPS = 25; make sure middleware update interval in omnetpp.ini is at least as small as this interval
}

