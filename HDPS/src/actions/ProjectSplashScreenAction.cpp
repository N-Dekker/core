#include "ProjectSplashScreenAction.h"
#include "Project.h"

#include <QToolButton>
#include <QTimer>
#include <QMainWindow>
#include <QPainter>

#include <algorithm>

namespace hdps::gui {

ProjectSplashScreenAction::ProjectSplashScreenAction(QObject* parent, const Project& project) :
    HorizontalGroupAction(parent, "Splash Screen"),
    _project(project),
    _enabledAction(this, "Splash screen post-load"),
    _closeManuallyAction(this, "Close manually"),
    _durationAction(this, "Duration", 1000, 10000, 10000),
    _animationDurationAction(this, "Duration", 10, 10000, 250),
    _animationPanAmountAction(this, "Pan Amount", 10, 100, 20),
    _backgroundColorAction(this, "Background Color", Qt::white),
    _editAction(this, "Edit"),
    _showSplashScreenAction(this, "Show"),
    _splashScreenDialog(*this),
    _projectImageAction(this, "Primary Image")
{
    setSerializationName("Splash Screen");
    setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);

    _enabledAction.setStretch(1);
    _enabledAction.setToolTip("Show splash screen at startup");
    _enabledAction.setSerializationName("Enabled");

    _closeManuallyAction.setToolTip("Close manually");
    _closeManuallyAction.setSerializationName("CloseManually");

    _durationAction.setToolTip("Duration of the splash screen in milliseconds");
    _durationAction.setSuffix("ms");
    _durationAction.setSerializationName("Duration");

    _animationDurationAction.setToolTip("Duration of the splash screen animations in milliseconds");
    _animationDurationAction.setSuffix("ms");
    _animationDurationAction.setSerializationName("AnimationDuration");

    _animationPanAmountAction.setToolTip("Amount of panning up/down during animation in pixels");
    _animationPanAmountAction.setSuffix("px");
    _animationPanAmountAction.setSerializationName("Pan Amount");

    auto& fontAwesome = Application::getIconFont("FontAwesome");

    _editAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::AlwaysCollapsed);
    _editAction.setConfigurationFlag(WidgetAction::ConfigurationFlag::NoLabelInGroup);
    _editAction.setIcon(fontAwesome.getIcon("cog"));
    _editAction.setToolTip("Edit the splash screen settings");
    _editAction.setPopupSizeHint(QSize(350, 0));

    _editAction << _durationAction;
    _editAction << _closeManuallyAction;
    _editAction << _backgroundColorAction;
    _editAction << _projectImageAction;

    _showSplashScreenAction.setDefaultWidgetFlags(TriggerAction::Icon);
    _showSplashScreenAction.setIcon(fontAwesome.getIcon("eye"));
    _showSplashScreenAction.setToolTip("Preview the splash screen");

    _projectImageAction.setDefaultWidgetFlags(ImageAction::Loader);
    _projectImageAction.setIcon(fontAwesome.getIcon("image"));
    _projectImageAction.setToolTip("Primary image");
    _projectImageAction.setSerializationName("PrimaryImage");

    addAction(&_enabledAction);
    addAction(&_editAction);
    addAction(&_showSplashScreenAction);

    const auto updateDurationAction = [this]() -> void {
        _durationAction.setEnabled(!_closeManuallyAction.isChecked());
    };

    updateDurationAction();

    connect(&_showSplashScreenAction, &TriggerAction::triggered, this, [this]() -> void {
        _splashScreenDialog.open();
    });

    connect(&_closeManuallyAction, &ToggleAction::toggled, this, updateDurationAction);
}

QString ProjectSplashScreenAction::getTypeString() const
{
    return "ProjectSplashScreen";
}

const Project& ProjectSplashScreenAction::getProject() const
{
    return _project;
}

void ProjectSplashScreenAction::fromVariantMap(const QVariantMap& variantMap)
{
    Serializable::fromVariantMap(variantMap);

    _enabledAction.fromParentVariantMap(variantMap);
    _closeManuallyAction.fromParentVariantMap(variantMap);
    _durationAction.fromParentVariantMap(variantMap);
    _animationDurationAction.fromParentVariantMap(variantMap);
    _animationPanAmountAction.fromParentVariantMap(variantMap);
    _backgroundColorAction.fromParentVariantMap(variantMap);
    _projectImageAction.fromParentVariantMap(variantMap);
}

