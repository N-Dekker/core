// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "HorizontalGroupAction.h"
#include "IntegralAction.h"
#include "StringAction.h"

#include <QCompleter>

namespace mv::gui {

/**
 * Version action class
 *
 * Action class for storing a major and minor version
 *
 * @author Thomas Kroes
 */
class VersionAction final : public HorizontalGroupAction
{
    Q_OBJECT

public:

    /**
     * Construct with \p parent and \p project
     * @param parent Pointer to parent object
     * @param title Title of the action
     */
    Q_INVOKABLE VersionAction(QObject* parent, const QString& title);

public: // Serialization

    /**
     * Load project from variant
     * @param Variant representation of the project
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save project to variant
     * @return Variant representation of the project
     */
    QVariantMap toVariantMap() const override;

public: // Action getters

    const gui::IntegralAction& getMajorAction() const { return _majorAction; }
    const gui::IntegralAction& getMinorAction() const { return _minorAction; }
    const gui::StringAction& getSuffixAction() const { return _suffixAction; }
    const gui::StringAction& getVersionStringAction() const { return _versionStringAction; }

    gui::IntegralAction& getMajorAction() { return _majorAction; }
    gui::IntegralAction& getMinorAction() { return _minorAction; }
    gui::StringAction& getSuffixAction() { return _suffixAction; }
    gui::StringAction& getVersionStringAction() { return _versionStringAction; }

private:
    IntegralAction  _majorAction;           /** Action for storing the major version */
    IntegralAction  _minorAction;           /** Action for storing the minor version */
    StringAction    _suffixAction;          /** Action which stores the version suffix (e.g. alpha or beta) */
    StringAction    _versionStringAction;   /** Action which stores the string representation of the version */
    QCompleter      _suffixCompleter;       /** Completer for version suffix */
};

}

Q_DECLARE_METATYPE(mv::gui::VersionAction)

inline const auto versionActionMetaTypeId = qRegisterMetaType<mv::gui::VersionAction*>("mv::gui::VersionAction");