// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"
#include "WidgetActionContextMenu.h"
#include "WidgetActionMimeData.h"
#include "Application.h"
#include "AbstractActionsManager.h"

#include "util/Exception.h"

#include <QDebug>
#include <QMenu>
#include <QJsonArray>
#include <QDrag>
#include <QDialog>
#include <QDialogButtonBox>
#include <QEventLoop>

#ifdef _DEBUG
    #define WIDGET_ACTION_VERBOSE
#endif

using namespace hdps::util;

namespace hdps::gui {

QMap<WidgetAction::Scope, QString> WidgetAction::scopeNames {
    { WidgetAction::Scope::Private, "Private" },
    { WidgetAction::Scope::Public, "Public" }
};

WidgetAction::WidgetAction(QObject* parent, const QString& title) :
    QWidgetAction(parent),
    util::Serializable(),
    _defaultWidgetFlags(),
    _sortIndex(-1),
    _stretch(-1),
    _forceHidden(false),
    _forceDisabled(false),
    _connectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::PublishViaApi) | static_cast<std::int32_t>(ConnectionPermissionFlag::ConnectViaApi) | static_cast<std::int32_t>(ConnectionPermissionFlag::DisconnectViaApi)),
    _cachedConnectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::None)),
    _scope(Scope::Private),
    _publicAction(nullptr),
    _connectedActions(),
    _settingsPrefix(),
    _highlighting(HighlightOption::None),
    _popupSizeHint(),
    _configuration(static_cast<std::int32_t>(ConfigurationFlag::Default)),
    _location()
{
    Q_ASSERT(!title.isEmpty());

    setText(title);
    setSerializationName(title);

    updateLocation();

    if (core()->isInitialized())
    {
        actions().addAction(this);

        if (projects().hasProject())
            setStudioMode(projects().getCurrentProject()->getStudioModeAction().isChecked(), false);
    }
}

WidgetAction::~WidgetAction()
{
    if (!core()->isInitialized())
        return;

    actions().removeAction(this);
}

WidgetAction* WidgetAction::getParentAction() const
{
    return dynamic_cast<WidgetAction*>(this->parent());
}

WidgetActions WidgetAction::getParentActions() const
{
    WidgetActions parentActions;

    auto currentParent = dynamic_cast<WidgetAction*>(parent());

    while (currentParent) {
        parentActions << currentParent;

        currentParent = dynamic_cast<WidgetAction*>(currentParent->parent());
    }

    return parentActions;
}

WidgetActions WidgetAction::getChildren() const
{
    WidgetActions children;

    for (auto child : this->children()) {
        auto childAction = dynamic_cast<WidgetAction*>(child);

        if (childAction)
            children << childAction;
    }

    return children;
}

bool WidgetAction::isRoot() const
{
    if (!getParentAction())
        return true;

    return getParentAction()->getScope() != getScope();
}

bool WidgetAction::isLeaf() const
{
    return getChildren().isEmpty();
}

QWidget* WidgetAction::createWidget(QWidget* parent)
{
    if (parent != nullptr && dynamic_cast<WidgetActionCollapsedWidget::ToolButton*>(parent->parent()))
        return getWidget(parent, _defaultWidgetFlags | WidgetActionWidget::PopupLayout);

    return getWidget(parent, _defaultWidgetFlags);
}

QWidget* WidgetAction::createWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return getWidget(parent, widgetFlags);
}

std::int32_t WidgetAction::getSortIndex() const
{
    return _sortIndex;
}

void WidgetAction::setSortIndex(const std::int32_t& sortIndex)
{
    if (sortIndex == _sortIndex)
        return;

    _sortIndex = sortIndex;

    emit sortIndexChanged(_sortIndex);
}

QWidget* WidgetAction::createCollapsedWidget(QWidget* parent) const
{
    return new WidgetActionCollapsedWidget(parent, const_cast<WidgetAction*>(this));
}

