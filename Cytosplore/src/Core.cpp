#include "Core.h"

#include "MainWindow.h"
#include "PluginManager.h"

#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "DataTypePlugin.h"
#include "DataConsumer.h"

namespace hdps {

Core::Core(gui::MainWindow& mainWindow)
: _mainWindow(mainWindow)
{
    
}

Core::~Core() {
    // Delete the plugin manager
    _pluginManager.reset();

    // Delete all plugins
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            kv.second[i].reset();
        }
    }
}

void Core::init() {
    _pluginManager = std::unique_ptr<plugin::PluginManager>(new plugin::PluginManager(*this));

    _pluginManager->LoadPlugins();
}

void Core::addPlugin(plugin::Plugin* plugin) {
    plugin->setCore(this);

    // If it is a view plugin then it should be added to the main window
    if (plugin->getType() == plugin::Type::VIEW) {
        _mainWindow.addView(dynamic_cast<plugin::ViewPlugin*>(plugin));
    }

    _plugins[plugin->getType()].push_back(std::unique_ptr<plugin::Plugin>(plugin));

    // Initialize the plugin after it has been added to the core
    plugin->init();

    // Notify data consumers about the new data set
    if (plugin->getType() == plugin::Type::DATA_TYPE) {
        notifyDataAdded(plugin->getName());
    }

    // If it is a loader plugin it should call loadData
    if (plugin->getType() == plugin::Type::LOADER) {
        dynamic_cast<plugin::LoaderPlugin*>(plugin)->loadData();
    }
    // If it is a writer plugin it should call writeData
    if (plugin->getType() == plugin::Type::WRITER) {
        dynamic_cast<plugin::WriterPlugin*>(plugin)->writeData();
    }
}

const QString Core::addData(const QString kind) {
    return _pluginManager->AddPlugin(kind);
}

plugin::DataTypePlugin* Core::requestData(const QString name)
{
    for (std::unique_ptr<plugin::Plugin>& plugin : _plugins[plugin::Type::DATA_TYPE]) {
        if (plugin->getName() == name) {
            return dynamic_cast<plugin::DataTypePlugin*>(plugin.get());
        }
    }
    qFatal((QString("Failed to find plugin with name: ") + name).toStdString().c_str());
}

void Core::notifyDataAdded(const QString name) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataAdded(name);
            }

            plugin::ViewPlugin* vp = dynamic_cast<plugin::ViewPlugin*>(kv.second[i].get());
            if (vp) {
                vp->addData(name);
            }
        }
    }
}

void Core::notifyDataChanged(const QString name) {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataChanged(name);
            }
        }
    }
}

void Core::notifyDataRemoved() {
    for (auto& kv : _plugins) {
        for (int i = 0; i < kv.second.size(); ++i) {
            plugin::DataConsumer* dc = dynamic_cast<plugin::DataConsumer*>(kv.second[i].get());

            if (dc) {
                dc->dataRemoved();
            }
        }
    }
}

gui::MainWindow& Core::gui() const {
    return _mainWindow;
}

} // namespace hdps
