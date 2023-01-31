#pragma once

#include "StartPageActionsWidget.h"

#include <actions/RecentFilesAction.h>

#include <QWidget>

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
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageOpenProjectWidget(QWidget* parent = nullptr);

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

private:
    StartPageActionsWidget          _openCreateProjectWidget;   /** Actions widget for open and create project action */
    StartPageActionsWidget          _recentProjectsWidget;      /** Actions widget for existing projects action */
    StartPageActionsWidget          _exampleProjectsWidget;     /** Actions widget for example projects action */
    hdps::gui::RecentFilesAction    _recentProjectsAction;      /** Action for recent projects */
    QIcon                           _leftAlignedIcon;           /** Icon for left-aligned default project */
    QIcon                           _leftAlignedLoggingIcon;    /** Icon for left-aligned default project with logging */
    QIcon                           _rightAlignedIcon;          /** Icon for right-aligned default project */
    QIcon                           _rightAlignedLoggingIcon;   /** Icon for right-aligned default project with logging */

    friend class StartPageContentWidget;
};