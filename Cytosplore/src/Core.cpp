#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "DataTypePlugin.h"
#include "DataConsumer.h"
namespace hdps {

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core() {

}

void Core::init() {
    _pluginManager = std::unique_ptr<plugin::PluginManager>(new plugin::PluginManager(*this));

    _pluginManager->LoadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin) {
    // If is it a view plugin then it should be added to the main window
    if (plugin->getType() == plugin::Type::VIEW) {
        _mainWindow.addView(plugin);
    }

    _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));

    // Initialize the plugin after it has been added to the core
    plugin->init();

    // 
    if (plugin->getType() == plugin::Type::DATA_TYPE) {
        notifyDataAdded(*dynamic_cast<plugin::DataTypePlugin*>(plugin));
    }
}

void Core::addData(const QString kind) {
    _pluginManager->AddPlugin(kind);
}

void Core::notifyDataAdded(const plugin::DataTypePlugin& data) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataAdded(data);
            }
        }
    }
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

} // namespace hdps