QWidget* WidgetAction::createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags /*= 0x00001*/) const
{
    return new WidgetActionLabel(const_cast<WidgetAction*>(this), parent, widgetFlags);
}

QMenu* WidgetAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return new WidgetActionContextMenu(parent, { this });
}

std::int32_t WidgetAction::getDefaultWidgetFlags() const
{
    return _defaultWidgetFlags;
}

void WidgetAction::setDefaultWidgetFlags(const std::int32_t& widgetFlags)
{
    _defaultWidgetFlags = widgetFlags;
}

void WidgetAction::setDefaultWidgetFlag(const std::int32_t& widgetFlag, bool unset /*= false*/)
{
    if (unset)
        _defaultWidgetFlags = _defaultWidgetFlags & ~static_cast<std::int32_t>(widgetFlag);
    else
        _defaultWidgetFlags |= static_cast<std::int32_t>(widgetFlag);
}

WidgetAction::HighlightOption WidgetAction::getHighlighting() const
{
    return _highlighting;
}

bool WidgetAction::isHighlighted() const
{
    return (_highlighting == HighlightOption::Moderate) | (_highlighting == HighlightOption::Strong);
}

void WidgetAction::setHighlighting(const HighlightOption& highlighting)
{
    if (highlighting == _highlighting)
        return;

    _highlighting = highlighting;

    emit highlightingChanged(_highlighting);
}

void WidgetAction::setHighlighted(bool highlighted)
{
    setHighlighting(highlighted ? HighlightOption::Moderate : HighlightOption::None);
}

void WidgetAction::highlight()
{
    setHighlighted(true);
}

void WidgetAction::unHighlight()
{
    setHighlighted(false);
}

WidgetAction::Scope WidgetAction::getScope() const
{
    return _scope;
}

bool WidgetAction::isPrivate() const
{
    return _scope == Scope::Private;
}

bool WidgetAction::isPublic() const
{
    return _scope == Scope::Public;
}

void WidgetAction::makePublic(bool recursive /*= true*/)
{
    _scope = Scope::Public;

    emit scopeChanged(_scope);
    
    for (auto child : children()) {
        auto widgetAction = dynamic_cast<WidgetAction*>(child);

        if (widgetAction != nullptr)
            widgetAction->makePublic(recursive);
    }
}

bool WidgetAction::isPublished() const
{
    if (_publicAction == nullptr)
        return false;

    for (const auto connectedAction : _publicAction->getConnectedActions())
        if (connectedAction == this)
            return true;

    return false;
}

bool WidgetAction::isConnected() const
{
    return _publicAction != nullptr;
}

bool WidgetAction::publish(const QString& name /*= ""*/)
{
    return hdps::actions().publishPrivateAction(this, name);
}

void WidgetAction::connectToPublicAction(WidgetAction* publicAction, bool recursive)
{
    Q_ASSERT(publicAction != nullptr);

    if (publicAction == nullptr)
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    _publicAction = publicAction;

    actions().addPrivateActionToPublicAction(this, publicAction);

    emit isConnectedChanged(isConnected());
}

void WidgetAction::connectToPublicActionByName(const QString& publicActionName)
{
    try
    {
        Q_ASSERT(!publicActionName.isEmpty());

        if (publicActionName.isEmpty())
            throw std::runtime_error("Public action name is empty");

        ActionsListModel actionsListModel(this);

        auto action = actionsListModel.getAction(publicActionName);

        if (action == nullptr)
            throw std::runtime_error(QString("Public action %1 not found in the actions database").arg(publicActionName).toLatin1());

        if (!action->isPublic()) 
            throw std::runtime_error(QString("%1 is not public").arg(publicActionName).toLatin1());

        connectToPublicAction(action, true);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to connect to public action by name", e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to connect to public action by name");
    }
}

