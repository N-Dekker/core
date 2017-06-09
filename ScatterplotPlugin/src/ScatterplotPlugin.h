#ifndef SCATTERPLOT_PLUGIN_H
#define SCATTERPLOT_PLUGIN_H

#include <ViewPlugin.h>

#include <QComboBox>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ScatterplotWidget;

class ScatterplotPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ScatterplotPlugin() : ViewPlugin("Scatterplot View") { }
    ~ScatterplotPlugin(void);
    
    void init();
    
    void updateData(const PointsPlugin& data);

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
protected slots:
    void dataSetPicked(const QString& name);
private:
    ScatterplotWidget* widget;

    QComboBox dataOptions;
};


// =============================================================================
// Factory
// =============================================================================

class ScatterplotPluginFactory : public ViewPluginFactory
{
    Q_INTERFACES(hdps::plugin::ViewPluginFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ScatterplotPlugin"
                      FILE  "ScatterplotPlugin.json")
    
public:
    ScatterplotPluginFactory(void) {}
    ~ScatterplotPluginFactory(void) {}
    
    ViewPlugin* produce();
};

#endif // SCATTERPLOT_PLUGIN_H
