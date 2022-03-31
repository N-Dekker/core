#include "ToggleAction.h"

#include <QCheckBox>
#include <QPushButton>
#include <QMenu>
#include <QHBoxLayout>

namespace hdps {

namespace gui {

ToggleAction::ToggleAction(QObject* parent, const QString& title /*= ""*/, const bool& toggled /*= false*/, const bool& defaultToggled /*= false*/) :
    WidgetAction(parent),
    _defaultToggled(defaultToggled)
{
    setCheckable(true);
    setText(title);
    setDefaultWidgetFlags(WidgetFlag::Default);
    initialize(toggled, defaultToggled);
}

void ToggleAction::initialize(const bool& toggled /*= false*/, const bool& defaultToggled /*= false*/)
{
    setChecked(toggled);
    setDefaultToggled(defaultToggled);
}

bool ToggleAction::getDefaultToggled() const
{
    return _defaultToggled;
}

void ToggleAction::setDefaultToggled(const bool& defaultToggled)
{
    if (defaultToggled == _defaultToggled)
        return;

    _defaultToggled = defaultToggled;

    emit defaultToggledChanged(_defaultToggled);
}

bool ToggleAction::mayPublish() const
{
    return true;
}

void ToggleAction::connectToPublicAction(WidgetAction* publicAction)
{
    auto publicToggleAction = dynamic_cast<ToggleAction*>(publicAction);

    Q_ASSERT(publicToggleAction != nullptr);

    connect(this, &ToggleAction::toggled, publicToggleAction, &ToggleAction::setChecked);
    connect(publicToggleAction, &ToggleAction::toggled, this, &ToggleAction::setChecked);

    WidgetAction::connectToPublicAction(publicAction);
}

void ToggleAction::disconnectFromPublicAction()
{
    auto publicToggleAction = dynamic_cast<ToggleAction*>(_publicAction);

    Q_ASSERT(publicToggleAction != nullptr);

    disconnect(this, &ToggleAction::toggled, publicToggleAction, &ToggleAction::setChecked);
    disconnect(publicToggleAction, &ToggleAction::toggled, this, &ToggleAction::setChecked);

    WidgetAction::disconnectFromPublicAction();
}

WidgetAction* ToggleAction::getPublicCopy() const
{
    return new ToggleAction(parent(), text(), isChecked(), _defaultToggled);
}

ToggleAction::CheckBoxWidget::CheckBoxWidget(QWidget* parent, ToggleAction* toggleAction) :
    QCheckBox(parent),
    _toggleAction(toggleAction)
{
    setAcceptDrops(true);

    auto layout = new QHBoxLayout();

    layout->setMargin(0);

    connect(this, &QCheckBox::toggled, this, [this](bool toggled) {
        _toggleAction->setChecked(toggled);
    });

    const auto update = [this]() -> void {
        QSignalBlocker blocker(this);

        setEnabled(_toggleAction->isEnabled());
        setText(_toggleAction->text());
        setToolTip(_toggleAction->toolTip());
        setVisible(_toggleAction->isVisible());
    };

    connect(toggleAction, &ToggleAction::changed, this, [this, update]() {
        update();
    });

    const auto updateToggle = [this]() -> void {
        QSignalBlocker blocker(this);

        setChecked(_toggleAction->isChecked());
    };

    connect(_toggleAction, &ToggleAction::toggled, this, [this, updateToggle]() {
        updateToggle();
    });

    update();
    updateToggle();
}

ToggleAction::PushButtonWidget::PushButtonWidget(QWidget* parent, ToggleAction* toggleAction, const std::int32_t& widgetFlags) :
    QPushButton(parent),
    _toggleAction(toggleAction)
{
    setCheckable(true);

    connect(this, &QPushButton::toggled, this, [this](bool toggled) {
        _toggleAction->setChecked(toggled);
    });

    const auto update = [this, widgetFlags]() -> void {
        QSignalBlocker blocker(this);

        if (isEnabled() != _toggleAction->isEnabled())
            setEnabled(_toggleAction->isEnabled());

        if (isChecked() != _toggleAction->isChecked())
            setChecked(_toggleAction->isChecked());

        if (toolTip() != _toggleAction->toolTip())
            setToolTip(_toggleAction->toolTip());

        if (isVisible() != _toggleAction->isVisible())
            setVisible(_toggleAction->isVisible());

        if (widgetFlags & WidgetFlag::Text && text() != _toggleAction->text())
            setText(_toggleAction->text());

        if (widgetFlags & WidgetFlag::Icon) {
            setIcon(_toggleAction->icon());
            setProperty("class", "square-button");
        }
    };

    connect(_toggleAction, &QAction::changed, this, update);

    update();
}

QWidget* ToggleAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    if (dynamic_cast<QMenu*>(parent))
        return QWidgetAction::createWidget(parent);

    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::CheckBox)
        layout->addWidget(new ToggleAction::CheckBoxWidget(parent, this));

    if (widgetFlags & WidgetFlag::PushButton)
        layout->addWidget(new ToggleAction::PushButtonWidget(parent, this, widgetFlags));

    widget->setLayout(layout);

    return widget;
}

}
}
