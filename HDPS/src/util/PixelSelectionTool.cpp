#include "PixelSelectionTool.h"
#include "Application.h"

#include <QDebug>
#include <QEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

namespace hdps {

namespace util {

PixelSelectionTool::PixelSelectionTool(QWidget* targetWidget, const bool& enabled /*= true*/) :
    QObject(targetWidget),
    _enabled(enabled),
    _type(PixelSelectionType::Rectangle),
    _modifier(PixelSelectionModifierType::Replace),
    _active(false),
    _notifyDuringSelection(true),
    _brushRadius(BRUSH_RADIUS_DEFAULT),
    _mousePosition(),
    _mousePositions(),
    _mouseButtons(),
    _shapePixmap(),
    _areaPixmap(),
    _preventContextMenu(false),
    _mainColor(),
    _fillColor(),
    _areaBrush(),
    _penLineForeGround(),
    _penLineBackGround(),
    _penControlPoint()
{
    setMainColor(QColor(Qt::black));

    // Tap into events from the target widget (necessary for drawing)
    targetWidget->installEventFilter(this);
}

bool PixelSelectionTool::isEnabled() const
{
    return _enabled;
}

void PixelSelectionTool::setEnabled(const bool& enabled)
{
    _enabled = enabled;

    if (!_enabled) {
        _shapePixmap.fill(Qt::transparent);
        _areaPixmap.fill(Qt::transparent);
    }

    endSelection();
}

PixelSelectionType PixelSelectionTool::getType() const
{
    return _type;
}

void PixelSelectionTool::setType(const PixelSelectionType& type)
{
    if (type == _type)
        return;

    _type = type;

    emit typeChanged(_type);
    
    abort();
    paint();
}

PixelSelectionModifierType PixelSelectionTool::getModifier() const
{
    return _modifier;
}

void PixelSelectionTool::setModifier(const PixelSelectionModifierType& modifier)
{
    if (modifier == _modifier)
        return;

    _modifier = modifier;

    emit modifierChanged(_modifier);
    
    paint();
}

bool PixelSelectionTool::isNotifyDuringSelection() const
{
    return _notifyDuringSelection;
}

void PixelSelectionTool::setNotifyDuringSelection(const bool& notifyDuringSelection)
{
    if (notifyDuringSelection == _notifyDuringSelection)
        return;

    _notifyDuringSelection = notifyDuringSelection;

    emit notifyDuringSelectionChanged(_notifyDuringSelection);
}

float PixelSelectionTool::getBrushRadius() const
{
    return _brushRadius;
}

void PixelSelectionTool::setBrushRadius(const float& brushRadius)
{
    if (brushRadius == _brushRadius)
        return;

    _brushRadius = std::max(std::min(brushRadius, BRUSH_RADIUS_MAX), BRUSH_RADIUS_MIN);

    emit brushRadiusChanged(_brushRadius);
    
    paint();
}

QColor PixelSelectionTool::getMainColor() const
{
    return _mainColor;
}

void PixelSelectionTool::setMainColor(const QColor& mainColor)
{
    _mainColor          = mainColor;
    _fillColor          = QColor(_mainColor.red(), _mainColor.green(), _mainColor.blue(), 50);
    _areaBrush          = QBrush(_fillColor);
    _penLineForeGround  = QPen(_mainColor, 1.7f, Qt::SolidLine);
    _penLineBackGround  = QPen(QColor(_mainColor.red(), _mainColor.green(), _mainColor.blue(), 140), 1.7f, Qt::DotLine);
    _penControlPoint    = QPen(_mainColor, 7.0f, Qt::SolidLine, Qt::RoundCap);
}

void PixelSelectionTool::setChanged()
{
    emit typeChanged(_type);
    emit modifierChanged(_modifier);
    emit notifyDuringSelectionChanged(_notifyDuringSelection);
    emit brushRadiusChanged(_brushRadius);
}

void PixelSelectionTool::abort()
{
    endSelection();
}

void PixelSelectionTool::update()
{
    paint();
}

bool PixelSelectionTool::eventFilter(QObject* target, QEvent* event)
{
    auto shouldPaint = false;

    switch (event->type())
    {
        // Prevent recursive paint events
        case QEvent::Paint:
            return false;

        case QEvent::ContextMenu:
        {
            // Prevent context menu when selection is ended in polygon mode (by right-clicking)
            if (_preventContextMenu) {
                _preventContextMenu = false;
                return true;
            }

            break;
        }

        case QEvent::KeyPress:
        {
            // Get key that was pressed
            auto keyEvent = static_cast<QKeyEvent*>(event);
            
            // Do not handle repeating keys
            if (!keyEvent->isAutoRepeat()) {
                // Start navigating when the space key is pressed
                if (keyEvent->key() == Qt::Key_Escape && _type == PixelSelectionType::Polygon) {
                    endSelection();
                    shouldPaint = true;
                }
            }

            break;
        }

        case QEvent::Resize:
        {
            const auto resizeEvent = static_cast<QResizeEvent*>(event);

            _shapePixmap    = QPixmap(resizeEvent->size());
            _areaPixmap     = QPixmap(resizeEvent->size());

            _shapePixmap.fill(Qt::transparent);
            _areaPixmap.fill(Qt::transparent);

            shouldPaint = true;

            break;
        }
        
        case QEvent::MouseButtonPress:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_type)
            {
                case PixelSelectionType::Rectangle:
                case PixelSelectionType::Brush:
                case PixelSelectionType::Lasso:
                case PixelSelectionType::Sample:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            startSelection();
                            _mousePositions.clear();
                            _mousePositions << mouseEvent->pos();
                            break;

                        case Qt::RightButton:
                            break;

                        default:
                            break;
                    }
                    
                    break;
                }

