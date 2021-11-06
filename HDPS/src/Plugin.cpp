#include "Plugin.h"

#include "Application.h"

namespace hdps
{

namespace plugin
{

QMap<QString, std::int32_t> hdps::plugin::Plugin::_noInstances = QMap<QString, std::int32_t>();

Plugin::Plugin(const PluginFactory* factory) :
    _factory(factory),
    EventListener(),
    _name(getKind() + QUuid::createUuid().toString()),
    _guiName(QString("%1 %2").arg(getKind(), QString::number(_noInstances[getKind()] + 1))),
    _properties()
{
    _noInstances[getKind()]++;
}

QVariant Plugin::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return Application::current()->getSetting(QString("%1/%2").arg(getKind(), path), defaultValue);
}

void Plugin::setSetting(const QString& path, const QVariant& value)
{
    Application::current()->setSetting(QString("%1/%2").arg(getKind(), path), value);
}

}
}
