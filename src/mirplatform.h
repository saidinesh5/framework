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

#ifndef MIRPLATFORM_H
#define MIRPLATFORM_H

#include "abstractplatform.h"
#include <QScopedPointer>

namespace Maliit
{

class MirPlatformConnection;

class MirPlatform : public AbstractPlatform
{

public:
    explicit MirPlatform();

    virtual void setupInputPanel(QWindow* window,
                                 Maliit::Position position);
    virtual void setInputRegion(QWindow* window,
                                const QRegion& region);

private:
    QScopedPointer<MirPlatformConnection> m_mirPlatformConnection;

};

}

#endif // MIRPLATFORM_H
