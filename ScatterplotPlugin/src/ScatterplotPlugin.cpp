#include "ScatterplotPlugin.h"

#include "ScatterplotSettings.h"
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

    settings = new ScatterplotSettings(this);

    addWidget(widget);
    addWidget(settings);
}

void ScatterplotPlugin::dataAdded(const QString name)
{
    settings->addDataOption(name);
}

void ScatterplotPlugin::dataChanged(const QString name)
{
    if (name != settings->currentData()) {
        return;
    }
    updateData();
}

void ScatterplotPlugin::dataRemoved(const QString name)
{
    
}

void ScatterplotPlugin::selectionChanged(const QString dataName)
{
    qDebug() << getName() << "Selection updated";
    updateData();
}

QStringList ScatterplotPlugin::supportedDataKinds()
{
    QStringList supportedKinds;
    supportedKinds << "Points";
    return supportedKinds;
}

void ScatterplotPlugin::dataSetPicked(const QString& name)
{
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));

    int nDim = points->numDimensions;

    settings->initDimOptions(nDim);

    updateData();
}

void ScatterplotPlugin::pointSizeChanged(const int size)
{
    widget->setPointSize(size);
}

void ScatterplotPlugin::subsetCreated()
{
    qDebug() << "Creating subset";
    const hdps::Set* set = _core->requestData(settings->currentData());
    const hdps::Set* selection = _core->requestSelection(set->getDataName());
    _core->createSubsetFromSelection(selection, "Subset");
}

void ScatterplotPlugin::xDimPicked(int index)
{
    updateData();
}

void ScatterplotPlugin::yDimPicked(int index)
{
    updateData();
}


void ScatterplotPlugin::updateData()
{
    qDebug() << "UPDATING";
    const IndexSet* dataSet = dynamic_cast<const IndexSet*>(_core->requestData(settings->currentData()));
    const PointsPlugin* points = dynamic_cast<const PointsPlugin*>(_core->requestPlugin(dataSet->getDataName()));
    const IndexSet* selection = dynamic_cast<const IndexSet*>(_core->requestSelection(points->getName()));
    
    std::vector<float>* positions = new std::vector<float>();
    std::vector<float> colors;

    int nDim = points->numDimensions;

    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();
    qDebug() << "X: " << xIndex << " Y: " << yIndex;
    if (xIndex < 0 || yIndex < 0)
        return;

    // Calculate data bounds
    float maxLength = getMaxLength(&points->data, nDim);

    if (dataSet->isFull()) {
        unsigned int numPoints = points->data.size() / nDim;

        positions->resize(numPoints * 2);
        colors.resize(numPoints * 3, 0.5f);

        for (int i = 0; i < numPoints; i++)
        {
            (*positions)[i * 2 + 0] = points->data[i * nDim + xIndex] / maxLength;
            (*positions)[i * 2 + 1] = points->data[i * nDim + yIndex] / maxLength;
            if (nDim >= 5) {
                colors[i * 3 + 0] = points->data[i * nDim + 2];
                colors[i * 3 + 1] = points->data[i * nDim + 3];
                colors[i * 3 + 2] = points->data[i * nDim + 4];
            }
        }

        for (unsigned int index : selection->indices)
        {
            colors[index * 3 + 0] = 1.0f;
            colors[index * 3 + 1] = 0.5f;
            colors[index * 3 + 2] = 1.0f;
        }
    }
    else {
        unsigned int numPoints = selection->indices.size();
        positions->resize(numPoints * 2);
        colors.resize(numPoints * 3, 0.5f);

        for (unsigned int index : dataSet->indices) {
            (*positions)[index * 2 + 0] = points->data[index * nDim + xIndex] / maxLength;
            (*positions)[index * 2 + 1] = points->data[index * nDim + yIndex] / maxLength;

            bool selected = false;
            for (unsigned int selectionIndex : selection->indices) {
                if (index == selectionIndex) {
                    selected = true;
                }
            }
            if (selected) {
                colors[index * 3 + 0] = 1.0f;
                colors[index * 3 + 1] = 0.5f;
                colors[index * 3 + 2] = 1.0f;
            }
            else {
                if (nDim >= 5) {
                    colors[index * 3 + 0] = points->data[index * nDim + 2];
                    colors[index * 3 + 1] = points->data[index * nDim + 3];
                    colors[index * 3 + 2] = points->data[index * nDim + 4];
                }
            }
        }
    }
    qDebug() << "Setting positions";
    widget->setData(positions);
    qDebug() << "Setting colors";
    widget->setColors(colors);
    qDebug() << "DONE UPDATING";
}

void ScatterplotPlugin::onSelection(const std::vector<unsigned int> selection) const
{
    if (settings->numDataOptions() == 0)
        return;

    const IndexSet* set = dynamic_cast<IndexSet*>(_core->requestData(settings->currentData()));
    IndexSet* selectionSet = dynamic_cast<IndexSet*>(_core->requestSelection(set->getDataName()));
    qDebug() << "Selection size: " << selection.size();
    selectionSet->indices.clear();
    if (set->isFull())
    {
        for (unsigned int index : selection) {
            selectionSet->indices.push_back(index);
        }
    }
    else
    {
        for (unsigned int index : selection) {
            selectionSet->indices.push_back(set->indices[index]);
        }
    }
    qDebug() << "Selection on: " << selectionSet->getDataName();
    _core->notifySelectionChanged(selectionSet->getDataName());
}

float ScatterplotPlugin::getMaxLength(const std::vector<float>* data, const int nDim) const
{
    int xIndex = settings->getXDimension();
    int yIndex = settings->getYDimension();

    float maxLength = 0;
    for (int i = 0; i < data->size() / nDim; i++) {
        float x = data->at(i * nDim + xIndex);
        float y = data->at(i * nDim + yIndex);
        float length = x*x + y*y;

        if (length > maxLength)
            maxLength = length;
    }
    return sqrt(maxLength);
}

// =============================================================================
// Factory
// =============================================================================

ViewPlugin* ScatterplotPluginFactory::produce()
{
    return new ScatterplotPlugin();
}
