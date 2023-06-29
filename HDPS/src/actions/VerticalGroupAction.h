#pragma once

#include "GroupAction.h"

namespace hdps::gui {

/**
 * Vertical group action class
 *
 * Convenience group action for displaying child actions vertically
 *
 * @author Thomas Kroes
 */
class VerticalGroupAction : public GroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p title
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param expanded Whether the group is initially expanded/collapsed
     */
    Q_INVOKABLE VerticalGroupAction(QObject* parent, const QString& title, const bool& expanded = false);
};

}

Q_DECLARE_METATYPE(hdps::gui::VerticalGroupAction)

inline const auto verticalGroupActionMetaTypeId = qRegisterMetaType<hdps::gui::VerticalGroupAction*>("hdps::gui::VerticalGroupAction");