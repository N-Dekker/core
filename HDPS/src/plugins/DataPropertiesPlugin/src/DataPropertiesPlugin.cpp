#include "DataPropertiesPlugin.h"

#include <Application.h>
#include <CoreInterface.h>
#include <AbstractDataHierarchyManager.h>

Q_PLUGIN_METADATA(IID "nl.BioVault.DataPropertiesPlugin")

using namespace hdps;

DataPropertiesPlugin::DataPropertiesPlugin(const PluginFactory* factory) :
    ViewPlugin(factory),
    _dataPropertiesWidget(nullptr)
{
}

void DataPropertiesPlugin::init()
{
    auto layout = new QVBoxLayout();

    layout->setContentsMargins(6, 6, 6, 6);

    layout->addWidget(&_dataPropertiesWidget);

    getWidget().setLayout(layout);

    connect(&dataHierarchy(), &AbstractDataHierarchyManager::selectedItemsChanged, this, [this](DataHierarchyItems selectedItems) -> void {
        if (selectedItems.isEmpty())
            getWidget().setWindowTitle("Data properties");
        else
            getWidget().setWindowTitle("Data properties: " + selectedItems.first()->getLocation());
    });

    getWidget().setWindowTitle("Data properties");
}

DataPropertiesPluginFactory::DataPropertiesPluginFactory() :
    ViewPluginFactory(true)
{
}

QIcon DataPropertiesPluginFactory::getIcon(const QColor& color /*= Qt::black*/) const
{
    return Application::getIconFont("FontAwesome").getIcon("sliders-h", color);
}

ViewPlugin* DataPropertiesPluginFactory::produce()
{
    return new DataPropertiesPlugin(this);
}
