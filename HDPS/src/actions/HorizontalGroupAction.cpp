// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "HorizontalGroupAction.h"

namespace hdps::gui {

HorizontalGroupAction::HorizontalGroupAction(QObject* parent, const QString& title, const bool& expanded /*= false*/) :
    GroupAction(parent, title, expanded)
{
    setDefaultWidgetFlags(GroupAction::Horizontal);
}

}