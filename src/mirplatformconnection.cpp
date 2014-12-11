/* * This file is part of Maliit framework *
 *
 * Copyright (C) 2014 Dinesh Manajipet <saidinesh5@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1 as published by the Free Software Foundation
 * and appearing in the file LICENSE.LGPL included in the packaging
 * of this file.
 */

#include "mirplatformconnection.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QTimer>
#include <QFile>
#include <QDir>

namespace Maliit {

MirPlatformConnection::MirPlatformConnection(QObject *parent):
    QObject(parent),
    m_updateTimer(new QTimer(parent)),
    m_localServer(new QLocalServer(parent)),
    m_mirConnection(0)
{
    m_sharedInfo.reset();
    startLocalServer();

    m_updateTimer->setInterval(100);
    m_updateTimer->setSingleShot(true);

    connect(m_updateTimer, SIGNAL(timeout()),
            this, SLOT(sendGeometryUpdate()));
}

void MirPlatformConnection::setGeometry(const QRect &rect)
{
    if(m_sharedInfo == rect)
        return;

    m_sharedInfo.keyboardX = rect.x();
    m_sharedInfo.keyboardY = rect.y();
    m_sharedInfo.keyboardWidth = rect.width();
    m_sharedInfo.keyboardHeight = rect.height();

    sendGeometryUpdateLater();
}

const QString MirPlatformConnection::serverSocketPath()
{
    char *xdgRuntimeDir = getenv("XDG_RUNTIME_DIR");

    if (xdgRuntimeDir) {
        return QDir(xdgRuntimeDir).filePath("ubuntu-keyboard-info");
    } else {
        return QDir("/tmp").filePath("ubuntu-keyboard-info");
    }

}

void MirPlatformConnection::startLocalServer()
{
    QString socketFilePath = serverSocketPath();

    QFile socketFile(socketFilePath);
    if (socketFile.exists()) {
        // Assume it's a leftover from a previous run of maliit-server and get rid of it.
        // Because more than one instances of maliit-server isn't supported atm.
        if (!socketFile.remove()) {
            qWarning() << "Maliit::MirPlatform: unable to remove pre-existing"
                       << socketFilePath ;
        }
    }

    connect( m_localServer, SIGNAL(newConnection()),
             this, SLOT( onIncomingConnection() ) );


    if (!m_localServer->listen(socketFilePath)) {
        qWarning() << "Maliit::MirPlatform: failed to listen for connections on"
                   << socketFilePath;

    }
}

void MirPlatformConnection::onIncomingConnection()
{
    QLocalSocket *newConnection = m_localServer->nextPendingConnection();

    if (m_mirConnection) {
        qWarning() << "Maliit::MirPlatform: Refusing duplicate connection, possibly from Mir server server";;
        delete newConnection;
        return;
    }

    m_mirConnection = newConnection;
    m_sharedInfo.reset();

    connect(m_mirConnection, SIGNAL(disconnected()),
            this, SLOT(onConnectionDisconnected()) );
}

void MirPlatformConnection::onConnectionDisconnected()
{
    m_mirConnection->deleteLater();
    m_mirConnection = 0;
}

void MirPlatformConnection::sendGeometryUpdate()
{
    if (!m_mirConnection || m_mirConnection->state() != QLocalSocket::ConnectedState)
        return;

    qint64 packetSize = sizeof(SharedInfo);
    qint64 bytesWritten = m_mirConnection->write(reinterpret_cast<char *>(&m_sharedInfo), packetSize );
    if (bytesWritten < 0) {
        qWarning("Maliit::MirPlatform: Failed send geometry information to the Mir server");
    } else if (bytesWritten != packetSize) {
        //FIXME: later
        qWarning() << "Maliit::MirPlatform"
                   << QString("Only %1/%2 bytes sent to the Mir server").arg(bytesWritten).arg(packetSize);
    }
}

void MirPlatformConnection::sendGeometryUpdateLater()
{
    if(m_updateTimer->isActive())
        m_updateTimer->stop();

    m_updateTimer->start();
}

}
