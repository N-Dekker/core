// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "Miscellaneous.h"

#include <QStringList>
#include <QAction>

using namespace std;

namespace mv::util
{

QString getIntegerCountHumanReadable(const float& count)
{
    if (count >= 0 && count < 1000)
        return QString::number(count);

    if (count >= 1000 && count < 1000000)
        return QString("%1 k").arg(QString::number(count / 1000.0f, 'f', 1));

    if (count >= 1000000)
        return QString("%1 mln").arg(QString::number(count / 1000000.0f, 'f', 1));

    return "";
}

QString getNoBytesHumanReadable(float noBytes)
{
    QStringList list{ "KB", "MB", "GB", "TB" };

    QStringListIterator it(list);
    QString unit("bytes");

    while (noBytes >= 1024.0 && it.hasNext())
    {
        unit = it.next();
        noBytes /= 1024.0;
    }

    return QString::number(noBytes, 'f', 2) + " " + unit;
}

}
