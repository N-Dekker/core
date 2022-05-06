#pragma once

#include "WidgetAction.h"
#include "ColorMapEditor1DNodeAction.h"
#include "ColorMapEditor1DWidget.h"

#include "TriggerAction.h"

namespace hdps {

namespace gui {

class ColorMapAction;

/**
 * Color map editor 1D action class
 *
 * Action class for manually defining a one-dimensional color map
 *
 * @author Thomas Kroes and Mitchell M. de Boer
 */
class ColorMapEditor1DAction : public WidgetAction
{
    Q_OBJECT

public:

    using Histogram = QVector<std::uint32_t>;

public:

    /** Widget class for one-dimensional color map editor action */
    class Widget : public WidgetActionWidget
    {
    public:

        /**
         * Constructor
         * @param parent Pointer to parent widget
         * @param colorMapEditor1DAction Pointer to one-dimensional color map editor action
         */
        Widget(QWidget* parent, ColorMapEditor1DAction* colorMapEditor1DAction);

    protected:
        ColorMapEditor1DWidget      _colorMapEditor1DWidget;    /** Color map editor widget */
        TriggerAction               _goToFirstNodeAction;       /** Go to first node action */
        TriggerAction               _goToPreviousNodeAction;    /** Go to previous node action */
        TriggerAction               _goToNextNodeAction;        /** Go to next node action */
        TriggerAction               _goToLastNodeAction;        /** Go to last node action */
        TriggerAction               _removeNodeAction;          /** Remove node action */
    };

    /**
     * Get widget representation of the one-dimensional color map editor action
     * @param parent Pointer to parent widget
     * @param widgetFlags Widget flags for the configuration of the widget
     */
    QWidget* getWidget(QWidget* parent, const std::int32_t& widgetFlags) override {
        return new Widget(parent, this);
    };

protected:

    /**
     * Constructor
     * @param colorMapAction Reference to color map action
     */
    ColorMapEditor1DAction(ColorMapAction& colorMapAction);

public:

    /**
     * Get color map image
     * @return Color map image
     */
    QImage getColorMapImage();

public: // Nodes

    /**
     * Get nodes
     * @return Nodes
     */
    QVector<ColorMapEditor1DNode*>& getNodes();

    /**
     * Add node at normalized coordinate
     * @param normalizedCoordinate Normalized coordinate
     * @param color Node color
     * @return Pointer to created node
     */
    ColorMapEditor1DNode* addNode(const QPointF& normalizedCoordinate, const QColor& color = Qt::gray);

    /**
     * Remove node
     * @param node Pointer to node which is to be removed
     */
    void removeNode(ColorMapEditor1DNode* node);

    /**
     * Get previous node
     * @param node Pointer to node of which to obtain the previous node
     * @return Pointer to previous node (node if it has no previous node)
     */
    ColorMapEditor1DNode* getPreviousNode(ColorMapEditor1DNode* node) const;

    /**
     * Get next node
     * @param node Pointer to node of which to obtain the next node
     * @return Pointer to next node (node if it has no previous node)
     */
    ColorMapEditor1DNode* getNextNode(ColorMapEditor1DNode* node) const;

public: // Histogram

    /**
     * Get one-dimensional histogram
     * @return One-dimensional histogram
     */
    const Histogram& getHistogram() const;

    /**
     * Set background one-dimensional histogram
     * @param histogram One-dimensional histogram
     */
    void setHistogram(const Histogram& histogram);

protected:

    /** Sort nodes and update their index */
    void sortNodes();

    /** Update the color map image */
    void updateColorMap();

public: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     */
    void connectToPublicAction(WidgetAction* publicAction) override;

    /** Disconnect this action from a public action */
    void disconnectFromPublicAction() override;

public: // Action getters

    ColorMapAction& getColorMapAction() { return _colorMapAction; }
    ColorMapEditor1DNodeAction& getNodeAction() { return _nodeAction; }

signals:

    /**
     * Signals that a node is added
     * @param node Pointer to added node
     */
    void nodeAdded(ColorMapEditor1DNode* node);

    /**
     * Signals that a node is about to be removed
     * @param node Pointer to node which is about to be removed
     */
    void nodeAboutToBeRemoved(ColorMapEditor1DNode* node);

    /**
     * Signals that a node is removed
     * @param index Index of the node which is removed
     */
    void nodeRemoved(const std::uint32_t& index);

    /**
     * Signals that the histogram changed
     * @param histogram Histogram
     */
    void histogramChanged(const Histogram& histogram);

protected:
    ColorMapAction&                 _colorMapAction;    /** Reference to color map action */
    QVector<ColorMapEditor1DNode*>  _nodes;             /** All sorted nodes */
    ColorMapEditor1DNodeAction      _nodeAction;        /** Node action */
    QImage                          _colorMapImage;     /** Output color map image */
    Histogram                       _histogram;         /** Histogram */

    static constexpr QSize colorMapImageSize = QSize(256, 1);

    /** Only color map settings action may instantiate this class */
    friend class ColorMapSettingsAction;
};

}
}
