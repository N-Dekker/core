#ifndef HDPS_PLUGINMANAGER_H
#define HDPS_PLUGINMANAGER_H

#include "Core.h"

#include "PluginFactory.h"

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

class PluginManager : public QObject {
public:
    PluginManager(Core& core);
    ~PluginManager(void) override;
    
    /**
    * Loads all plugin factories from the plugin directory and adds them as menu items.
    */
    void loadPlugins();

    /**
    * Creates a new plugin instance of the given kind and adds it to the core.
    */
    QString createPlugin(const QString kind);
    
    /**
     * Create an analysis plugin
     * @param kind Kind of analysis
     * @param inputDatasetName Name of the input dataset
     */
    void createAnalysisPlugin(const QString& kind, const QString& inputDatasetName);

    /**
     * Create an exporter plugin
     * @param kind Kind of exporter
     * @param inputDatasetName Name of the input dataset
     */
    void createExporterPlugin(const QString& kind, const QString& inputDatasetName);

    /**
     * Get a list of plugin kinds (names) given a plugin type and data type(s)
     * @param pluginType Type of plugin e.g. analysis, exporter
     * @param dataTypes Types of data that the plugin should be compatible with (data type ignored when empty)
     * @return List of compatible plugin kinds that can handle the data type
     */
    QStringList getPluginKindsByPluginTypeAndDataTypes(const Type& pluginType, const QVector<DataType>& dataTypes = QVector<DataType>());

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

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGINMANAGER_H
