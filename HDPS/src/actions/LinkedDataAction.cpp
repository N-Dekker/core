#include "LinkedDataAction.h"

#include "Set.h"

using namespace hdps;
using namespace hdps::gui;

LinkedDataAction::LinkedDataAction(QObject* parent, Dataset<DatasetImpl> dataset) :
    GroupAction(parent, false),
    _dataset(dataset),
    _mayReceiveAction(this, "Receive"),
    _maySendAction(this, "Send")
{
    setText("Linked data");

    _mayReceiveAction.setToolTip("Whether the dataset may receive linked data from other datasets");
    _maySendAction.setToolTip("Whether the dataset may send linked data to other datasets");

    connect(&_mayReceiveAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        _dataset->setLinkedDataFlag(DatasetImpl::LinkedDataFlag::Receive, toggled);
    });

    connect(&_maySendAction, &ToggleAction::toggled, this, [this](bool toggled) -> void {
        _dataset->setLinkedDataFlag(DatasetImpl::LinkedDataFlag::Send, toggled);
    });

    const auto updateActions = [this]() -> void {
        if (!_dataset.isValid())
            return;

        _mayReceiveAction.setChecked(_dataset->hasLinkedDataFlag(DatasetImpl::LinkedDataFlag::Receive));
        _maySendAction.setChecked(_dataset->hasLinkedDataFlag(DatasetImpl::LinkedDataFlag::Send));
    };

    connect(&_dataset, &Dataset<DatasetImpl>::dataChanged, this, updateActions);

    updateActions();
}
