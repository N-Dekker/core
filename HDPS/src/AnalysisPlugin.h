#pragma once

#include "PluginFactory.h"
#include "DataConsumer.h"
#include "widgets/SettingsWidget.h"

#include <memory>

namespace hdps
{
namespace plugin
{

class AnalysisPlugin : public Plugin, public DataConsumer
{
public:
    AnalysisPlugin(QString name) : Plugin(Type::ANALYSIS, name) { }
    ~AnalysisPlugin() override {};

    bool hasSettings()
    {
        return getSettings() != nullptr;
    }

    virtual gui::SettingsWidget* const getSettings() = 0;
};


class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    
    ~AnalysisPluginFactory() override {};
    
    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    AnalysisPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace hdps

Q_DECLARE_INTERFACE(hdps::plugin::AnalysisPluginFactory, "cytosplore.AnalysisPluginFactory")
