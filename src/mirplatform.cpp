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

#include "mirplatform.h"
#include "mirplatformconnection.h"

#include <QRegion>
#include <QWindow>

namespace Maliit {

MirPlatform::MirPlatform():
    AbstractPlatform(),
    m_mirPlatformConnection(new MirPlatformConnection)
{
}

void MirPlatform::setupInputPanel(QWindow *window, Maliit::Position position)
{
    Q_UNUSED(window)
    Q_UNUSED(position)
}

void MirPlatform::setInputRegion(QWindow *window, const QRegion &region)
{
    Q_UNUSED(window)

    m_mirPlatformConnection->setGeometry(region.rects().first());
}

}