void WidgetAction::disconnectFromPublicAction(bool recursive)
{
    Q_ASSERT(_publicAction != nullptr);

    if (_publicAction == nullptr)
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << __FUNCTION__;
#endif

    actions().removePrivateActionFromPublicAction(this, _publicAction);

    disconnect(_publicAction, &QAction::changed, this, nullptr);

    setEnabled(true);

    _publicAction = nullptr;

    emit isConnectedChanged(isConnected());

    if (recursive) {
        for (auto child : children()) {
            auto action = dynamic_cast<WidgetAction*>(child);

            if (action == nullptr)
                continue;

            action->restoreConnectionPermissions(true);
        }
    }
}

WidgetAction* WidgetAction::getPublicAction()
{
    return _publicAction;
}

WidgetAction* WidgetAction::getPublicCopy() const
{
    try
    {
        auto publicCopy = static_cast<WidgetAction*>(metaObject()->newInstance(Q_ARG(QObject*, &hdps::actions()), Q_ARG(QString, text())));

        if (publicCopy == nullptr)
            throw std::runtime_error(QString("Unable to create new %1 instance using the Qt meta-object system.").arg(metaObject()->className()).toLatin1());

        publicCopy->makePublic();
        publicCopy->fromVariantMap(toVariantMap());
        publicCopy->makeUnique();

        emit publicCopy->idChanged(getId());

        return publicCopy;
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Get public copy failed", e);

        return nullptr;
    }
    catch (...)
    {
        exceptionMessageBox("Get public copy failed");

        return nullptr;
    }
}

const WidgetActions WidgetAction::getConnectedActions() const
{
    return _connectedActions;
}

WidgetActions& WidgetAction::getConnectedActions()
{
    return _connectedActions;
}

bool WidgetAction::mayPublish(ConnectionContextFlag connectionContextFlags) const
{
    if (_connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ForceNone))
        return false;

    switch (connectionContextFlags)
    {
        case WidgetAction::Api:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::PublishViaApi);

        case WidgetAction::Gui:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::PublishViaGui);

        case WidgetAction::ApiAndGui:
            break;

        default:
            break;
    }

    return false;
}

bool WidgetAction::mayConnect(ConnectionContextFlag connectionContextFlags) const
{
    if (_connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ForceNone))
        return false;

    switch (connectionContextFlags)
    {
        case WidgetAction::Api:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ConnectViaApi);

        case WidgetAction::Gui:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ConnectViaGui);

        case WidgetAction::ApiAndGui:
            break;

        default:
            break;
    }

    return false;
}

bool WidgetAction::mayDisconnect(ConnectionContextFlag connectionContextFlags) const
{
    if (_connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::ForceNone))
        return false;

    switch (connectionContextFlags)
    {
        case hdps::gui::WidgetAction::Api:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::DisconnectViaApi);

        case hdps::gui::WidgetAction::Gui:
            return _connectionPermissions & static_cast<std::int32_t>(ConnectionPermissionFlag::DisconnectViaGui);

        case hdps::gui::WidgetAction::ApiAndGui:
            break;

        default:
            break;
    }

    return false;
}

std::int32_t WidgetAction::getConnectionPermissions() const
{
    return _connectionPermissions;
}

bool WidgetAction::isConnectionPermissionFlagSet(ConnectionPermissionFlag connectionPermissionsFlag)
{
    return _connectionPermissions & static_cast<std::int32_t>(connectionPermissionsFlag);
}

void WidgetAction::setConnectionPermissionsFlag(ConnectionPermissionFlag connectionPermissionsFlag, bool unset /*= false*/, bool recursive /*= false*/)
{
    if (unset)
        _connectionPermissions = _connectionPermissions & ~static_cast<std::int32_t>(connectionPermissionsFlag);
    else
        _connectionPermissions |= static_cast<std::int32_t>(connectionPermissionsFlag);

    emit connectionPermissionsChanged(_connectionPermissions);

    if (recursive)
        for (auto childAction : getChildren())
            childAction->setConnectionPermissionsFlag(connectionPermissionsFlag, unset, recursive);
}

