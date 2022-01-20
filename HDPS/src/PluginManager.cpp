#include "PluginManager.h"

#include <QApplication>
#include <QMenuBar>
#include <QDebug>
#include <QPluginLoader>
#include <QSignalMapper>
#include <QMessageBox>

#include <assert.h>

#include "MainWindow.h"
#include "PluginFactory.h"
#include "AnalysisPlugin.h"
#include "RawData.h"
#include "LoaderPlugin.h"
#include "WriterPlugin.h"
#include "ViewPlugin.h"
#include "TransformationPlugin.h"
#include "PluginType.h"

#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>

#include <stdexcept>

namespace hdps {

namespace plugin {

PluginManager::PluginManager(Core& core)
: _core(core)
{
    
}

PluginManager::~PluginManager(void)
{
    
}

void PluginManager::loadPlugins()
{
	QDir pluginDir(qApp->applicationDirPath());
    
#if defined(Q_OS_WIN)
    //if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
    //    pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginDir.dirName() == "MacOS")
    {
        pluginDir.cdUp();
        pluginDir.cdUp();
        pluginDir.cdUp();
    }
#endif
    pluginDir.cd("Plugins");
    
    _pluginFactories.clear();

    QSignalMapper* signalMapper = new QSignalMapper(this);
    
    // List of filenames of dependency resolved plugins
    const QStringList resolvedPlugins = resolveDependencies(pluginDir);

    // For each of the plugin files which are resolved, load them and add them in their proper menu category
    for (const QString& fileName: resolvedPlugins)
    {
        // Dynamic loader of plugin shared library
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));
        gui::MainWindow& gui = _core.gui();

        // Get metadata about plugin from the accompanying .json file compiled in the shared library
        QString pluginKind = pluginLoader.metaData().value("MetaData").toObject().value("name").toString();
        QString menuName = pluginLoader.metaData().value("MetaData").toObject().value("menuName").toString();

        // Create an instance of the plugin, i.e. the factory
        QObject *pluginFactory = pluginLoader.instance();

        // If pluginFactory is a nullptr then loading of the plugin failed for some reason. Print the reason to output.
        if (!pluginFactory)
        {
            qWarning() << "Failed to load plugin: " << fileName << pluginLoader.errorString();
            continue;
        }

        // Loading of the plugin succeeded so cast it to its original class
        _pluginFactories[pluginKind] = qobject_cast<PluginFactory*>(pluginFactory);
        _pluginFactories[pluginKind]->setKind(pluginKind);
        _pluginFactories[pluginKind]->setGuiName(menuName);

        // Add the plugin to a menu item and link the triggering of the menu item to triggering of the plugin
        QAction* action = NULL;

        if (qobject_cast<AnalysisPluginFactory*>(pluginFactory))
        {
            action = gui.addMenuAction(plugin::Type::ANALYSIS, pluginKind);
        }
        else if (qobject_cast<RawDataFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<LoaderPluginFactory*>(pluginFactory))
        {
            action = gui.addImportOption(menuName);
        }
        else if (qobject_cast<WriterPluginFactory*>(pluginFactory))
        {
        }
        else if (qobject_cast<ViewPluginFactory*>(pluginFactory))
        {
            action = gui.addMenuAction(plugin::Type::VIEW, pluginKind);
        }
        else if (qobject_cast<TransformationPluginFactory*>(pluginFactory))
        {
        }
        else
        {
            qDebug() << "Plugin " << fileName << " does not implement any of the possible interfaces!";
            return;
        }
        
        if (action)
        {
            QObject::connect(action, &QAction::triggered, signalMapper, static_cast<void (QSignalMapper::*)()>(&QSignalMapper::map));
            signalMapper->setMapping(action, pluginKind);
        }
    }

    QObject::connect(signalMapper, static_cast<void (QSignalMapper::*)(const QString&)>(&QSignalMapper::mapped), this, &PluginManager::pluginTriggered);
}

QStringList PluginManager::resolveDependencies(QDir pluginDir) const
{
    // Map keeping track of the list of plugin kinds on which a plugin is dependent
    QMap<QString, QStringList> dependencies;
    // List of plugin kinds which have had their dependencies resolved
    QStringList resolved;
    // Map from plugin kinds to plugin file names
    QMap<QString, QString> kindToPluginNameMap;

    /*
     * For each item in the plugin directory, store their dependencies. If a plugin has no dependencies,
     * immediately add it to the list of resolved plugins. Dependencies are given by a list of plugin kinds
     * under the 'dependencies' key in the accompanying .json metadata file.
     */
    for (QString fileName: pluginDir.entryList(QDir::Files))
    {
        QPluginLoader pluginLoader(pluginDir.absoluteFilePath(fileName));

        QJsonObject metaData = pluginLoader.metaData().value("MetaData").toObject();
        QString kind = metaData.value("name").toString();
        QJsonArray dependencyData = metaData.value("dependencies").toArray();

        // Map plugin kind to plugin file name
        kindToPluginNameMap[kind] = fileName;

        // If plugin has no dependencies, add it to the resolved list
        if (dependencyData.size() == 0)
        {
            resolved.push_back(kind);
            continue;
        }

        // Store plugin dependency list in a map
        QStringList dependencyList;
        for (const QJsonValue& dependency : dependencyData)
        {
            dependencyList.push_back(dependency.toString());
        }
        dependencies[kind] = dependencyList;
    }

    qDebug() << "Dependencies: " << dependencies;
    qDebug() << "Resolved: " << resolved;

    // Attempt to resolve all plugin dependencies in an iterative manner
    while (dependencies.size() != 0)
    {
        bool removed = false;

        // For all dependencies of a plugin, check if any of its dependencies are already resolved.
        // If so, remove that dependency from the list.
        for (QStringList& depList : dependencies)
        {
            QMutableListIterator<QString> it(depList);
            while (it.hasNext()) {
                QString dependency = it.next();
                if (resolved.contains(dependency))
                {
                    it.remove();
                    removed = true;
                }
            }
        }

        // Check for unresolved plugins which now have zero dependencies and mark them as resolved
        QStringList keys = dependencies.keys();
        for (QString key : keys) {
            QStringList depList = dependencies[key];

            if (depList.size() == 0) {
                dependencies.remove(key);
                resolved.push_back(key);
                qDebug() << "Resolved: " << key;
            }
        }

        // If no dependencies were resolved this iteration we can stop
        if (!removed)
            break;
    }

    // TODO List all of the plugins which have unresolved dependencies and what they are
    // Check if this plugin has all its dependencies resolved
    /*if (!resolvedPlugins.contains(fileName)) {
        qWarning() << "Plugin: " << pluginName << " has unresolved dependencies.";
        continue;
    }*/

    // Form a list of resolved plugin file names rather than kinds and return it
    QStringList resolvedOrderedPluginNames;
    for (const QString& kind : resolved)
    {
        resolvedOrderedPluginNames.push_back(kindToPluginNameMap[kind]);
    }
    return resolvedOrderedPluginNames;
}

