#pragma once

#include <ViewPlugin.h>

#include "PixelSelectionTool.h"

#include "widgets/DataSlot.h"
#include "ScatterplotWidget.h"

#include <QComboBox>
#include <QSlider>
#include <QPushButton>
#include <QRectF>

using namespace hdps::plugin;

// =============================================================================
// View
// =============================================================================

class ScatterplotSettings;
class Points;

namespace hdps
{
    class Vector2f;
}

class ScatterplotPlugin : public ViewPlugin
{
    Q_OBJECT
    
public:
    ScatterplotPlugin() :
        ViewPlugin("Scatterplot View"),
        _pixelSelectionTool(new PixelSelectionTool(this, false))
    {
        setDockingLocation(DockableWidget::DockingLocation::Right);
    }

    ~ScatterplotPlugin(void) override;
    
    /** Returns the icon of this plugin */
    QIcon getIcon() const override {
        return Application::getIconFont("FontAwesome").getIcon("braille");
    }

    void init() override;

    void onDataChanged(QString name);
    void onDataRemoved(QString name);
    void onSelectionChanged(QString name);

    ScatterplotWidget* _scatterPlotWidget;
    hdps::DataTypes supportedColorTypes;

    PixelSelectionTool& getSelectionTool();

    bool eventFilter(QObject* target, QEvent* event) override;

    QString getCurrentDataset() const;
    std::uint32_t getNumPoints() const;
    std::uint32_t getNumSelectedPoints() const;

public: // Selection

    bool canSelect() const;
    bool canSelectAll() const;
    bool canClearSelection() const;
    bool canInvertSelection() const;

    void selectAll();
    void clearSelection();
    void invertSelection();

private:

    /** Updates the window title (includes the name of the loaded dataset) */
    void updateWindowTitle();

public slots:
    void onDataInput(QString dataSetName);
    void onColorDataInput(QString dataSetName);

protected slots:
    void subsetCreated();

    void xDimPicked(int index);
    void yDimPicked(int index);
    void cDimPicked(int index);

    void selectPoints();

signals:
    void currentDatasetChanged(const QString& datasetName);
    void selectionChanged();

private:
    void updateData();
    void calculatePositions(const Points& points);
    void calculateScalars(std::vector<float>& scalars, const Points& points, int colorIndex);
    void updateSelection();

    //const Points* makeSelection(hdps::Selection selection);

    QString _currentDataSet;
    DataSlot* _dataSlot;
    ScatterplotSettings* _scatterPlotSettings;

    std::vector<hdps::Vector2f> _points;
    unsigned int _numPoints;

    PixelSelectionTool*     _pixelSelectionTool;     /** Pixel selection tool */
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
    ~ScatterplotPluginFactory(void) override {}
    
    ViewPlugin* produce() override;
};
