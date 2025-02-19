// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "GlobalSettingsGroupAction.h"

namespace mv
{

using namespace gui;

GlobalSettingsGroupAction::GlobalSettingsGroupAction(QObject* parent, const QString& title, bool expanded /*= true*/) :
    GroupAction(parent, title, expanded)
{
}

QString GlobalSettingsGroupAction::getSettingsPrefix() const
{
    return QString("GlobalSettings/%1/").arg(text());
}

}
