#include "GroupAction.h"
#include "WidgetActionLabel.h"
#include "ToggleAction.h"
#include "TriggerAction.h"
#include "TriggersAction.h"

#include <QDebug>
#include <QGridLayout>

namespace hdps {

namespace gui {

const std::uint32_t GroupAction::globalLabelWidthPercentage = 35;
const std::uint32_t GroupAction::globalLabelWidthFixed      = 200;

GroupAction::GroupAction(QObject* parent, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false),
    _widgetActions(),
    _showLabels(true),
    _labelSizingType(LabelSizingType::Percentage),
    _labelWidthPercentage(GroupAction::globalLabelWidthPercentage),
    _labelWidthFixed(GroupAction::globalLabelWidthFixed)
{
}

GroupAction::GroupAction(QObject* parent, WidgetActions widgetActions, const bool& expanded /*= false*/) :
    WidgetAction(parent),
    _expanded(expanded),
    _readOnly(false),
    _widgetActions(),
    _showLabels(true),
    _labelSizingType(LabelSizingType::Percentage),
    _labelWidthPercentage(GroupAction::globalLabelWidthPercentage),
    _labelWidthFixed(GroupAction::globalLabelWidthFixed)
{
    setActions(widgetActions);
}

void GroupAction::setExpanded(const bool& expanded)
{
    if (expanded == _expanded)
        return;

    if (expanded)
        expand();
    else
        collapse();
}

void GroupAction::expand()
{
    if (_expanded == true)
        return;

    _expanded = true;

    emit expanded();
}

void GroupAction::collapse()
{
    if (_expanded == false)
        return;

    _expanded = false;

    emit collapsed();
}

void GroupAction::toggle()
{
    setExpanded(!isExpanded());
}

bool GroupAction::isExpanded() const
{
    return _expanded;
}

bool GroupAction::isCollapsed() const
{
    return !_expanded;
}

bool GroupAction::isReadOnly() const
{
    return _readOnly;
}

void GroupAction::setReadOnly(const bool& readOnly)
{
    if (readOnly == _readOnly)
        return;

    _readOnly = readOnly;

    emit readOnlyChanged(_readOnly);
}

bool GroupAction::getShowLabels() const
{
    return _showLabels;
}

void GroupAction::setShowLabels(bool showLabels)
{
    if (showLabels == _showLabels)
        return;

    _showLabels = showLabels;

    emit showLabelsChanged(_showLabels);
}

GroupAction::LabelSizingType GroupAction::getLabelSizingType() const
{
    return _labelSizingType;
}

void GroupAction::setLabelSizingType(const LabelSizingType& labelSizingType)
{
    if (labelSizingType == _labelSizingType)
        return;

    _labelSizingType = labelSizingType;

    emit labelSizingTypeChanged(_labelSizingType);
}

std::uint32_t GroupAction::getLabelWidthPercentage() const
{
    return _labelWidthPercentage;
}

void GroupAction::setLabelWidthPercentage(std::uint32_t labelWidthPercentage)
{
    if (labelWidthPercentage == _labelWidthPercentage)
        return;

    _labelSizingType        = LabelSizingType::Percentage;
    _labelWidthPercentage   = labelWidthPercentage;

    emit labelWidthPercentageChanged(_labelWidthPercentage);
}

std::uint32_t GroupAction::getLabelWidthFixed() const
{
    return _labelWidthFixed;
}

void GroupAction::setLabelWidthFixed(std::uint32_t labelWidthFixed)
{
    if (labelWidthFixed == _labelWidthFixed)
        return;

    _labelSizingType    = LabelSizingType::Fixed;
    _labelWidthFixed    = labelWidthFixed;

    emit labelWidthFixedChanged(_labelWidthFixed);
}

void GroupAction::setActions(const WidgetActions& widgetActions /*= WidgetActions()*/)
{
    _widgetActions = widgetActions;

    emit actionsChanged(_widgetActions);
}

WidgetActions GroupAction::getSortedWidgetActions() const
{
    auto sortedActions = _widgetActions;

    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (childWidgetAction == nullptr)
            continue;

        if (!childWidgetAction->isVisible())
            continue;

        sortedActions << childWidgetAction;
    }

    std::sort(sortedActions.begin(), sortedActions.end(), [](WidgetAction* lhs, WidgetAction* rhs) {
        return rhs->getSortIndex() > lhs->getSortIndex();
    });

    return sortedActions;
}

GroupAction::FormWidget::FormWidget(QWidget* parent, GroupAction* groupAction, const std::int32_t& widgetFlags) :
    WidgetActionWidget(parent, groupAction),
    _layout(new QGridLayout())
{
    auto contentsMargin = _layout->contentsMargins();

    //_layout->setContentsMargins(10, 10, 10, 10);
    
    setLayout(_layout);

    const auto reset = [this, groupAction]() -> void {
        if (groupAction->getShowLabels()) {
            switch (groupAction->getLabelSizingType())
            {
                case LabelSizingType::Auto:
                {
                    //_layout->setColumnStretch(1, 1);
                    break;
                }

                case LabelSizingType::Percentage:
                {
                    _layout->setColumnStretch(0, groupAction->getLabelWidthPercentage());
                    _layout->setColumnStretch(1, 100 - groupAction->getLabelWidthPercentage());
                    break;
                }

                default:
                    break;
            }

        }
        else {
            _layout->setColumnStretch(0, 0);
            _layout->setColumnStretch(1, 1);
        }

        QLayoutItem* layoutItem;

        while ((layoutItem = layout()->takeAt(0)) != NULL)
        {
            delete layoutItem->widget();
            delete layoutItem;
        }

        for (auto widgetAction : groupAction->getSortedWidgetActions()) {
            const auto numRows          = _layout->rowCount();
            const auto isToggleAction   = dynamic_cast<ToggleAction*>(widgetAction);
            const auto isTriggerAction  = dynamic_cast<TriggerAction*>(widgetAction);
            const auto isTriggersAction = dynamic_cast<TriggersAction*>(widgetAction);

            if (groupAction->getShowLabels() && !isToggleAction && !isTriggerAction && !isTriggersAction) {
                auto labelWidget = dynamic_cast<WidgetActionLabel*>(widgetAction->createLabelWidget(this, WidgetActionLabel::ColonAfterName));

                switch (groupAction->getLabelSizingType())
                {
                    case LabelSizingType::Auto:
                        labelWidget->setElide(false);
                        labelWidget->setFixedWidth(groupAction->getLabelWidthFixed());
                        break;

                    case LabelSizingType::Percentage:
                        labelWidget->setElide(true);
                        break;

                    case LabelSizingType::Fixed:
                        labelWidget->setElide(true);
                        labelWidget->setFixedWidth(groupAction->getLabelWidthFixed());
                        break;

                    default:
                        break;
                }

                _layout->addWidget(labelWidget, numRows, 0);
            }

            auto actionWidget = widgetAction->createWidget(this);

            _layout->addWidget(actionWidget, numRows, 1);

            if (isToggleAction)
                _layout->setAlignment(actionWidget, Qt::AlignLeft);
        }
    };

    connect(groupAction, &GroupAction::actionsChanged, this, reset);
    connect(groupAction, &GroupAction::showLabelsChanged, this, reset);
    connect(groupAction, &GroupAction::labelSizingTypeChanged, this, reset);
    connect(groupAction, &GroupAction::labelWidthPercentageChanged, this, reset);
    connect(groupAction, &GroupAction::labelWidthFixedChanged, this, reset);

    reset();
}

QGridLayout* GroupAction::FormWidget::layout()
{
    return _layout;
}

}
}