                case PixelSelectionType::Polygon:
                {
                    if (_mousePositions.isEmpty() && mouseEvent->button() == Qt::LeftButton)
                        startSelection();

                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            _mousePositions << mouseEvent->pos() << mouseEvent->pos();
                            break;

                        case Qt::RightButton:
                            _mousePositions << mouseEvent->pos();
                            break;

                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            shouldPaint = true;

            break;
        }

        case QEvent::MouseButtonRelease:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mouseButtons = mouseEvent->buttons();

            switch (_type)
            {
                case PixelSelectionType::Rectangle:
                case PixelSelectionType::Brush:
                case PixelSelectionType::Lasso:
                case PixelSelectionType::Sample:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            endSelection();
                            break;

                        case Qt::RightButton:
                            break;

                        default:
                            break;
                    }

                    break;
                }

                case PixelSelectionType::Polygon:
                {
                    switch (mouseEvent->button())
                    {
                        case Qt::LeftButton:
                            break;

                        case Qt::RightButton:
                        {
                            _preventContextMenu = true;
                            endSelection();
                            break;
                        }
 
                        default:
                            break;
                    }

                    break;
                }

                default:
                    break;
            }

            shouldPaint = true;

            break;
        }

        case QEvent::MouseMove:
        {
            auto mouseEvent = static_cast<QMouseEvent*>(event);

            _mousePosition = mouseEvent->pos();

            switch (_type)
            {
                case PixelSelectionType::Rectangle:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton) {
                        if (_mousePositions.size() == 1) {
                            _mousePositions << _mousePosition;
                        } else {
                            if (_mousePositions.isEmpty()) {
                                _mousePositions << _mousePosition;
                            }
                            else {
                                _mousePositions.last() = _mousePosition;
                            }
                        }
                    }

                    if (isActive())
                        shouldPaint = true;

                    break;
                }

                case PixelSelectionType::Brush:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton)
                        _mousePositions << _mousePosition;

                    shouldPaint = true;

                    break;
                }

                case PixelSelectionType::Lasso:
                {
                    if (mouseEvent->buttons() & Qt::LeftButton && !_mousePositions.isEmpty())
                        _mousePositions << mouseEvent->pos();

                    if (isActive())
                        shouldPaint = true;

                    break;
                }

                case PixelSelectionType::Polygon:
                {
                    if (!_mousePositions.isEmpty())
                        _mousePositions.last() = mouseEvent->pos();

                    if (isActive())
                        shouldPaint = true;

                    break;
                }

                case PixelSelectionType::Sample:
                {
                    _mousePositions = { _mousePosition };

                    shouldPaint = true;

                    break;
                }

                default:
                    break;
            }

            break;
        }

        case QEvent::Wheel:
        {
            auto wheelEvent = static_cast<QWheelEvent*>(event);

            switch (_type)
            {
                case PixelSelectionType::Rectangle:
                    break;

                case PixelSelectionType::Brush:
                {
                    if (wheelEvent->angleDelta().y() < 0)
                        setBrushRadius(_brushRadius - BRUSH_RADIUS_DELTA);
                    else
                        setBrushRadius(_brushRadius + BRUSH_RADIUS_DELTA);

                    shouldPaint = true;

                    break;
                }

                case PixelSelectionType::Lasso:
                case PixelSelectionType::Polygon:
                    break;

                default:
                    break;
            }

            break;
        }

        default:
            break;
    }

    if (shouldPaint)
        paint();

    return QObject::eventFilter(target, event);
}

