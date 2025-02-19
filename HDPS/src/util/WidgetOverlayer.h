// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "util/WidgetFader.h"

#include <QObject>

class QWidget;

namespace mv::util {

/**
 * Widget overlayer utility class
 *
 * Helper class for:
 *  - Layering a widget on top of another widget and synchronizing its geometry
 *  - Animating the source widget opacity
 *
 * @author Thomas Kroes
 */
class WidgetOverlayer : public QObject
{
    Q_OBJECT

public:

    /**
     * Construct widget overlayer with \p parent object, \p sourceWidget and \p targetWidget
     * @param parent Pointer to parent object
     * @param sourceWidget Pointer to source widget (will be layered on top of the \p targetWidget)
     * @param targetWidget Pointer to target widget
     * @param initialOpacity Opacity of \p sourceWidget at initialization
     */
    WidgetOverlayer(QObject* parent, QWidget* sourceWidget, QWidget* targetWidget, float initialOpacity = 1.0f);

    /**
     * Respond to \p target events
     * @param target Object of which an event occurred
     * @param event The event that took place
     */
    bool eventFilter(QObject* target, QEvent* event) override;

    /**
     * Get widget fader
     * @return Reference to widget fader
     */
    util::WidgetFader& getWidgetFader();

private:
    util::WidgetFader   _widgetFader;       /** Widget fader for animating the widget opacity */
    QWidget*            _sourceWidget;      /** Pointer to source widget (will be layered on top of the \p targetWidget) */
    QWidget*            _targetWidget;      /** Pointer to target widget */
};

}
