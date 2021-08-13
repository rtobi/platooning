/*
 * ReadyToPlatoon Message
 * Copyright 2018 Tobias Renzler
 * Licensed under GPLv2, see COPYING file for detailed license and warranty terms.
 */

packet ReadyToPlatoonMsg
{
    uint32_t txStationID;
    uint32_t rxStationID;
    string txTraciID;
    string rxTraciID; 
    bool ready;
};