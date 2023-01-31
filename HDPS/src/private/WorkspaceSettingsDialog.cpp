#include "WorkspaceSettingsDialog.h"

#include <AbstractWorkspaceManager.h>

#include <Application.h>
#include <CoreInterface.h>

#include <QVBoxLayout>

#ifdef _DEBUG
    #define WORKSPACE_SETTINGS_DIALOG_VERBOSE
#endif

using namespace hdps;
using namespace hdps::gui;

WorkspaceSettingsDialog::WorkspaceSettingsDialog(QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _groupAction(this),
    _okAction(this, "Ok")
{
    setWindowIcon(Application::getIconFont("FontAwesome").getIcon("cog"));
    setModal(true);
    setWindowTitle("Workspace Settings");

    _groupAction.setLabelSizingType(GroupAction::LabelSizingType::Auto);

    const auto workspace = workspaces().getCurrentWorkspace();

    _groupAction << workspace->getTitleAction();
    _groupAction << workspace->getDescriptionAction();
    _groupAction << workspace->getTagsAction();
    _groupAction << workspace->getCommentsAction();
    _groupAction << workspaces().getLockingAction().getLockedAction();

    auto layout = new QVBoxLayout();

    auto groupActionWidget = _groupAction.createWidget(this);

    groupActionWidget->layout()->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(groupActionWidget);

    auto bottomLayout = new QHBoxLayout();

    bottomLayout->addStretch(1);
    bottomLayout->addWidget(_okAction.createWidget(this));

    layout->addLayout(bottomLayout);

    setLayout(layout);

    connect(&_okAction, &TriggerAction::triggered, this, &WorkspaceSettingsDialog::accept);
}
