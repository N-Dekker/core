#ifndef HDPS_PLUGIN_H
#define HDPS_PLUGIN_H

#include "CoreInterface.h"
#include "PluginType.h"

#include <QString>
#include <QMap>
#include <QVariant>
#include <QUuid>

class QMenu;

namespace hdps
{
namespace plugin
{

class Plugin
{
public:
    Plugin(Type type, QString kind);
    virtual ~Plugin() {};

    /**
     * Can be implemented to initialize the plugin to a certain state.
     * This function gets called when the plugin is first instantiated.
     */
    virtual void init() = 0;
	
    /**
     * Returns the unique name of this plugin.
     */
    QString getName() const
    {
        return _name;
    }

    /**
     * Returns the GUI name of this plugin.
     */
    QString getGuiName() const
    {
        return _guiName;
    }

    /**
    * Returns the kind of plugin. The kind is specific to the
    * particular implementation of a plugin type.
    */
    QString getKind() const
    {
        return _kind;
    }

    /**
     * Returns the type of plugin, see all types in PluginType
     */
    Type getType() const
    {
        return _type;
    }

    /**
     * Returns the version of the plugin. If no version string is implemented 
     * by this particular plugin it will return the "No version" string.
     */
    virtual QString getVersion() const
    {
        return "No Version";
    }

    /**
     * Stores a reference to the core in the plugin via the CoreInterface
     * class, which provides restricted access to the core.
     */
    void setCore(CoreInterface* core)
    {
        this->_core = core;
    }

public: // Properties

    /**
     * Get property in variant form
     * @param name Name of the property
     * @param defaultValue Default value
     * @return Property in variant form
     */
    QVariant getProperty(const QString& name, const QVariant& defaultValue = QVariant()) const
    {
        if (!hasProperty(name))
            return defaultValue;

        return _properties[name];
    }

    /**
    * Set property
    * @param name Name of the property
    * @param value Property value
    */
    void setProperty(const QString& name, const QVariant& value)
    {
        _properties[name] = value;
    }

    /**
    * Determines whether a property with a give name exists
    * @param name Name of the property
    * @param value If property with the given name exists
    */
    bool hasProperty(const QString& name) const
    {
        return _properties.contains(name);
    }

    /** Returns a list of available property names */
    QStringList propertyNames() const
    {
        return _properties.keys();
    }

public: // GUI

    /**
     * Generates a context menu for display in other (view) plugins
     * @param kind Kind of plugin in which the context menu will be shown
     * @return Context menu
     */
    virtual QMenu* contextMenu(const QString& kind) { return nullptr; };

protected:
    CoreInterface* _core;

private:
    const QString               _name;
    const QString               _guiName;
    const QString               _kind;
    const Type                  _type;
    QMap<QString, QVariant>     _properties;    /** Properties map */

    /** Keeps track of how many instance have been created per plugin kind */
    static QMap<QString, std::int32_t> _noInstances;   
};

} // namespace plugin

} // namespace hdps

#endif // HDPS_PLUGIN_H