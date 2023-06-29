#pragma once

#include "WidgetActionViewWidget.h"

namespace hdps::gui {

class WidgetAction;
class OverlayWidget;

/**
 * Widget action widget class
 *
 * Base class for widgets that interact with a widget action
 * 
 * @author Thomas Kroes
 */
class WidgetActionWidget : public WidgetActionViewWidget
{
public:

    /** Reserved widget settings */
    enum WidgetFlag {
        PopupLayout = 0x00100,      /** Widget with popup layout */
    };

public:

    // TODO: revisit
    /** Widget states */
    enum State {
        Standard,   /** Standard representation of the action (default) */
        Collapsed,  /** The widget is in a collapsed state and represented by a popup button */
        Popup       /** The widget representation of the action in a popup */
    };

public:

    /**
     * Constructor
     * @param parent Parent widget
     * @param action Pointer to the widget action that will be displayed
     * @param widgetFlags Widget flags for the configuration of the widget (type)
     */
    WidgetActionWidget(QWidget* parent, WidgetAction* action, const std::int32_t& widgetFlags = 0);

public: // Popup widget related
    
    /**
     * Get whether the widget is in a popup state
     * @return Boolean determining whether the widget is in a popup state
     */
    virtual bool isPopup() const final;

    /**
     * Override the size hint to account for popups
     * @return Size hint
     */
    QSize sizeHint() const override;

    /**
     * Override to allow for popup layouts
     * @param layout Pointer to layout
     */
    void setLayout(QLayout* layout);

private:

    /**
     * Set popup layout (layout contains groupbox etc.)
     * @param popupLayout Pointer to the popup layout
     */
    virtual void setPopupLayout(QLayout* popupLayout) final;

protected:
    WidgetAction*       _widgetAction;      /** Pointer to widget action that will be displayed */
    std::int32_t        _widgetFlags;       /** Widget creation flags */
};

}
