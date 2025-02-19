// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "GlobalSettingsGroupAction.h"

#include "actions/OptionAction.h"

namespace mv
{

/**
 * Application global settings action class
 *
 * Action class which groups all global settings for the application
 *
 * @author Thomas Kroes
 * @author Thomas Höllt
 */
class ApplicationSettingsAction final : public GlobalSettingsGroupAction
{
public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     */
    ApplicationSettingsAction(QObject* parent);

public: // Action getters

    gui::OptionAction* getAppearanceOptionAction() { return _appearanceOptionAction; }

private:
    gui::OptionAction* _appearanceOptionAction;       /** Options for dark, light, or system appearance */
};

}