QVariantMap ProjectSplashScreenAction::toVariantMap() const
{
    auto variantMap = Serializable::toVariantMap();

    _enabledAction.insertIntoVariantMap(variantMap);
    _closeManuallyAction.insertIntoVariantMap(variantMap);
    _durationAction.insertIntoVariantMap(variantMap);
    _animationDurationAction.insertIntoVariantMap(variantMap);
    _animationPanAmountAction.insertIntoVariantMap(variantMap);
    _backgroundColorAction.insertIntoVariantMap(variantMap);
    _projectImageAction.insertIntoVariantMap(variantMap);

    return variantMap;
}

ProjectSplashScreenAction::Dialog::Dialog(ProjectSplashScreenAction& projectSplashScreenAction, QWidget* parent /*= nullptr*/) :
    QDialog(parent),
    _projectSplashScreenAction(projectSplashScreenAction),
    _opacityAnimation(this, "windowOpacity", this),
    _positionAnimation(this, "pos", this),
    _backgroundImage(":/Images/StartPageBackground"),
    _closeToolButton(),
    _animationState(AnimationState::Idle)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setWindowFlags(Qt::Window | Qt::Popup);
    setAttribute(Qt::WA_StyledBackground);
    setFixedSize(QSize(640, 480));

    const auto updateStyleSheet = [this]() -> void {
        setStyleSheet(QString("QDialog { \
            background-color: %1; \
            border: 1px solid rgb(50, 50, 50); \
         }").arg(_projectSplashScreenAction.getBackgroundColorAction().getColor().name(QColor::HexArgb)));
    };
    
    updateStyleSheet();

    connect(&_projectSplashScreenAction.getBackgroundColorAction(), &ColorAction::colorChanged, this, updateStyleSheet);

    const auto updateDuration = [this]() -> void {
        const auto animationDuration = std::min(_projectSplashScreenAction.getAnimationDurationAction().getValue(), _projectSplashScreenAction.getDurationAction().getValue() / 2);

        _opacityAnimation.setDuration(animationDuration);
        _positionAnimation.setDuration(animationDuration);
    };
    
    updateDuration();

    connect(&_projectSplashScreenAction.getDurationAction(), &IntegralAction::valueChanged, this, updateDuration);
    connect(&_projectSplashScreenAction.getAnimationDurationAction(), &IntegralAction::valueChanged, this, updateDuration);

    connect(&_opacityAnimation, &QPropertyAnimation::finished, this, [this]() -> void {
        if (_opacityAnimation.currentValue().toFloat() == 1.0f) {
            setEnabled(true);
        }
        else {
            accept();

            _projectSplashScreenAction.getShowSplashScreenAction().setEnabled(true);
        }

        _animationState = AnimationState::Idle;
    });

    auto& project = const_cast<Project&>(_projectSplashScreenAction.getProject());

    auto mainLayout = new QGridLayout();

    const auto margin = 5;

    mainLayout->setContentsMargins(margin + 20, margin, margin, margin);

    _closeToolButton.setIcon(Application::getIconFont("FontAwesome").getIcon("times"));
    _closeToolButton.setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    _closeToolButton.setAutoRaise(true);
    _closeToolButton.setToolTip("Close the splash screen");

    connect(&_closeToolButton, &QToolButton::clicked, this, &Dialog::fadeOut);

    mainLayout->addWidget(&_closeToolButton, 0, 2, Qt::AlignRight);
    mainLayout->setSpacing(20);

    auto imageLabel = new QLabel();

    imageLabel->setFixedSize(200, 200);
    imageLabel->setScaledContents(true);

    const auto updatePrimaryImage = [this, imageLabel]() -> void {
        imageLabel->setPixmap(QPixmap::fromImage(_projectSplashScreenAction.getPrimaryImageAction().getImage()));
    };

    connect(&_projectSplashScreenAction.getPrimaryImageAction(), &ImageAction::imageChanged, this, updatePrimaryImage);

    mainLayout->addWidget(imageLabel, 1, 0);

    auto htmlLabel = new QLabel();
    
    htmlLabel->setWordWrap(true);
    htmlLabel->setTextFormat(Qt::RichText);
    htmlLabel->setOpenExternalLinks(true);

    mainLayout->addWidget(htmlLabel, 1, 2);

    const auto updateText = [this, &project, htmlLabel]() -> void {
        auto& versionAction = project.getProjectVersionAction();

        const auto title        = project.getTitleAction().getString();
        const auto version      = QString("%1.%2 <i>%3</i>").arg(QString::number(versionAction.getMajorAction().getValue()), QString::number(versionAction.getMinorAction().getValue()), versionAction.getSuffixAction().getString().toLower());
        const auto description  = project.getDescriptionAction().getString();
        const auto comments     = project.getCommentsAction().getString();

        htmlLabel->setText(QString(" \
            <p style='font-size: 20pt; font-weight: bold;'>%1</p> \
            <p>Version: %2</p> \
            <p>%3</p> \
            <p>%4</p> \
        ").arg(title, version, description, comments));
    };

    updateText();

    connect(&project.getTitleAction(), &StringAction::stringChanged, this, updateText);
    connect(&project.getProjectVersionAction().getVersionStringAction(), &StringAction::stringChanged, this, updateText);
    connect(&project.getDescriptionAction(), &StringAction::stringChanged, this, updateText);
    connect(&project.getCommentsAction(), &StringAction::stringChanged, this, updateText);

    //mainLayout->setColumnStretch(2, 1);
    mainLayout->setRowStretch(2, 1);

    setLayout(mainLayout);
}

