#include "Plugin.h"

#include "Application.h"

namespace hdps
{

namespace plugin
{

QMap<QString, std::int32_t> hdps::plugin::Plugin::_noInstances = QMap<QString, std::int32_t>();

Plugin::Plugin(Type type, QString kind) :
    _name(kind + QUuid::createUuid().toString()),
    _guiName(QString("%1 %2").arg(kind, QString::number(_noInstances[kind] + 1))),
    _kind(kind),
    _type(type),
    _properties(),
    _widgetActions()
{
    _noInstances[kind]++;
}

QVariant Plugin::getSetting(const QString& path, const QVariant& defaultValue /*= QVariant()*/) const
{
    return Application::current()->getSetting(QString("%1/%2").arg(_kind, path), defaultValue);
}

void Plugin::setSetting(const QString& path, const QVariant& value)
{
    Application::current()->setSetting(QString("%1/%2").arg(_kind, path), value);
}

void Plugin::addActionWidget(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _widgetActions.append(widgetAction);
}

void Plugin::removeActionWidget(gui::WidgetAction* widgetAction)
{
    Q_ASSERT(widgetAction != nullptr);

    _widgetActions.removeOne(widgetAction);
}

const hdps::plugin::Plugin::QActionList& Plugin::getActionWidgets() const
{
    return _widgetActions;
}

}
}
