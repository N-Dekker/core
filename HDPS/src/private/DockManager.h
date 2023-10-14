// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "CentralDockWidget.h"
#include "ViewPluginDockWidget.h"

#include <ViewPlugin.h>
#include <Task.h>

#include <util/Serializable.h>

#include <DockManager.h>
#include <DockAreaWidget.h>

#include <QString>
#include <QWidget>


/**
 * Dock manager class
 *
 * ADS inherited dock manager class, primary purpose it to support layout serialization
 *
 * inherits ADS dock manager as protected so add/remove of dock widgets has to pass through this class.
 *
 * @author Thomas Kroes
 */
class DockManager : protected ads::CDockManager, public mv::util::Serializable
{
    Q_OBJECT

public:

    using CDockManager::objectName;
    using CDockManager::setObjectName;
    using CDockManager::setCentralWidget;
    using CDockManager::dockWidgetsMap;
    using CDockManager::grab;
    using CDockManager::setStyleSheet;
    
    
    /**
     * Constructs a dock manager derived from the advanced docking system
     * @param name Name of the dock manager
     * @param parent Pointer to parent widget
     */
    DockManager(const QString& name, QWidget* parent = nullptr);

    /** Destructor */
    ~DockManager();

    /**
     * Get view plugin dock widgets
     * @return Vector of pointers to plugin dock widgets
     */
    ViewPluginDockWidgets getViewPluginDockWidgets();

    /**
     * Get view plugin dock widgets
     * @return Vector of pointers to plugin dock widgets
     */
    const ViewPluginDockWidgets getViewPluginDockWidgets() const;

    /**
     * Find the dock area widget where the widget of \p viewPlugin resides
     * @param viewPlugin Pointer to view plugin that holds the widget
     * @return Pointer to ADS dock widget area (if found, otherwise nullptr)
     */
    ads::CDockAreaWidget* findDockAreaWidget(mv::plugin::ViewPlugin* viewPlugin);

    /**
     * Remove \p viewPlugin from the dock manager
     * @param viewPlugin Pointer to the view plugin to remove
     */
    void removeViewPluginDockWidget(mv::plugin::ViewPlugin * viewPlugin);

    /** Resets the docking layout to defaults */
    void reset();

    /** Adds a ViewPluginDockWidget */
    void addViewPluginDockWidget(ads::DockWidgetArea area, ads::CDockWidget* Dockwidget, ads::CDockAreaWidget* DockAreaWidget = nullptr);

    /** Removes a ViewPluginDockWidget */
    void removeViewPluginDockWidget(ads::CDockWidget* Dockwidget);

    /** get as QWidget pointer*/
    QWidget* getWidget();

public: // Serialization task

    /** Get task for reporting serialization progress */
    mv::Task* getSerializationTask();

    /**
     * Set task for reporting serialization progress to \p serializationTask
     * @param serializationTask Pointer to serialization task
     */
    void setSerializationTask(mv::Task* serializationTask);

public: // Serialization

    /**
     * Load from variant map
     * @param variantMap Variant map representation of the serializable object
     */
    void fromVariantMap(const QVariantMap& variantMap) override;
    
    /**
     * Save to variant map
     * @return Variant map representation of the serializable object
     */
    QVariantMap toVariantMap() const override;

    friend class ViewPluginsDockWidget;
    friend class QPointer<DockManager>;

private:
    mv::Task*     _serializationTask;     /** For reporting serialization progress */
    mv::Task      _layoutTask;            /** For reporting layout progress */

private:
    ViewPluginDockWidgets _orderedViewPluginDockWidgets; /* the ViewPluginDockWidgets in the order in which they were created */
};
