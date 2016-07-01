/*
 * RelaySettings.cpp
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */
#include "relaysettings.h"

RelaySettings::RelaySettings():
    enabled(false), start_time(10), on_time(20), off_time(20) { }


RelaySettings::RelaySettings(bool enabled, uint start_time, uint on_time, uint off_time):
    enabled(enabled), start_time(start_time), on_time(on_time), off_time(off_time) { }
