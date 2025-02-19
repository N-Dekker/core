// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "actions/StringAction.h"
#include "actions/OptionsAction.h"
#include "actions/ToggleAction.h"
#include "actions/StringAction.h"

#include "AbstractTasksModel.h"

#include <QSortFilterProxyModel>
#include <QMap>

namespace mv
{

/**
 * Tasks filter class
 *
 * Proxy model for filtering tasks
 *
 * @author Thomas Kroes
 */
class TasksFilterModel final : public QSortFilterProxyModel
{
    Q_OBJECT

public:

    /** Construct with parent \p parent object
     * @param parent Pointer to parent object
    */
    TasksFilterModel(QObject* parent = nullptr);

    /**
     * Returns whether \p row with \p parent is filtered out (false) or in (true)
     * @param row Row index
     * @param parent Parent index
     * @return Boolean indicating whether the item is filtered in or out
     */
    bool filterAcceptsRow(int row, const QModelIndex& parent) const override;

    /**
     * Compares two cluster items
     * @param lhs Left cluster
     * @param rhs Right cluster
     */
    bool lessThan(const QModelIndex& lhs, const QModelIndex& rhs) const override;

private:

    /**
     * Get source variant data for \p index, \p column and data \p role
     * @param index Source model index
     * @param column Source model column
     * @param role Data role
     * @return Data in variant form
     */
    QVariant getSourceData(const QModelIndex& index, const AbstractTasksModel::Column& column, int role) const;

    /**
     * Extract task types from row range and add them to the task types action when they do not exist yet
     * @param parent Parent row index
     * @param first Index of the first row
     * @param last Index of the last row
     */
    void addTaskTypesForRows(const QModelIndex& parent, int first, int last);

signals:
    
    /**
     * Signals that the number of rows changed from \p previousRowCount to \p currentRowCount
     * @param parent Parent row index of which the number of rows changed
     * @param previousRowCount Previous number of rows
     * @param currentRowCount Current number of rows
     */
    void rowCountChanged(const QModelIndex& parent, std::int32_t previousRowCount, std::int32_t currentRowCount);

public: // Action getters

    gui::OptionsAction& getTaskTypeFilterAction() { return _taskTypeFilterAction; }
    gui::OptionsAction& getTaskScopeFilterAction() { return _taskGuiScopeFilterAction; }
    gui::OptionsAction& getTaskStatusFilterAction() { return _taskStatusFilterAction; }
    gui::ToggleAction& getTopLevelTasksOnlyAction() { return _topLevelTasksOnlyAction; }
    gui::ToggleAction& getHideDisabledTasksFilterAction() { return _hideDisabledTasksFilterAction; }
    gui::ToggleAction& getHideHiddenTasksFilterAction() { return _hideHiddenTasksFilterAction; }
    gui::StringAction& getParentTaskFilterAction() { return _parentTaskFilterAction; }

private:
    gui::OptionsAction              _taskTypeFilterAction;              /** Action for filtering based on task type */
    gui::OptionsAction              _taskGuiScopeFilterAction;          /** Action for filtering based on task scope */
    gui::OptionsAction              _taskStatusFilterAction;            /** Action for filtering based on task status */
    gui::ToggleAction               _hideDisabledTasksFilterAction;     /** Action for hiding disabled tasks */
    gui::ToggleAction               _topLevelTasksOnlyAction;           /** Action for toggling top-level tasks only */
    gui::ToggleAction               _hideHiddenTasksFilterAction;       /** Action for hiding hidden tasks */
    gui::StringAction               _parentTaskFilterAction;            /** Action for filtering on parent task */
    QMap<QString, std::uint32_t>    _statusTypeCounts;                  /** Counts the number of task per status type */
    std::uint32_t                   _rowCount;                          /** Current number of rows */
};

}
