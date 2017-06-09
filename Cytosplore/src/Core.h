#ifndef HDPS_CORE_H
#define HDPS_CORE_H

#include "CoreInterface.h"

#include <memory>
#include <unordered_map>
#include <vector>

namespace hdps {

namespace plugin {
    class PluginManager;
    class Plugin;
    class DataConsumer;
    enum Type;
}

namespace gui {
    class MainWindow;
}

class Core : public CoreInterface
{
public:
    Core(gui::MainWindow& mainWindow);
    ~Core();

    /**
    * Initializes the core. Loads all plugins from the designated plugin directory.
    */
    void init();

    /**
    * Adds the given plugin to the list of plugins kept by the core.
    */
    void addPlugin(plugin::Plugin* plugin);

    /**
    * Create a new dataset instance of the given kind.
    */
    const QString addData(const QString kind) override;

    /**
     * Request a dataset from the core by its name.
     */
    plugin::DataTypePlugin* requestData(const QString name) override;

    /** Notify all data consumers that a new dataset has been added to the core. */
    void notifyDataAdded(const QString name);
    /** Notify all data consumers that a dataset has been changed. */
    void notifyDataChanged(const QString name);
    /** Notify all data consumers that a dataset has been removed. */
    void notifyDataRemoved(const QString name);

    /**
    * Returns a reference to the main window for adding widgets to it.
    */
    gui::MainWindow& gui() const;
private:
    std::vector<plugin::DataConsumer*> getDataConsumers();

private:
    gui::MainWindow& _mainWindow;

    /** Plugin manager responsible for loading plugins and adding them to the core. */
    std::unique_ptr<plugin::PluginManager> _pluginManager;

    /** List of plugin instances currently present in the application. Instances are stored by type. */
    std::unordered_map<plugin::Type, std::vector<std::unique_ptr<plugin::Plugin>>> _plugins;
};

} // namespace hdps

#endif // HDPS_CORE_H