void WidgetAction::setConnectionPermissions(std::int32_t connectionPermissions, bool recursive /*= false*/)
{
    _connectionPermissions = connectionPermissions;

    emit connectionPermissionsChanged(_connectionPermissions);

    if (recursive)
        for (auto childAction : getChildren())
            childAction->setConnectionPermissions(connectionPermissions, recursive);
}

void WidgetAction::setConnectionPermissionsToNone(bool recursive /*= false*/)
{
    setConnectionPermissions(static_cast<std::int32_t>(ConnectionPermissionFlag::None), recursive);
}

void WidgetAction::setConnectionPermissionsToForceNone(bool recursive /*= false*/)
{
    setConnectionPermissionsFlag(ConnectionPermissionFlag::ForceNone, false, recursive);
}

void WidgetAction::setConnectionPermissionsToAll(bool recursive /*= false*/)
{
    setConnectionPermissionsFlag(ConnectionPermissionFlag::All, false, recursive);
}

void WidgetAction::cacheConnectionPermissions(bool recursive /*= false*/)
{
    _cachedConnectionPermissions = _connectionPermissions;

    if (recursive)
        for (auto childAction : getChildren())
            childAction->cacheConnectionPermissions(recursive);
}

void WidgetAction::restoreConnectionPermissions(bool recursive /*= false*/)
{
    setConnectionPermissions(_cachedConnectionPermissions);

    if (recursive)
        for (auto childAction : getChildren())
            childAction->restoreConnectionPermissions(recursive);
}

void WidgetAction::startDrag()
{
    if (!mayConnect(WidgetAction::Gui))
        return;

    auto drag       = new QDrag(this);
    auto mimeData   = new WidgetActionMimeData(this);

    drag->setMimeData(mimeData);
    drag->setPixmap(Application::getIconFont("FontAwesome").getIcon("link").pixmap(QSize(12, 12)));

    drag->exec();
}

void WidgetAction::setSettingsPrefix(const QString& settingsPrefix, const bool& load /*= true*/)
{
    _settingsPrefix = settingsPrefix;

    if (load)
        loadFromSettings();
}

void WidgetAction::setSettingsPrefix(plugin::Plugin* plugin, const QString& settingsPrefix, const bool& load /*= true*/)
{
    setSettingsPrefix(QString("Plugins/%1/%2").arg(plugin->getKind(), settingsPrefix), load);
}

QString WidgetAction::getSettingsPrefix() const
{
    return _settingsPrefix;
}

void WidgetAction::loadFromSettings()
{
    if (getSettingsPrefix().isEmpty())
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << QString("Load from settings: %1").arg(getSettingsPrefix());
#endif

    const auto settingsVariant = Application::current()->getSetting(_settingsPrefix);

    if (settingsVariant.isValid())
        fromVariantMap(settingsVariant.toMap());
}

void WidgetAction::saveToSettings()
{
    if (getSettingsPrefix().isEmpty())
        return;

#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << QString("Save to settings: %1").arg(getSettingsPrefix());
#endif

    Application::current()->setSetting(_settingsPrefix, toVariantMap());
}

QString WidgetAction::getLocation() const
{
    return _location;
}

void WidgetAction::updateLocation(bool recursive /*= true*/)
{
    const auto parentAction = getParentAction();
    const auto location     = parentAction ? QString("%1/%2").arg(parentAction->getLocation(), text()) : text();

    if (location == _location)
        return;

    _location = location;

    emit locationChanged(_location);

    if (recursive)
        for (const auto& child : getChildren())
            child->updateLocation(recursive);
}

