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

#ifndef MALIIT_MIR_PLATFORM_CONNECTION_H
#define MALIIT_MIR_PLATFORM_CONNECTION_H

#include <QObject>
#include <QRect>

class QTimer;
class QLocalServer;
class QLocalSocket;

namespace Maliit {

// This class simply starts a server and listens to incoming connections on a socket,
// mainly to inform the Mir server about the keyboard geometry
class MirPlatformConnection: QObject {
    Q_OBJECT
public:
    MirPlatformConnection(QObject* parent = 0);

    void setGeometry(const QRect& rect);

private:
    static const QString serverSocketPath();
    void startLocalServer();

private Q_SLOTS:
    void onIncomingConnection();
    void onConnectionDisconnected();
    void sendGeometryUpdate();
    void sendGeometryUpdateLater();

private:
  
  // NB! Must match thep definition in unity-mir. Not worth creating a shared header
  // just for that.
  typedef struct{
      qint32 keyboardX;
      qint32 keyboardY;
      qint32 keyboardWidth;
      qint32 keyboardHeight;

      bool operator== (const QRect& rect){
          return keyboardX == rect.x() && keyboardY == rect.y() &&
                  keyboardWidth == rect.width() && keyboardHeight == rect.height();
      }

      void reset()
      {
          keyboardX = -1;
          keyboardY = -1;
          keyboardWidth = 0;
          keyboardHeight = 0;
      }
  } SharedInfo;
  
    SharedInfo m_sharedInfo;

    QTimer* m_updateTimer;
    QLocalServer* m_localServer;
    QLocalSocket* m_mirConnection;
};


}

#endif //MALIIT_MIR_PLATFORM_CONNECTION_H
