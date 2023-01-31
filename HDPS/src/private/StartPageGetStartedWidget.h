#pragma once

#include "StartPageActionsWidget.h"

#include <actions/OptionAction.h>
#include <actions/RecentFilesAction.h>

#include <QWidget>
#include <QStandardItemModel>

/**
 * Start page get started widget class
 *
 * Widget class which contains actions related to getting started.
 *
 * @author Thomas Kroes
 */
class StartPageGetStartedWidget : public QWidget
{
public:

    /** Ways to create a new project from workspace */
    enum class FromWorkspaceType {
        BuiltIn,            /** Create project from built-in workspace */
        RecentWorkspace,    /** Create project from recent workspace */
        RecentProject       /** Replicate workspace from recent project in new project */
    };

protected:

    /**
     * Construct with \p parent widget
     * @param parent Pointer to parent widget
     */
    StartPageGetStartedWidget(QWidget* parent = nullptr);

    /** Updates the actions to reflect changes */
    void updateActions();

private:

    /** Update actions for creating a project from workspace */
    void updateCreateProjectFromWorkspaceActions();

    /** Update actions for creating a project from dataset */
    void updateCreateProjectFromDatasetActions();

private:
    StartPageActionsWidget          _createProjectFromWorkspaceWidget;  /** Actions widget for creating a project from workspace */
    StartPageActionsWidget          _createProjectFromDatasetWidget;    /** Actions widget for creating a project from a dataset */
    StartPageActionsWidget          _instructionVideosWidget;           /** Actions widget for watching instruction videos */
    hdps::gui::OptionAction         _workspaceLocationTypeAction;       /** Action for filtering workspace location types (create project from built-in workspace or import from project) */
    QStandardItemModel              _workspaceLocationTypesModel;       /** Input model for the above workspace location type action */
    hdps::gui::RecentFilesAction    _recentWorkspacesAction;            /** Action for recent workspaces (create project from recent workspace) */
    hdps::gui::RecentFilesAction    _recentProjectsAction;              /** Action for recent projects (replicate workspace from recent project) */

    friend class StartPageContentWidget;
};