QVector<WidgetAction*> WidgetAction::findChildren(const QString& searchString, bool recursive /*= true*/) const
{
    QVector<WidgetAction*> foundChildren;

    for (auto child : children()) {
        auto childWidgetAction = dynamic_cast<WidgetAction*>(child);

        if (!childWidgetAction)
            continue;

        if (searchString.isEmpty())
            foundChildren << childWidgetAction;
        else
            if (childWidgetAction->text().contains(searchString, Qt::CaseInsensitive))
                foundChildren << childWidgetAction;

        if (recursive)
            foundChildren << childWidgetAction->findChildren(searchString, recursive);
    }

    return foundChildren;
}

QSize WidgetAction::getPopupSizeHint() const
{
    return _popupSizeHint;
}

void WidgetAction::setPopupSizeHint(const QSize& popupSizeHint)
{
    _popupSizeHint = popupSizeHint;
}

QWidget* WidgetAction::getWidget(QWidget* parent, const std::int32_t& widgetFlags)
{
    return new QWidget();
}

void WidgetAction::fromVariantMap(const QVariantMap& variantMap)
{
    auto previousId = getId();

    Serializable::fromVariantMap(variantMap);

    variantMapMustContain(variantMap, "IsChecked");
    variantMapMustContain(variantMap, "SortIndex");
    variantMapMustContain(variantMap, "ConnectionPermissions");

    setChecked(variantMap["IsChecked"].toBool());
    setSortIndex(variantMap["SortIndex"].toInt());

    if (variantMap.contains("Stretch"))
        setStretch(variantMap["Stretch"].toInt());

    if (variantMap.contains("IsForceHidden"))
        setForceHidden(variantMap["IsForceHidden"].toInt());

    if (variantMap.contains("IsForceDisabled"))
        setForceHidden(variantMap["IsForceHidden"].toInt());

    setConnectionPermissions(variantMap["ConnectionPermissions"].toInt());
    cacheConnectionPermissions(true);

    if (variantMap.contains("PublicActionID")) {
        const auto publicActionId = variantMap["PublicActionID"].toString();

        if (!publicActionId.isEmpty()) {
            const auto publicAction = actions().getAction(publicActionId);

            if (publicAction)
                connectToPublicAction(publicAction, false);
        }
    }

    if (getId() != previousId)
        emit idChanged(getId());

    if (core()->isInitialized() && projects().hasProject())
        setStudioMode(projects().getCurrentProject()->getStudioModeAction().isChecked(), false);
}

QVariantMap WidgetAction::toVariantMap() const
{
    QVariantMap variantMap = Serializable::toVariantMap();

    variantMap.insert({
        { "ActionType", QVariant::fromValue(getTypeString()) },
        { "IsChecked", QVariant::fromValue(isChecked()) },
        { "SortIndex", QVariant::fromValue(_sortIndex) },
        { "ConnectionPermissions", QVariant::fromValue(_connectionPermissions) },
        { "IsPublic", QVariant::fromValue(isPublic()) },
        { "PublicActionID", QVariant::fromValue(_publicAction == nullptr ? "" : _publicAction->getId()) },
        { "IsForceHidden", QVariant::fromValue(getForceHidden()) },
        { "IsForceDisabled", QVariant::fromValue(getForceDisabled()) },
        { "Stretch", QVariant::fromValue(getStretch()) }
    });

    return variantMap;
}

bool WidgetAction::isResettable() const
{
    qDebug() << "isResettable() is not implemented in " << text();

    return false;
}

void WidgetAction::reset()
{
    qDebug() << "reset() is not implemented in " << text();
}

std::int32_t WidgetAction::getConfiguration() const
{
    return _configuration;
}

bool WidgetAction::isConfigurationFlagSet(ConfigurationFlag configurationFlag) const
{
    return _configuration & static_cast<std::int32_t>(configurationFlag);
}

