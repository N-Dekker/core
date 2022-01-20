#include "StringAction.h"

#include <QHBoxLayout>

namespace hdps {

namespace gui {

    

StringAction::StringAction(QObject* parent, const QString& title /*= ""*/, const QString& string /*= ""*/, const QString& defaultString /*= ""*/) :
    WidgetAction(parent)
{
    setText(title);
    setMayReset(true);
    setDefaultWidgetFlags(WidgetFlag::Basic);
    initialize(string, defaultString);
}

void StringAction::initialize(const QString& string /*= ""*/, const QString& defaultString /*= ""*/)
{
    setString(string);
    setDefaultString(defaultString);

    setResettable(isResettable());
}

QString StringAction::getString() const
{
    return _string;
}

void StringAction::setString(const QString& string)
{
    if (string == _string)
        return;

    _string = string;

    emit stringChanged(_string);

    setResettable(isResettable());
}

QString StringAction::getDefaultString() const
{
    return _defaultString;
}

void StringAction::setDefaultString(const QString& defaultString)
{
    if (defaultString == _defaultString)
        return;

    _defaultString = defaultString;

    emit defaultStringChanged(_defaultString);

    setResettable(isResettable());
}

bool StringAction::isResettable() const
{
    return _string != _defaultString;
}

void StringAction::reset()
{
    setString(_defaultString);
}

QString StringAction::getPlaceholderString() const
{
    return _placeholderString;
}

void StringAction::setPlaceHolderString(const QString& placeholderString)
{
    if (placeholderString == _placeholderString)
        return;

    _placeholderString = placeholderString;

    emit placeholderStringChanged(_placeholderString);
}

StringAction::LineEditWidget::LineEditWidget(QWidget* parent, StringAction* stringAction) :
    QLineEdit(parent)
{
    setObjectName("LineEdit");
    setAcceptDrops(true);

    // Update the line edit text from the string action
    const auto updateLineEdit = [this, stringAction]() {
        QSignalBlocker blocker(this);

        setText(stringAction->getString());
    };

    // Update the place holder text in the line edit
    const auto updatePlaceHolderText = [this, stringAction]() -> void {
        setPlaceholderText(stringAction->getPlaceholderString());
    };

    // Update the line edit string when the action string changes
    connect(stringAction, &StringAction::stringChanged, this, updateLineEdit);

    // Update the line edit placeholder string when the action placeholder string changes
    connect(stringAction, &StringAction::placeholderStringChanged, this, updatePlaceHolderText);

    // Update the action string when the line edit text changes
    connect(this, &QLineEdit::textChanged, this, [this, stringAction](const QString& text) {
        stringAction->setString(text);
    });

    // Perform initial updates
    updateLineEdit();
    updatePlaceHolderText();
}

QWidget* StringAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    auto widget = new WidgetActionWidget(parent, this);
    auto layout = new QHBoxLayout();

    layout->setMargin(0);
    layout->setSpacing(3);

    if (widgetFlags & WidgetFlag::LineEdit)
        layout->addWidget(new StringAction::LineEditWidget(parent, this));

    if (widgetFlags & WidgetFlag::ResetPushButton)
        layout->addWidget(createResetButton(parent));

    widget->setLayout(layout);

    return widget;
}

}
}
