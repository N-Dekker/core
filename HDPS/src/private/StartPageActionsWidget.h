// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StartPageActionsModel.h"
#include "StartPageActionsFilterModel.h"

#include <widgets/HierarchyWidget.h>

#include <QWidget>

/**
 * Start page actions widget class
 *
 * Widget class for listing start page actions.
 *
 * @author Thomas Kroes
 */
class StartPageActionsWidget : public QWidget
{
public:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     * @param title Title shown in the header
     * @param restyle Whether to change the style to seamlessly integrate with the start page (false for use outside of startpage widget)
     */
    StartPageActionsWidget(QWidget* parent = nullptr, const QString& title = "", bool restyle = true);

    /**
     * Get layout
     * @return Reference to main layout
     */
    QVBoxLayout& getLayout();

    /**
     * Get model
     * @return Reference to model
     */
    StartPageActionsModel& getModel();

    /**
     * Get filter model
     * @return Reference to filter model
     */
    StartPageActionsFilterModel& getFilterModel();

    /**
     * Get hierarchy widget
     * @return Reference to hierarchy widget
     */
    hdps::gui::HierarchyWidget& getHierarchyWidget();

private:

    /**
     * Open persistent editor for \p rowIndex
     * @param rowIndex Index of the row for which to open the persistent editor
     */
    void openPersistentEditor(int rowIndex);

    /**
     * Close persistent editor for \p rowIndex
     * @param rowIndex Index of the row for which to close the persistent editor
     */
    void closePersistentEditor(int rowIndex);
    
    /** Update all  custom style elements */
    void updateCustomStyle();

private:
    QVBoxLayout                     _layout;            /** Main layout */
    StartPageActionsModel           _model;             /** Model which contains start page actions */
    StartPageActionsFilterModel     _filterModel;       /** Model for filtering and sorting start page actions */
    hdps::gui::HierarchyWidget      _hierarchyWidget;   /** Widget for displaying the actions */
    bool                            _restyle;           /** Remember whether the restyle flag was set upon creation */
};
