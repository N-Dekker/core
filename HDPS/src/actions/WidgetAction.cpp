#include "WidgetAction.h"
#include "WidgetActionLabel.h"
#include "WidgetActionCollapsedWidget.h"
#include "WidgetActionContextMenu.h"
#include "Application.h"
#include "DataHierarchyItem.h"
#include "Plugin.h"
#include "util/Exception.h"

#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QJsonArray>
#include <QDialog>
#include <QDialogButtonBox>

#ifdef _DEBUG
    #define WIDGET_ACTION_VERBOSE
#endif

namespace hdps {

namespace gui {

WidgetAction::WidgetAction(QObject* parent /*= nullptr*/) :
    QWidgetAction(parent),
    _defaultWidgetFlags(),
    _sortIndex(-1),
    _connectionPermissions(ConnectionPermissionFlag::Default),
    _publicAction(nullptr),
    _connectedActions(),
    _settingsPrefix(),
    _highlighted(false),
    _popupSizeHint(QSize(0, 0))
{
}

WidgetAction::~WidgetAction()
{
}

QString WidgetAction::getTypeString() const
{
    return "";
}

WidgetAction* WidgetAction::getParentWidgetAction()
{
    return dynamic_cast<WidgetAction*>(this->parent());
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
    _sortIndex = sortIndex;
}

QWidget* WidgetAction::createCollapsedWidget(QWidget* parent)
{
    return new WidgetActionCollapsedWidget(parent, this);
}

QWidget* WidgetAction::createLabelWidget(QWidget* parent, const std::int32_t& widgetFlags /*= 0x00001*/)
{
    return new WidgetActionLabel(this, parent, widgetFlags);
}

QMenu* WidgetAction::getContextMenu(QWidget* parent /*= nullptr*/)
{
    return new WidgetActionContextMenu(parent, this);
}

std::int32_t WidgetAction::getDefaultWidgetFlags() const
{
    return _defaultWidgetFlags;
}

void WidgetAction::setDefaultWidgetFlags(const std::int32_t& widgetFlags)
{
    _defaultWidgetFlags = widgetFlags;
}

void WidgetAction::setHighlighted(bool highlighted)
{
    if (highlighted == _highlighted)
        return;

    _highlighted = highlighted;

    emit highlightedChanged(_highlighted);
}

bool WidgetAction::isHighlighted() const
{
    return _highlighted;
}

bool WidgetAction::isPublic() const
{
    return Application::getActionsManager().isActionPublic(this);
}

bool WidgetAction::isPublished() const
{
    return Application::getActionsManager().isActionPublished(this);
}

bool WidgetAction::isConnected() const
{
    return Application::getActionsManager().isActionConnected(this);
}

void WidgetAction::publish(const QString& name /*= ""*/)
{
    try
    {
        if (name.isEmpty()) {
            auto& fontAwesome = Application::getIconFont("FontAwesome");

            QDialog publishDialog;

            publishDialog.setWindowIcon(fontAwesome.getIcon("cloud-upload-alt"));
            publishDialog.setWindowTitle("Publish " + text() + " parameter");

            auto mainLayout         = new QVBoxLayout();
            auto parameterLayout    = new QHBoxLayout();
            auto label              = new QLabel("Name:");
            auto lineEdit           = new QLineEdit(text());

            parameterLayout->addWidget(label);
            parameterLayout->addWidget(lineEdit);

            mainLayout->addLayout(parameterLayout);

            auto dialogButtonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

            dialogButtonBox->button(QDialogButtonBox::Ok)->setText("Publish");
            dialogButtonBox->button(QDialogButtonBox::Ok)->setToolTip("Publish the parameter");
            dialogButtonBox->button(QDialogButtonBox::Cancel)->setToolTip("Cancel publishing");

            connect(dialogButtonBox->button(QDialogButtonBox::Ok), &QPushButton::clicked, &publishDialog, &QDialog::accept);
            connect(dialogButtonBox->button(QDialogButtonBox::Cancel), &QPushButton::clicked, &publishDialog, &QDialog::reject);

            mainLayout->addWidget(dialogButtonBox);

            publishDialog.setLayout(mainLayout);
            publishDialog.setFixedWidth(300);

            const auto updateOkButtonReadOnly = [dialogButtonBox, lineEdit]() -> void {
                dialogButtonBox->button(QDialogButtonBox::Ok)->setEnabled(!lineEdit->text().isEmpty());
            };

            connect(lineEdit, &QLineEdit::textChanged, this, updateOkButtonReadOnly);

            updateOkButtonReadOnly();

            if (publishDialog.exec() == QDialog::Accepted)
                publish(lineEdit->text());
        }
        else {
            if (Application::getActionsManager().isActionPublished(this))
                throw std::runtime_error("Action is already published");

            auto publicCopy = getPublicCopy();

            if (publicCopy == nullptr)
                throw std::runtime_error("Public copy not created");

            publicCopy->setText(name);

            connectToPublicAction(publicCopy);

            Application::getActionsManager().addAction(publicCopy);

            emit isPublishedChanged(Application::getActionsManager().isActionPublished(this));
            emit isConnectedChanged(Application::getActionsManager().isActionConnected(this));
        }
    }
    catch (std::exception& e)
    {
        exceptionMessageBox("Unable to publish " + text(), e);
    }
    catch (...)
    {
        exceptionMessageBox("Unable to publish " + text());
    }
}

void WidgetAction::connectToPublicAction(WidgetAction* publicAction)
{
    Q_ASSERT(publicAction != nullptr);

    _publicAction = publicAction;

    _publicAction->connectPrivateAction(this);

    emit isConnectedChanged(Application::getActionsManager().isActionConnected(this));
}

void WidgetAction::disconnectFromPublicAction()
{
    Q_ASSERT(_publicAction != nullptr);

    _publicAction->disconnectPrivateAction(this);

    emit isConnectedChanged(Application::getActionsManager().isActionConnected(this));
}

void WidgetAction::connectPrivateAction(WidgetAction* privateAction)
{
    Q_ASSERT(privateAction != nullptr);

    _connectedActions << privateAction;

    emit actionConnected(privateAction);

    connect(privateAction, &WidgetAction::destroyed, this, [this, privateAction]() -> void {
        disconnectPrivateAction(privateAction);
    });
}

void WidgetAction::disconnectPrivateAction(WidgetAction* privateAction)
{
    Q_ASSERT(privateAction != nullptr);

    _connectedActions.removeOne(privateAction);

    emit actionDisconnected(privateAction);
}

WidgetAction* WidgetAction::getPublicAction()
{
    return _publicAction;
}

const QVector<WidgetAction*> WidgetAction::getConnectedActions() const
{
    return _connectedActions;
}

WidgetAction* WidgetAction::getPublicCopy() const
{
    return nullptr;
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

    fromVariantMap(Application::current()->getSetting(_settingsPrefix).toMap());
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

QString WidgetAction::getSettingsPath() const
{
    QStringList actionPath;

    auto currentParent = dynamic_cast<WidgetAction*>(parent());

    const auto getPathName = [](const WidgetAction* widgetAction) -> QString {
        if (!widgetAction->objectName().isEmpty())
            return widgetAction->objectName();

        return widgetAction->text();
    };

    actionPath << getPathName(this);

    while (currentParent) {
        actionPath.insert(actionPath.begin(), getPathName(currentParent));

        currentParent = dynamic_cast<WidgetAction*>(currentParent->parent());
    }

    return actionPath.join("/");
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

}
}
