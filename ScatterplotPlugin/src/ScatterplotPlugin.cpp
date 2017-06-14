#include "ScatterplotPlugin.h"

#include "PointsPlugin.h"

#include <QtCore>
#include <QtDebug>

Q_PLUGIN_METADATA(IID "nl.tudelft.ScatterplotPlugin")

// =============================================================================
// View
// =============================================================================

ScatterplotPlugin::~ScatterplotPlugin(void)
{
    
}

void ScatterplotPlugin::init()
{
    widget = new hdps::gui::ScatterplotWidget();
    widget->setPointSize(10);
    widget->setAlpha(0.5f);
    widget->addSelectionListener(this);
    
    connect(&dataOptions, SIGNAL(currentIndexChanged(QString)), SLOT(dataSetPicked(QString)));
    addWidget(&dataOptions);
    addWidget(widget);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    DataTypePlugin* data = _core->requestData(name);
    if (data->getKind() == "Points") {
        dataOptions.addItem(name);
        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);
        qDebug() << "Data Added for scatterplot: " << points->data.size();
        updateData(*points);
    }
}

void ScatterplotPlugin::dataChanged(const QString name)
{
    DataTypePlugin* data = _core->requestData(name);
    if (data->getKind() == "Points") {
        const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);
        qDebug() << "Data Changed for scatterplot: " << points->data.size();
        updateData(*points);
    }
}

void ScatterplotPlugin::dataRemoved(const QString name)
{
    
}

QStringList ScatterplotPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void ScatterplotPlugin::dataSetPicked(const QString& name)
{
    DataTypePlugin* data = _core->requestData(name);
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(data);

    updateData(*points);
}

void ScatterplotPlugin::updateData(const PointsPlugin& points)
{
    std::vector<float>* data = new std::vector<float>();
    std::vector<float> colors;
    for (int i = 0; i < points.data.size() / 5; i++) {
        data->push_back(points.data[i * 5 + 0]);
        data->push_back(points.data[i * 5 + 1]);
        colors.push_back(points.data[i * 5 + 2]);
        colors.push_back(points.data[i * 5 + 3]);
        colors.push_back(points.data[i * 5 + 4]);
    }

    widget->setData(data);
    widget->setColors(colors);
}

void ScatterplotPlugin::onSelection(const std::vector<unsigned int> selection) const
{
    _core->addData("Selection");
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
