/*
 * RelaySettings.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */
#ifndef RELAYSETTINGS_H
#define RELAYSETTINGS_H

#include <QtGlobal>

struct RelaySettings
{
public:
    RelaySettings();
    RelaySettings(bool enabled, uint start_time, uint on_time, uint off_time);
    bool enabled;
    uint start_time; // seconds
    uint on_time;    // seconds
    uint off_time;   // seconds
};

#endif // RELAYSETTINGS_H
