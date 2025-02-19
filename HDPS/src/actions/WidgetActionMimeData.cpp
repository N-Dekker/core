// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetActionMimeData.h"

namespace mv::gui {

WidgetActionMimeData::WidgetActionMimeData(WidgetAction* action) :
    QMimeData(),
    _action(action),
    _actionsListModel(this),
    _actionsFilterModel(this),
    _highlightActions()
{
    _actionsFilterModel.setSourceModel(&_actionsListModel);
    _actionsFilterModel.getScopeFilterAction().setSelectedOptions({ "Private" });
    _actionsFilterModel.getTypeFilterAction().setString(getAction()->getTypeString());

    for (int rowIndex = 0; rowIndex < _actionsFilterModel.rowCount(); ++rowIndex) {
        auto action = _actionsFilterModel.getAction(rowIndex);

        if (action == getAction())
            continue;

        if (!action->isEnabled())
            continue;

        if (!action->mayConnect(WidgetAction::Gui))
            continue;

        if (action->isHighlighted())
            action->unHighlight();

        if (action->isConnected() && (action->getPublicAction() == getAction()))
            continue;

        _highlightActions << action;
    }

    for (auto highlightAction : _highlightActions)
        highlightAction->highlight();
}

WidgetActionMimeData::~WidgetActionMimeData()
{
    for (auto highlightAction : _highlightActions)
        highlightAction->unHighlight();
}

QStringList WidgetActionMimeData::formats() const
{
    return { format() };
}

}