/*
 * QanaMeasurement.h
 *
 *  Created on: Nov 17, 2011
 *      Author: PManandhar
 */
#ifndef QANAMEASUREMENT_H
#define QANAMEASUREMENT_H

#include "Global.h"

class QanaMeasurement
{
public:
    QanaMeasurement();
    volatile int time;
    volatile bool relays[Global::N_RELAYS];
    volatile double daq[Global::N_RELAYS];
};

#endif // QANAMEASUREMENT_H
