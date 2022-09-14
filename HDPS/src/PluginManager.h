#pragma once

#include "Core.h"

#include "PluginFactory.h"

#include <actions/WidgetAction.h>

#include <QObject>
#include <QString>
#include <QStringList>
#include <QHash>
#include <QDir>

namespace Ui
{
    class MainWindow;
}

namespace hdps
{
namespace plugin
{

class PluginManager : public WidgetAction
{
public:
    PluginManager(Core& core);
    ~PluginManager(void) override;
    
    /**
    * Loads all plugin factories from the plugin directory and adds them as menu items.
    */
    void loadPlugins();

    /**
    * Creates a new plugin instance of the given kind and adds it to the core
    * @param kind Kind of plugin
    * @param datasets Zero or more datasets upon which the plugin is based (e.g. analysis plugin)
    * @return Pointer to created plugin
    */
    Plugin* createPlugin(const QString& kind, const Datasets& datasets = Datasets());
    
    /**
     * Create a plugin of \p kind
     * @param kind Kind of plugin (name of the plugin)
     * @return Pointer to created plugin
     */
    template<typename PluginType>
    PluginType* requestPlugin(const QString& kind, const Datasets& datasets)
    {
        return dynamic_cast<PluginType*>(createPlugin(kind, datasets));
    }

    /**
     * Get plugin kinds by plugin type(s)
     * @param pluginTypes Plugin type(s)
     * @return Plugin kinds
     */
    QStringList getPluginKindsByPluginTypes(const plugin::Types& pluginTypes) const;

    /**
     * Get plugin trigger actions by \p pluginType and \p datasets
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const Datasets& datasets) const;

    /**
     * Get plugin trigger actions by \p pluginType and \p dataTypes
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const plugin::Type& pluginType, const DataTypes& dataTypes) const;

    /**
     * Get plugin trigger actions by \p pluginKind and \p datasets
     * @param pluginKind Kind of plugin
     * @param datasets Vector of input datasets
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const Datasets& datasets) const;

    /**
     * Get plugin trigger actions by \p pluginKind and \p dataTypes
     * @param pluginKind Kind of plugin
     * @param dataTypes Vector of input data types
     * @return Vector of plugin trigger actions
     */
    gui::PluginTriggerActions getPluginTriggerActions(const QString& pluginKind, const DataTypes& dataTypes) const;

    /**
     * Get plugin GUI name from plugin kind
     * @param pluginKind Kind of plugin
     * @param GUI name of the plugin, empty if the plugin kind was not found
     */
    QString getPluginGuiName(const QString& pluginKind) const;

    /**
     * Get a list of plugin kinds (names) given a plugin type
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @return List of compatible plugin kinds that can handle the data type
     */
    QStringList requestPluginKindsByPluginType(const plugin::Type& pluginType);

    /**
     * Get plugin icon from plugin kind
     * @param pluginKind Kind of plugin
     * @return Plugin icon name of the plugin, null icon the plugin kind was not found
     */
    QIcon getPluginIcon(const QString& pluginKind) const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:

    /**
    * Resolves plugin dependencies, returns list of resolved plugin filenames.
    */
    QStringList resolveDependencies(QDir pluginDir) const;

    Core& _core;

    QHash<QString, PluginFactory*> _pluginFactories;

private slots:
    QString pluginTriggered(const QString& kind);
};

}

}
