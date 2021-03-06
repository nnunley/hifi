//
//  Snapshot.cpp
//  interface/src/ui
//
//  Created by Stojce Slavkovski on 1/26/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <QDateTime>
#include <QFileInfo>

#include <AccountManager.h>
#include <FileUtils.h>

#include "Snapshot.h"
#include "Menu.h"

// filename format: hifi-snap-by-%username%-on-%date%_%time%_@-%location%.jpg
// %1 <= username, %2 <= date and time, %3 <= current location
const QString FILENAME_PATH_FORMAT = "hifi-snap-by-%1-on-%2@%3.jpg";

const QString DATETIME_FORMAT = "yyyy-MM-dd_hh-mm-ss";
const QString SNAPSHOTS_DIRECTORY = "Snapshots";

const QString LOCATION_X = "location-x";
const QString LOCATION_Y = "location-y";
const QString LOCATION_Z = "location-z";

const QString ORIENTATION_X = "orientation-x";
const QString ORIENTATION_Y = "orientation-y";
const QString ORIENTATION_Z = "orientation-z";
const QString ORIENTATION_W = "orientation-w";

const QString DOMAIN_KEY = "domain";

SnapshotMetaData* Snapshot::parseSnapshotData(QString snapshotPath) {
    
    if (!QFile(snapshotPath).exists()) {
        return NULL;
    }
    
    QImage shot(snapshotPath);
    
    // no location data stored
    if (shot.text(LOCATION_X).isEmpty() || shot.text(LOCATION_Y).isEmpty() || shot.text(LOCATION_Z).isEmpty()) {
        return NULL;
    }
    
    SnapshotMetaData* data = new SnapshotMetaData();
    data->setLocation(glm::vec3(shot.text(LOCATION_X).toFloat(),
                                shot.text(LOCATION_Y).toFloat(),
                                shot.text(LOCATION_Z).toFloat()));
    
    data->setOrientation(glm::quat(shot.text(ORIENTATION_W).toFloat(),
                                   shot.text(ORIENTATION_X).toFloat(),
                                   shot.text(ORIENTATION_Y).toFloat(),
                                   shot.text(ORIENTATION_Z).toFloat()));
    
    data->setDomain(shot.text(DOMAIN_KEY));
                                   
    return data;
}

void Snapshot::saveSnapshot(QGLWidget* widget, Avatar* avatar) {
    QImage shot = widget->grabFrameBuffer();
    
    glm::vec3 location = avatar->getPosition();
    glm::quat orientation = avatar->getHead()->getOrientation();
    
    // add metadata
    shot.setText(LOCATION_X, QString::number(location.x));
    shot.setText(LOCATION_Y, QString::number(location.y));
    shot.setText(LOCATION_Z, QString::number(location.z));
    
    shot.setText(ORIENTATION_X, QString::number(orientation.x));
    shot.setText(ORIENTATION_Y, QString::number(orientation.y));
    shot.setText(ORIENTATION_Z, QString::number(orientation.z));
    shot.setText(ORIENTATION_W, QString::number(orientation.w));
    
    shot.setText(DOMAIN_KEY, NodeList::getInstance()->getDomainHandler().getHostname());

    QString formattedLocation = QString("%1_%2_%3").arg(location.x).arg(location.y).arg(location.z);
    // replace decimal . with '-'
    formattedLocation.replace('.', '-');
    
    QString username = AccountManager::getInstance().getUsername();
    // normalize username, replace all non alphanumeric with '-'
    username.replace(QRegExp("[^A-Za-z0-9_]"), "-");
    
    QDateTime now = QDateTime::currentDateTime();
    QString fileName = Menu::getInstance()->getSnapshotsLocation();

    if (!fileName.endsWith(QDir::separator())) {
        fileName.append(QDir::separator());
    }

    fileName.append(QString(FILENAME_PATH_FORMAT.arg(username, now.toString(DATETIME_FORMAT), formattedLocation)));
    shot.save(fileName, 0, 100);
}