void ProjectSplashScreenAction::Dialog::open()
{
    _projectSplashScreenAction.getShowSplashScreenAction().setEnabled(false);
    _closeToolButton.lower();

    if (!_projectSplashScreenAction.getCloseManuallyAction().isChecked()) {
        QTimer::singleShot(_projectSplashScreenAction.getDurationAction().getValue() - _projectSplashScreenAction.getAnimationDurationAction().getValue(), this, [this]() -> void {
            if (_opacityAnimation.currentValue().toFloat() == 1.0f)
                fadeOut();
        });
    }
    
    fadeIn();

    QDialog::open();
}

void ProjectSplashScreenAction::Dialog::fadeIn()
{
    if (_animationState == AnimationState::FadingIn)
        return;

    _animationState = AnimationState::FadingIn;

    setEnabled(false);

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();
    
    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(_opacityAnimation.currentValue().toFloat());
    _opacityAnimation.setEndValue(1.0);
    _opacityAnimation.start();

    if (_positionAnimation.state() == QPropertyAnimation::Running)
        _positionAnimation.stop();

    const auto position = getMainWindow()->frameGeometry().topLeft() + getMainWindow()->rect().center() - rect().center();

    _positionAnimation.setEasingCurve(QEasingCurve::OutQuad);
    _positionAnimation.setStartValue(position - QPoint(0, _projectSplashScreenAction.getAnimationPanAmountAction().getValue()));
    _positionAnimation.setEndValue(position);
    _positionAnimation.start();
}

void ProjectSplashScreenAction::Dialog::fadeOut()
{
    if (_animationState == AnimationState::FadingOut)
        return;

    _animationState = AnimationState::FadingOut;

#ifdef _DEBUG
    qDebug() << __FUNCTION__;
#endif

    if (_opacityAnimation.state() == QPropertyAnimation::Running)
        _opacityAnimation.stop();

    _opacityAnimation.setEasingCurve(QEasingCurve::InQuad);
    _opacityAnimation.setStartValue(_opacityAnimation.currentValue().toFloat());
    _opacityAnimation.setEndValue(0.0);
    _opacityAnimation.start();

    if (_positionAnimation.state() == QPropertyAnimation::Running)
        _positionAnimation.stop();

    const auto position = getMainWindow()->frameGeometry().topLeft() + getMainWindow()->rect().center() - rect().center();

    _positionAnimation.setEasingCurve(QEasingCurve::InQuad);
    _positionAnimation.setStartValue(_positionAnimation.currentValue().toPoint());
    _positionAnimation.setEndValue(position + QPoint(0, _projectSplashScreenAction.getAnimationPanAmountAction().getValue()));
    _positionAnimation.start();
}

QMainWindow* ProjectSplashScreenAction::Dialog::getMainWindow() const
{
    for (auto topLevelWidget : qApp->topLevelWidgets()) {
        auto mainWindow = qobject_cast<QMainWindow*>(topLevelWidget);
        
        if (mainWindow)
            return mainWindow;
    }
    
    return nullptr;
}

void ProjectSplashScreenAction::Dialog::paintEvent(QPaintEvent* paintEvent)
{
    QPainter painter(this);

    painter.setBackground(QBrush(_projectSplashScreenAction.getBackgroundColorAction().getColor()));

    auto centerOfWidget     = rect().center();
    auto pixmapRectangle    = _backgroundImage.rect();

    pixmapRectangle.moveCenter(centerOfWidget - QPoint(200, 0));

    painter.setOpacity(0.5);
    painter.drawPixmap(pixmapRectangle.topLeft(), _backgroundImage);
}

}