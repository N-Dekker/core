#pragma once

#include <QWidget>
#include <QMap>
#include <QPen>
#include <QBrush>

class QPainter;

/**
 * Pixel selection tool class
 *
 * Class for pixel selection using various tools (e.g. brush, rectangle and lasso)
 * It handles mouse/keyboard events to switch between tools and perform the selection itself
 * It draws an overlay of the selection shape \p _shapePixmap and the selected area \p _areaPixmap
 *
 * @author Thomas Kroes
 */
class PixelSelectionTool : public QObject
{
    Q_OBJECT

public:

    /** Types */
    enum class Type
    {
        Rectangle,      /** Select pixels within a rectangle */
        Brush,          /** A brush is used the paint the pixel selection */
        Lasso,          /** A lasso tool is used to select pixels */
        Polygon         /** Select pixels in the interior of a polygon */
    };

    /** Maps type name to type enum and vice versa */
    static QMap<QString, Type> const types;

    /** Get string representation of type enum */
    static QString getTypeName(const Type& type) {
        return types.key(type);
    }

    /** Get enum representation from type name */
    static Type getTypeEnum(const QString& typeName) {
        return types[typeName];
    }

    /** Modifiers */
    enum class Modifier
    {
        Replace,        /** Replace selection */
        Add,            /** Add to selection */
        Remove          /** Remove from selection */
    };

    /** Maps modifier name to modifier enum and vice versa */
    static QMap<QString, Modifier> const modifiers;

    /** Get string representation of modifier enum */
    static QString getModifierName(const Modifier& modifier) {
        return modifiers.key(modifier);
    }

    /** Get enum representation from modifier name */
    static Modifier getModifierEnum(const QString& modifierName) {
        return modifiers[modifierName];
    }

public: // Construction/destruction

    /**
     * Constructor
     * @param parent Parent object
     * @param enabled Whether the tool is enabled or not
     */
    PixelSelectionTool(QObject* parent, const bool& enabled = true);

public: // Getters/setters

    /** Get/set whether the tool is enabled or not */
    bool isEnabled() const;
    void setEnabled(const bool& enabled);

    /** Get/set the current pixel selection shape type */
    Type getType() const;
    void setType(const Type& type);

    /** Get/set the current pixel selection modifier */
    Modifier getModifier() const;
    void setModifier(const Modifier& modifier);

    /** Get/set whether notifications should be fired continuously or only at the end of selection */
    bool isNotifyDuringSelection() const;
    void setNotifyDuringSelection(const bool& notifyDuringSelection);

    /** Get/set (brush) radius */
    float getRadius() const;
    void setRadius(const float& radius);

    /** Trigger signals */
    void setChanged();

    /** Returns whether the selection process is currently active */
    bool isActive() const {
        return _active;
    }

    /** Aborts the selection process */
    void abort();

    /** Get the pixmap overlay that contains the selection tool visualization */
    QPixmap& getShapePixmap() {
        return _shapePixmap;
    }

    /** Get the pixmap overlay that contains the selected pixels*/
    QPixmap& getAreaPixmap() {
        return _areaPixmap;
    }

public: // Event handling

    /**
     * Listens to the events of \p target
     * @param target Target object to watch for events
     * @param event Event that occurred
     */
    bool eventFilter(QObject* target, QEvent* event) override;

private:

    /** Paints the selection tool pixmaps */
    void paint();

    /** Initiates the selection process */
    void startSelection();

    /** Ends the selection process */
    void endSelection();

signals:

    /** Signals that the type has changed */
    void typeChanged(const Type& type);

    /** Signals that the selection modifier has changed */
    void modifierChanged(const Modifier& modifier);

    /** Signals that the notify during selection property has changed */
    void notifyDuringSelectionChanged(const bool& notifyDuringSelection);

    /** Signals that the radius has changed */
    void radiusChanged(const float& radius);

    /** Signals that the selection shape changed */
    void shapeChanged();

    /** Signals that the selection area changed */
    void areaChanged();

    /** Signals that the selection process has started */
    void started();

    /** Signals that the selection process has ended */
    void ended();

protected:
    bool                _enabled;                   /** Whether the tool is enabled or not */
    Type                _type;                      /** Current selection type */
    Modifier            _modifier;                  /** Current selection modifier */
    bool                _active;                    /** Whether the selection process is active */
    bool                _notifyDuringSelection;     /** Whether the selection is published continuously or at the end */
    float               _radius;                    /** Brush/circle radius */
    QPoint              _mousePosition;             /** Current mouse position */
    QVector<QPoint>     _mousePositions;            /** Recorded mouse positions */
    int                 _mouseButtons;              /** State of the left, middle and right mouse buttons */
    QPixmap             _shapePixmap;               /** Pixmap for the selection tool shape */
    QPixmap             _areaPixmap;                /** Pixmap for the selection area */

public:

    // Brush/circle radius 
    static constexpr float RADIUS_MIN       = 10.0f;        /** Minimum radius */
    static constexpr float RADIUS_MAX       = 1000.0f;      /** Maximum radius */
    static constexpr float RADIUS_DEFAULT   = 25.0f;        /** Default radius */
    static constexpr float RADIUS_DELTA     = 10.0f;        /** Radius increment */

    // Drawing constants
    static const QColor COLOR_MAIN;         /** Main color */
    static const QColor COLOR_FILL;         /** Selection area fill color (based on main color) */
    static const QBrush AREA_BRUSH;         /** Selection area brush */
    static const QPen PEN_LINE_FG;          /** Foreground pen */
    static const QPen PEN_LINE_BG;          /** Background pen */
    static const QPen PEN_CP;               /** Control point pen */
};