QString PluginManager::createPlugin(const QString kind)
{
    return pluginTriggered(kind);
}

void PluginManager::createAnalysisPlugin(const QString& kind, Dataset<DatasetImpl>& dataset)
{
    try
    {
        if (!_pluginFactories.keys().contains(kind))
            throw std::runtime_error("Unrecognized plugin kind");

        auto pluginInstance = dynamic_cast<AnalysisPlugin*>(_pluginFactories[kind]->produce());

        if (!pluginInstance)
            return;

        pluginInstance->setInputDataset(dataset);

        _core.addPlugin(pluginInstance);
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(nullptr, "HDPS", QString("Unable to create analysis plugin: %1").arg(e.what()));
    }
}

void PluginManager::createExporterPlugin(const QString& kind, Dataset<DatasetImpl>& dataset)
{
    try
    {
        if (!_pluginFactories.keys().contains(kind))
            throw std::runtime_error("Unrecognized plugin kind");

        auto pluginInstance = dynamic_cast<WriterPlugin*>(_pluginFactories[kind]->produce());

        if (!pluginInstance)
            return;

        pluginInstance->setInputDataset(dataset);

        _core.addPlugin(pluginInstance);
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(nullptr, "HDPS", QString("Unable to create analysis plugin: %1").arg(e.what()));
    }
}

void PluginManager::createViewPlugin(const QString& kind, const Datasets& datasets)
{
    try
    {
        if (!_pluginFactories.keys().contains(kind))
            throw std::runtime_error("Unrecognized plugin kind");

        auto pluginInstance = dynamic_cast<ViewPlugin*>(_pluginFactories[kind]->produce());

        if (!pluginInstance)
            return;

        _core.addPlugin(pluginInstance);

        pluginInstance->loadData(datasets);
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(nullptr, "HDPS", QString("Unable to create analysis plugin: %1").arg(e.what()));
    }
}

void PluginManager::createTransformationPlugin(const QString& kind, const Datasets& datasets)
{
    try
    {
        if (!_pluginFactories.keys().contains(kind))
            throw std::runtime_error("Unrecognized plugin kind");

        auto pluginInstance = dynamic_cast<TransformationPlugin*>(_pluginFactories[kind]->produce());

        if (!pluginInstance)
            return;

        _core.addPlugin(pluginInstance);

        pluginInstance->transform(datasets);
    }
    catch (std::exception& e)
    {
        QMessageBox::warning(nullptr, "HDPS", QString("Unable to create analysis plugin: %1").arg(e.what()));
    }
}

QStringList PluginManager::getPluginKindsByPluginTypeAndDataTypes(const Type& pluginType, const QVector<DataType>& dataTypes /*= QVector<DataType>()*/)
{
    QStringList pluginKinds;

    for (auto pluginFactory : _pluginFactories) {
        if (pluginFactory->getType() != pluginType)
            continue;

        auto pluginCompatible = dataTypes.isEmpty();

        for (auto dataType : dataTypes) {
            if (pluginFactory->supportedDataTypes().contains(dataType)) {
                pluginCompatible = true;
                break;
            }
        }

        if (pluginCompatible)
            pluginKinds << pluginFactory->getKind();
    }

    pluginKinds.sort();

    return pluginKinds;
}

QString PluginManager::getPluginGuiName(const QString& pluginKind) const
{
    if (!_pluginFactories.contains(pluginKind))
        return "";

    return _pluginFactories[pluginKind]->getGuiName();
}

QStringList PluginManager::requestPluginKindsByPluginType(const plugin::Type& pluginType)
{
    QStringList pluginKinds;

    for (auto pluginFactory : _pluginFactories) {
        if (pluginFactory->getType() != pluginType)
            continue;

        pluginKinds << pluginFactory->getKind();
    }

    pluginKinds.sort();

    return pluginKinds;
}

QIcon PluginManager::getPluginIcon(const QString& pluginKind) const
{
    if (!_pluginFactories.contains(pluginKind))
        return QIcon();

    return _pluginFactories[pluginKind]->getIcon();
}

QString PluginManager::pluginTriggered(const QString& kind)
{
    PluginFactory *pluginFactory = _pluginFactories[kind];
    Plugin* plugin = pluginFactory->produce();

    _core.addPlugin(plugin);
    qDebug() << "Added plugin" << plugin->getKind() << "with version" << plugin->getVersion();
    return plugin->getName();
}

} // namespace plugin

} // namespace hdps
