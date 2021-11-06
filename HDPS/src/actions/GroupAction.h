#pragma once

#include "WidgetAction.h"

class QWidget;
class QGridLayout;

namespace hdps {

namespace gui {

/**
 * Group action class
 *
 * Groups multiple actions
 * When added to a dataset, the widget is added to the data properties widget as a section
 * 
 * @author Thomas Kroes
 */
class GroupAction : public WidgetAction
{
    Q_OBJECT

public:

    /**
     * Group widget class for widget action group
     */
    class FormWidget : public WidgetActionWidget
    {
    protected:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param groupAction Pointer to group action
         */
        FormWidget(QWidget* parent, GroupAction* groupAction);

        /** Get grid layout */
        QGridLayout* layout();

    protected:
        QGridLayout*    _layout;        /** Main grid layout */

        friend class GroupAction;
    };

    /**
     * Get widget representation of the group action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new FormWidget(parent, this);
    };

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param expanded Whether the group is initially expanded/collapsed
     */
    GroupAction(QObject* parent, const bool& expanded = false);

    /** Set expanded/collapsed */
    void setExpanded(const bool& expanded);

    /** Expand the group */
    void expand();

    /** Collapse the group */
    void collapse();

    /** Toggle the group */
    void toggle();

    /** Get whether the group is expanded */
    bool isExpanded() const;

    /** Get whether the group is collapsed */
    bool isCollapsed() const;

    /** Gets the group read-only */
    bool isReadOnly() const;

    /**
     * Sets the group read-only
     * @param readOnly Whether the group is read-only
     */
    void setReadOnly(const bool& readOnly);

    /**
     * Add widget action using stream in operator
     * @param widgetAction Reference to widget action
     */
    void operator << (WidgetAction& widgetAction)
    {
        _widgetActions << &widgetAction;
    }

    /** Get sorted widget actions */
    QVector<WidgetAction*> getSortedWidgetActions();

signals:

    /** Signals that the group got expanded */
    void expanded();

    /** Signals that the group got collapsed */
    void collapsed();

    /** Signals that the group read-only status changed */
    void readOnlyChanged(const bool& readOnly);

protected:
    bool                        _expanded;          /** Whether or not the group is expanded */
    bool                        _readOnly;          /** Whether or not the group is read-only */
    QVector<WidgetAction*>      _widgetActions;     /** Widget actions */
};

}
}
