#pragma once

#include "WidgetAction.h"

#include <QPushButton>

namespace hdps {

namespace gui {

/**
 * Trigger action class
 *
 * Action class for trigger
 *
 * @author Thomas Kroes
 */
class TriggerAction : public WidgetAction
{
    Q_OBJECT

public:

    /** Describes the widget flags */
    enum WidgetFlag {

        /** Push button options */
        Icon            = 0x00001,          /** Enable push button icon */
        Text            = 0x00002,          /** Enable push button text */

        /** Push button configurations */
        IconText = Icon | Text              /** Push button with icon and text */
    };

public:

    /** Push button widget class for trigger action */
    class PushButtonWidget : public QPushButton {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param triggerAction Pointer to trigger action
         * @param widgetFlags Widget flags
         */
        PushButtonWidget(QWidget* parent, TriggerAction* triggerAction, const std::int32_t& widgetFlags);

    protected:
        TriggerAction*   _triggersAction;      /** Pointer to toggle action */

        friend class TriggerAction;
    };

public:

    /**
     * Get type string
     * @return Widget action type in string format
     */
    QString getTypeString() const override;

protected:

    /**
     * Get widget representation of the trigger action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override;

public: // Linking

    /**
     * Get whether the action may be published or not
     * @return Boolean indicating whether the action may be published or not
     */
    bool mayPublish() const override;

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

protected:  // Linking

    /**
     * Get public copy of the action (other compatible actions can connect to it)
     * @return Pointer to public copy of the action
     */
    virtual WidgetAction* getPublicCopy() const override;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    TriggerAction(QObject* parent, const QString& title = "");
};

}
}
