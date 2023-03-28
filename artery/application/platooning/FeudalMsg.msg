//
// FeudalMsg.msg
//
//      Author: rtobi
//
cplusplus {{
#include "artery/application/platooning/FeudalisticPlatooningDefinitions.h"
}};

enum MessageTypeFeudalisticPlatoon;

packet SimpleArrayMsg
{
    uint8_t data[];
}

packet VassalMsg
{
   uint32_t vassalageID;
   SimpleArrayMsg cams[];
   SimpleArrayMsg denms[];
};

packet FeudalSync
{
    uint16_t ref;
    uint32_t members[];
    uint32_t lteAddresses[];
}

packet FeudalMsg
{
    uint8_t msgType @enum(MessageTypeFeudalisticPlatoon);
    uint32_t txStationID;
    simtime_t timestamp;
    uint32_t platoonID;
    uint32_t vassalageID;
    FeudalSync syncMsg[];
    VassalMsg vassalMsg[];
};

