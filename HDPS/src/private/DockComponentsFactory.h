#pragma once

#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>

#include "DockAreaTitleBar.h"

/**
 * Dock components factory class
 *
 * Extends the ADS dock components factory class to create a custom title bar.
 *
 * @author Thomas Kroes
 */
class DockComponentsFactory : public ads::CDockComponentsFactory
{
public:
    using Super = ads::CDockComponentsFactory;

    /**
     * Create custom dock area title bar
     * @param dockAreaWidget Pointer to owning dock area widget
     * @return Pointer to created dock area title bar
     */
    ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* dockAreaWidget) const override;

    //CDockAreaTabBar* createDockAreaTabBar(CDockAreaWidget* DockArea) const override {
    //    auto dockAreaTabBar = new ads::CDockAreaTabBar(DockArea);

    //    auto CustomButton = new QToolButton(DockArea);
    //    CustomButton->setToolTip(QObject::tr("Help"));

    //    
    //    return dockAreaTabBar;
    //}
};