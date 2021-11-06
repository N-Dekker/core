#include "SelectedIndicesAction.h"

#include <QGridLayout>
#include <QListView>

#include <QSet>

using namespace hdps;
using namespace hdps::gui;

SelectedIndicesAction::SelectedIndicesAction(QObject* parent, hdps::CoreInterface* core, const QString& datasetName) :
    WidgetAction(parent),
    EventListener(),
    _core(core),
    _points(datasetName),
    _updateAction(this, "Update"),
    _manualUpdateAction(this, "Manual update"),
    _selectionChangedTimer(),
    _selectedIndices()
{
    setText("Selected indices");
    setEventCore(_core);
    
    _selectionChangedTimer.setSingleShot(true);

    registerDataEventByType(PointType, [this](hdps::DataEvent* dataEvent) {
        if (!_points.isValid())
            return;

        if (dataEvent->dataSetName != _points.getSourceData().getName())
            return;

        switch (dataEvent->getType()) {
            case EventType::DataAdded:
            case EventType::DataChanged:
            case EventType::SelectionChanged:
                _selectionChangedTimer.start(100);
                break;
            
            default:
                break;
        }
    });

    connect(&_selectionChangedTimer, &QTimer::timeout, this, [this]() {
        if (_manualUpdateAction.isChecked())
            return;

        if (!_points.isValid())
            return;

        auto& selection = dynamic_cast<Points&>(_points->getSelection());

        if (_points->isFull()) {
            _selectedIndices = selection.indices;
        }
        else {
            _selectedIndices.clear();
            _selectedIndices.reserve(_points->indices.size());

            QSet<std::uint32_t> indicesSet(_points->indices.begin(), _points->indices.end());

            // Add selection indices if they belong to the subset
            for (const auto& selectionIndex : selection.indices)
                if (indicesSet.contains(selectionIndex))
                    _selectedIndices.push_back(selectionIndex);
        }

        emit selectedIndicesChanged(getSelectedIndices());
    });

    const auto updateUpdateAction = [this]() -> void {
        _updateAction.setEnabled(_manualUpdateAction.isChecked());
    };

    connect(&_manualUpdateAction, &ToggleAction::toggled, this, [this, updateUpdateAction]() {
        updateUpdateAction();
    });

    connect(&_updateAction, &TriggerAction::triggered, this, [this]() {
        emit selectedIndicesChanged(getSelectedIndices());
    });

    updateUpdateAction();
}

const std::vector<std::uint32_t>& SelectedIndicesAction::getSelectedIndices() const
{
    return _selectedIndices;
}

SelectedIndicesAction::Widget::Widget(QWidget* parent, SelectedIndicesAction* selectedIndicesAction) :
    WidgetActionWidget(parent, selectedIndicesAction)
{
    auto selectedIndicesListWidget = new QListView();

    selectedIndicesListWidget->setFixedHeight(100);

    auto selectedIndicesLayout = new QHBoxLayout();

    selectedIndicesLayout->setMargin(0);
    selectedIndicesLayout->addWidget(selectedIndicesListWidget);

    auto updateLayout = new QVBoxLayout();

    updateLayout->addWidget(selectedIndicesAction->getUpdateAction().createWidget(this));
    updateLayout->addWidget(selectedIndicesAction->getManualUpdateAction().createWidget(this));
    updateLayout->addStretch(1);

    selectedIndicesLayout->addLayout(updateLayout);

    setLayout(selectedIndicesLayout);

    const auto updateSelectedIndicesWidget = [this, selectedIndicesAction, selectedIndicesListWidget]() -> void {
        QStringList items;

        for (auto selectedIndex : selectedIndicesAction->getSelectedIndices())
            items << QString::number(selectedIndex);

        selectedIndicesListWidget->setModel(new QStringListModel(items));
    };

    connect(selectedIndicesAction, &SelectedIndicesAction::selectedIndicesChanged, this, [this, updateSelectedIndicesWidget]() {
        updateSelectedIndicesWidget();
    });

    updateSelectedIndicesWidget();
}
