// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "StartPageActionsWidget.h"

#include <actions/RecentFilesAction.h>

#include <QWidget>

class StartPageContentWidget;

/**
 * Start page open project widget class
 *
 * Widget class which contains actions related to opening existing projects.
 *
 * @author Thomas Kroes
 */
class StartPageOpenProjectWidget : public QWidget
{
protected:

    /**
     * Construct with \p startPageContentWidget
     * @param startPageContentWidget Pointer to start page content widget
     */
    StartPageOpenProjectWidget(StartPageContentWidget* startPageContentWidget);

    /** Updates the actions to reflect changes */
    void updateActions();

private:

    /**
     * Create icon for default project with \p default view alignment and set \p icon by reference
     * @param alignment Alignment of the project
     * @param icon Icon that is set by reference
     * @param logging Whether to show the logging view plugin at the bottom
     */
    void createIconForDefaultProject(const Qt::Alignment& alignment, QIcon& icon, bool logging = false);

    /** Update actions for opening and creating projects */
    void updateOpenCreateActions();

    /** Update actions for opening recent projects */
    void updateRecentActions();

    /** Update actions for opening example projects */
    void updateExamplesActions();
    
    /** Create the custom drawn icons  */
    void createCustomIcons();
    
    /** Update all  custom style elements */
    void updateCustomStyle();

private:
    StartPageContentWidget*         _startPageContentWidget;    /** Non-owning pointer to start page content widget */
    StartPageActionsWidget          _openCreateProjectWidget;   /** Actions widget for open and create project action */
    StartPageActionsWidget          _recentProjectsWidget;      /** Actions widget for existing projects action */
    StartPageActionsWidget          _exampleProjectsWidget;     /** Actions widget for example projects action */
    mv::gui::RecentFilesAction    _recentProjectsAction;      /** Action for recent projects */
    QIcon                           _leftAlignedIcon;           /** Icon for left-aligned default project */
    QIcon                           _leftAlignedLoggingIcon;    /** Icon for left-aligned default project with logging */
    QIcon                           _rightAlignedIcon;          /** Icon for right-aligned default project */
    QIcon                           _rightAlignedLoggingIcon;   /** Icon for right-aligned default project with logging */

    friend class StartPageContentWidget;
};
