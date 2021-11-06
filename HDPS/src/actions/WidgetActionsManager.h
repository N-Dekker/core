#ifndef HDPS_WIDGET_ACTIONS_MANAGER_H
#define HDPS_WIDGET_ACTIONS_MANAGER_H

#include "actions/WidgetAction.h"

namespace hdps {

namespace gui {

/**
 * Widget actions manager class
 *
 * @author Thomas Kroes
 */
class WidgetActionsManager
{

public:

    /**
     * Add a widget action
     * This class does not alter the ownership of the allocated widget action
     * @param widgetAction Widget action to expose
     */
    void addAction(gui::WidgetAction* widgetAction);

    /**
     * Removes an action widget
     * This does not de-allocate the widget action memory
     * @param widgetAction Widget action to remove
     */
    void removeAction(gui::WidgetAction* widgetAction);

    /**
     * Gets widget actions by context
     * @param context Context
     * @return Widget actions
     */
    WidgetActions getActionsByContext(const QString& context) const;

    /**
     * Get widget action by name
     * @param name Name of the action
     * @return Pointer to action (if any)
     */
    gui::WidgetAction* getActionByName(const QString& name);

    /**
     * Populates existing menu with exposed actions menus
     * @param contextMenu Context menu to populate
     */
    void populateContextMenu(const QString& context, QMenu* contextMenu);

    /**
     * Gets context menu for the exposed actions
     * @param parent Parent widget
     * @param context Context
     * @return Context menu
     */
    QMenu* getContextMenu(QWidget* parent, const QString& context);

public:
    
    /**
     * Informs the manager that a widget started dragging (called by the widget action)
     */
    void widgetStartedDragging(WidgetAction* widgetAction);

    /**
     * Informs the manager that a widget started dragging (called by the widget action)
     */
    void widgetStoppedDragging(WidgetAction* widgetAction);

protected:
    WidgetActions    _actions;     /** Exposed widget actions */

    friend class WidgetAction;
};

}
}

#endif // HDPS_WIDGET_ACTIONS_MANAGER_H