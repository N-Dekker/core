#ifndef SCATTERPLOT_PLUGIN_H
#define SCATTERPLOT_PLUGIN_H

#include <ViewPlugin.h>
#include "widgets/ScatterplotWidget.h"

#include <QComboBox>
#include <QSlider>
#include <QPushButton>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class PointsPlugin;

class ScatterplotPlugin : public ViewPlugin, public SelectionListener
{
    Q_OBJECT
    
public:
    ScatterplotPlugin() : ViewPlugin("Scatterplot View"), pointSizeSlider(Qt::Horizontal) { }
    ~ScatterplotPlugin(void);
    
    void init();

    void dataAdded(const QString name) Q_DECL_OVERRIDE;
    void dataChanged(const QString name) Q_DECL_OVERRIDE;
    void dataRemoved(const QString name) Q_DECL_OVERRIDE;
    void selectionChanged(const QString dataName) Q_DECL_OVERRIDE;
    QStringList supportedDataKinds() Q_DECL_OVERRIDE;
    virtual void onSelection(const std::vector<unsigned int> selection) const;
protected slots:
    void dataSetPicked(const QString& name);
    void pointSizeChanged(const int size);
    void subsetCreated();
private:
    void updateData();

    hdps::gui::ScatterplotWidget* widget;

    QComboBox dataOptions;
    QSlider pointSizeSlider;
    QPushButton subsetButton;
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
