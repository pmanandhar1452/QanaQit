/*
 * Global.h
 *
 *  Created on: Nov 17, 2011
 *      Author: Prakash Manandhar
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include <QString>
#include <QSettings>
#include "relaysettings.h"

class Global: public QObject {
        Q_OBJECT
public:
        static const int N_RELAYS = 4; // number of relays
        static const int N_DAQ    = 6; // number of ADC channels

        // singleton
        static Global * instance () {
                if(pI == NULL)
                        pI = new Global();
                return pI;
        }

        static void destroy () {
                if (pI != NULL)
                        delete pI;
                pI = NULL;
        }

        static QString toHrMinSec(double T); // convert from seconds to hour minute second

        QString getDataFolder    ();
        QVector<bool> getChannelSelect ();    // whether the given channel is to be sampled
        QVector<RelaySettings> getRelaySettings ();      // whether a given relay is selected or not
        void setRelaySettings(const QVector<RelaySettings>&);

        int getDataCollectionTime ();

public slots:
        void setDataFolder (const QString&);
        void setChannelSelect(const QVector<bool>&); // set a given channel as selected or not

        void setDataCollectionTime (const int&);

private:
        Global();
        virtual ~Global();
        static Global * pI;
        QSettings * settings;
};

#endif /* GLOBAL_H_ */