void PixelSelectionTool::paint()
{
    if (_type != PixelSelectionType::ROI && !_enabled)
        return;

    _shapePixmap.fill(Qt::transparent);
    _areaPixmap.fill(Qt::transparent);

    QPainter shapePainter(&_shapePixmap), areaPainter(&_areaPixmap);

    shapePainter.setRenderHint(QPainter::Antialiasing);
    areaPainter.setRenderHint(QPainter::Antialiasing);

    shapePainter.setFont(QFont("Font Awesome 5 Free Solid", 9));

    auto textRectangle = QRectF();

    QVector<QPoint> controlPoints;

    const auto noMousePositions = _mousePositions.size();

    switch (_type)
    {
        case PixelSelectionType::Rectangle:
        {
            if (noMousePositions != 2)
                break;

            const auto topLeft      = QPointF(std::min(_mousePositions.first().x(), _mousePositions.last().x()), std::min(_mousePositions.first().y(), _mousePositions.last().y()));
            const auto bottomRight  = QPointF(std::max(_mousePositions.first().x(), _mousePositions.last().x()), std::max(_mousePositions.first().y(), _mousePositions.last().y()));
            const auto rectangle    = QRectF(topLeft, bottomRight);

            controlPoints << _mousePositions.first();
            controlPoints << _mousePositions.last();
                
            areaPainter.setBrush(_areaBrush);
            areaPainter.setPen(Qt::NoPen);
            areaPainter.drawRect(rectangle);

            shapePainter.setPen(_penLineForeGround);
            shapePainter.drawRect(rectangle);

            const auto size         = 8.0f;
            const auto textCenter   = rectangle.topRight() + QPoint(size, -size);

            textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));

            break;
        }
        
        case PixelSelectionType::Brush:
        {
            const auto brushCenter = _mousePosition;

            if (noMousePositions >= 1) {
                if (noMousePositions == 1) {
                    areaPainter.setBrush(_areaBrush);
                    areaPainter.drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);
                }
                else {
                    areaPainter.setBrush(Qt::NoBrush);
                    areaPainter.setPen(QPen(_areaBrush, 2.0 * _brushRadius, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
                    areaPainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());
                }
            }

            shapePainter.setPen(Qt::NoPen);
            shapePainter.setBrush(_areaBrush);

            shapePainter.drawPoint(brushCenter);

            shapePainter.setPen(_mouseButtons & Qt::LeftButton ? _penLineForeGround : _penLineBackGround);
            shapePainter.setBrush(Qt::NoBrush);
            shapePainter.drawEllipse(QPointF(brushCenter), _brushRadius, _brushRadius);

            controlPoints << brushCenter;

            const auto textAngle    = 0.75f * M_PI;
            const auto size         = 12.0f;
            const auto textCenter   = brushCenter + (_brushRadius + size) * QPointF(sin(textAngle), cos(textAngle));

            textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));

            break;
        }
        
        case PixelSelectionType::Lasso:
        {
            if (noMousePositions < 2)
                break;

            areaPainter.setBrush(_areaBrush);
            areaPainter.setPen(Qt::NoPen);
            areaPainter.drawPolygon(_mousePositions.constData(), _mousePositions.count());

            shapePainter.setBrush(Qt::NoBrush);

            shapePainter.setPen(_penLineForeGround);
            shapePainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());

            controlPoints << _mousePositions.first();
            controlPoints << _mousePositions.last();

            shapePainter.setBrush(_areaBrush);
            shapePainter.setPen(_penLineBackGround);
            shapePainter.drawPolyline(controlPoints.constData(), controlPoints.count());

            const auto size = 8.0f;
            const auto textCenter = _mousePositions.first() - QPoint(size, size);

            textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));

            break;
        }
        
        case PixelSelectionType::Polygon:
        {
            if (noMousePositions < 2)
                break;

            areaPainter.setBrush(_areaBrush);
            areaPainter.setPen(Qt::NoPen);
            areaPainter.drawPolygon(_mousePositions.constData(), _mousePositions.count());

            shapePainter.setBrush(Qt::NoBrush);

            shapePainter.setPen(_penLineForeGround);
            shapePainter.drawPolyline(_mousePositions.constData(), _mousePositions.count());

            QVector<QPoint> connectingPoints;

            connectingPoints << _mousePositions.first();
            connectingPoints << _mousePositions.last();

            shapePainter.setPen(_penLineBackGround);
            shapePainter.drawPolyline(connectingPoints.constData(), connectingPoints.count());

            controlPoints << _mousePositions;

            const auto size = 8.0f;
            const auto textCenter = _mousePositions.first() - QPoint(size, size);

            textRectangle = QRectF(textCenter - QPointF(size, size), textCenter + QPointF(size, size));

            break;
        }

        case PixelSelectionType::Sample:
        {
            if (noMousePositions < 1)
                break;

            const auto mousePosition = _mousePositions.last();

            areaPainter.setBrush(_areaBrush);
            areaPainter.setPen(QPen(_areaBrush, 5, Qt::SolidLine, Qt::RoundCap));
            areaPainter.drawPoint(mousePosition);

            shapePainter.setBrush(Qt::NoBrush);
            shapePainter.setPen(_mouseButtons & Qt::LeftButton ? _penLineForeGround : _penLineBackGround);

            shapePainter.drawPolyline(QVector<QPoint>({
                QPoint(mousePosition.x(), 0),
                QPoint(mousePosition.x(), _shapePixmap.size().height())
            }));

            shapePainter.drawPolyline(QVector<QPoint>({
                QPoint(0, mousePosition.y()),
                QPoint(_shapePixmap.size().width(), mousePosition.y())
                }));

            break;
        }

        case PixelSelectionType::ROI:
        {
            const auto topLeft      = QPointF(0.0f, 0.0f);
            const auto bottomRight  = QPointF(_shapePixmap.size().width(), _shapePixmap.size().height());
            const auto rectangle    = QRectF(topLeft, bottomRight);

            auto boundsPen = _penLineBackGround;

            boundsPen.setWidth(2 * boundsPen.width());

            shapePainter.setPen(boundsPen);
            shapePainter.drawRect(rectangle);

            const auto crossHairSize = 15;

            shapePainter.setPen(_penLineForeGround);

            shapePainter.drawPolyline(QVector<QPointF>({
                rectangle.center() - QPointF(crossHairSize, 0.0f),
                rectangle.center() + QPointF(crossHairSize, 0.0f)
            }));

            shapePainter.drawPolyline(QVector<QPointF>({
                rectangle.center() - QPointF(0.0f, crossHairSize),
                rectangle.center() + QPointF(0.0f, crossHairSize)
                }));

            break;
        }

        default:
            break;
    }
    
    shapePainter.setPen(_penControlPoint);
    shapePainter.drawPoints(controlPoints);
    
    switch (_type)
    {
        case PixelSelectionType::Rectangle:
        case PixelSelectionType::Brush:
        case PixelSelectionType::Lasso:
        case PixelSelectionType::Polygon:
        {
            switch (_modifier)
            {
                case PixelSelectionModifierType::Replace:
                    break;

                case PixelSelectionModifierType::Add:
                    shapePainter.setPen(_penLineForeGround);
                    shapePainter.drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("plus-circle"), QTextOption(Qt::AlignCenter));
                    break;

                case PixelSelectionModifierType::Remove:
                    shapePainter.setPen(_penLineForeGround);
                    shapePainter.drawText(textRectangle, hdps::Application::getIconFont("FontAwesome").getIconCharacter("minus-circle"), QTextOption(Qt::AlignCenter));
                    break;

                default:
                    break;
            }

            break;
        }

        case PixelSelectionType::Sample:
        {
            //shapePainter.drawText(_mousePosition, QString("%1, %2").arg(QString::number(_mousePosition.x()), QString::number(_mousePosition.y())));
            break;
        }

        default:
            break;
    }

    emit shapeChanged();
    emit areaChanged();
}

void PixelSelectionTool::startSelection()
{
    _active = true;

    emit started();
}

void PixelSelectionTool::endSelection()
{
    emit ended();

    _mousePositions.clear();
    _active = false;
}

#if (__cplusplus < 201703L)   // definition needed for pre C++17 gcc and clang
constexpr float PixelSelectionTool::BRUSH_RADIUS_MIN;
constexpr float PixelSelectionTool::BRUSH_RADIUS_MAX;
constexpr float PixelSelectionTool::BRUSH_RADIUS_DEFAULT;
constexpr float PixelSelectionTool::BRUSH_RADIUS_DELTA;
#endif

}
}
