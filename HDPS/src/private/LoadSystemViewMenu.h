#pragma once

#include <util/DockArea.h>

#include <actions/TriggerAction.h>

#include <DockAreaWidget.h>

#include <QMenu>

/**
 * Load system menu class
 * 
 * Menu class for loading system view plugins
 * 
 * @author Thomas Kroes
 */
class LoadSystemViewMenu : public QMenu
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent widget
     * @param dockAreaWidget Pointer to dock area widget to which new view plugins are docked (new view plugins will be docked top-level if nullptr)
     */
    LoadSystemViewMenu(QWidget *parent = nullptr, ads::CDockAreaWidget* dockAreaWidget = nullptr);

    /**
     * Get whether the menu has any enabled menu items that produce a plugin
     * return Boolean determining whether the menu has any enabled menu items that produce a plugin
     */
    bool mayProducePlugins() const;

private:

    /**
     * Get actions for loading system views
     * @param dockArea Dock area to dock to (if set to none, the view plugin preferred dock area is used)
     * @return Vector of actions
     */
    QVector<QPointer<hdps::gui::TriggerAction>> getLoadSystemViewsActions(hdps::gui::DockAreaFlag dockArea = hdps::gui::DockAreaFlag::None);

private:
    ads::CDockAreaWidget*   _dockAreaWidget;      /** Pointer to dock area widget to which new view plugins are docked (new view plugins will be docked top-level if nullptr) */
};
