/*
 * Platoon Message
 * Copyright 2018 Tobias Renzler
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

cplusplus {{
#include "/home/tobi/Software/artery_eclipse/artery/scenarios/platooning/PlatooningService.h"
}};

enum MessageTypePlatoon;

packet PlatoonMsg
{
    uint32_t msgType @enum(MessageTypePlatoon);
    uint32_t txStationID;
    uint32_t platoonID;
    uint32_t innerPosition;
    uint32_t depth;
    uint32_t nonce;
    uint32_t nonceRef;
    simtime_t timestamp;
    uint32_t ivDistance;
    string edgeID;
    string laneID;
    uint32_t members[];
};