void WidgetAction::setConfigurationFlag(ConfigurationFlag configurationFlag, bool unset /*= false*/, bool recursive /*= false*/)
{
    const auto flagSet = isConfigurationFlagSet(configurationFlag);

    if (unset)
        _configuration = _configuration & ~static_cast<std::int32_t>(configurationFlag);
    else
        _configuration |= static_cast<std::int32_t>(configurationFlag);

    emit configurationChanged(_configuration);

    if (recursive) {
        for (auto child : this->children()) {
            auto childAction = dynamic_cast<WidgetAction*>(child);

            if (childAction)
                childAction->setConfigurationFlag(configurationFlag, unset, recursive);
        }
    }

    if (flagSet != isConfigurationFlagSet(configurationFlag))
        emit configurationFlagToggled(configurationFlag, isConfigurationFlagSet(configurationFlag));
}

void WidgetAction::setConfiguration(std::int32_t configuration, bool recursive /*= false*/)
{
    _configuration = configuration;

    emit configurationChanged(_configuration);

    if (recursive) {
        for (auto childAction : getChildren())
            childAction->setConfiguration(configuration, recursive);
    }
}

QString WidgetAction::getTypeString(bool humanFriendly /*= false*/) const
{
    const auto className = QString(metaObject()->className());

    if (humanFriendly)
        return className.split("::").last().replace("Action", "");
    
    return className;
}

std::int32_t WidgetAction::getStretch() const
{
    return _stretch;
}

void WidgetAction::setStretch(const std::int32_t& stretch)
{
    if (stretch == _stretch)
        return;

    _stretch = stretch;

    emit stretchChanged(_stretch);
}

void WidgetAction::cacheState(const QString& name /*= "cache"*/)
{
#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name;
#endif

    _cachedStates[name] = toVariantMap();
}

void WidgetAction::restoreState(const QString& name /*= "cache"*/, bool remove /*= true*/)
{
#ifdef WIDGET_ACTION_VERBOSE
    qDebug() << __FUNCTION__ << name << remove;
#endif

    const auto exceptionMessage = QString("Unable to restore %1 state called %2").arg(text(), name);

    try
    {
        if (!_cachedStates.contains(name))
            throw std::runtime_error(QString("%1 does not exist").arg(name).toLatin1());

        fromVariantMap(_cachedStates[name].toMap());

        if (remove)
            _cachedStates.remove(name);
    }
    catch (std::exception& e)
    {
        exceptionMessageBox(exceptionMessage, e);
    }
    catch (...)
    {
        exceptionMessageBox(exceptionMessage);
    }
}

bool WidgetAction::getForceHidden() const
{
    return _forceHidden;
}

void WidgetAction::setForceHidden(bool forceHidden)
{
    if (forceHidden == _forceHidden)
        return;

    _forceHidden = forceHidden;

    emit visibleChanged();
    emit changed();
    emit forceHiddenChanged(_forceHidden);
}

bool WidgetAction::isVisible() const
{
    if (getForceHidden())
        return false;

    return QWidgetAction::isVisible();
}

bool WidgetAction::getForceDisabled() const
{
    return _forceDisabled;
}

void WidgetAction::setForceDisabled(bool forceDisabled)
{
    if (forceDisabled == _forceDisabled)
        return;

    _forceDisabled = forceDisabled;

    emit enabledChanged(isEnabled());
    emit changed();
    emit forceDisabledChanged(_forceDisabled);
}

bool WidgetAction::isEnabled() const
{
    if (getForceDisabled())
        return false;

    return QWidgetAction::isEnabled();
}

void WidgetAction::setText(const QString& text)
{
    if (text == this->text())
        return;

    if (text.isEmpty())
        throw std::runtime_error("Setting widget action text to an empty string!");

    QWidgetAction::setText(text);

    emit textChanged(this->text());

    updateLocation();
}

bool WidgetAction::mayConnectToPublicAction(const WidgetAction* publicAction) const
{
    return true;
}

void WidgetAction::setStudioMode(bool studioMode, bool recursive /*= true*/)
{
    if (studioMode) {
        cacheConnectionPermissions(recursive);
        setConnectionPermissionsToAll(recursive);
    }
    else {
        restoreConnectionPermissions(recursive);
    }
}

}


