// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "pointsplugin_export.h"

#include "RawData.h"

#include "Set.h"
#include <QString>
#include <QMap>
#include <QVariant>
#include <vector>

using namespace mv::plugin;

// =============================================================================
// View
// =============================================================================

class POINTSPLUGIN_EXPORT PointsPlugin : public RawData
{
public:
    PointsPlugin() : RawData("Points") { }
    ~PointsPlugin(void) override;

    void init() override;

    mv::Set* createSet() const override;

    unsigned int getNumPoints() const;

    unsigned int getNumDimensions() const;

    const std::vector<float>& getData() const;

    const std::vector<QString>& getDimensionNames() const;

    void setData(const float* data, unsigned int numPoints, unsigned int numDimensions);

    void setDimensionNames(const std::vector<QString>& dimNames);

    // Constant subscript indexing
    const float& operator[](unsigned int index) const;

    // Subscript indexing
    float& operator[](unsigned int index);

    // Temporary property metadata
    QVariant getProperty(const QString & name) const;

    void setProperty(const QString & name, const QVariant & value);

    bool hasProperty(const QString & name) const;

    QStringList propertyNames() const;

private:
    /** Main store of point data in dimension-major order */
    std::vector<float> _data;

    /** Number of features of each data point */
    unsigned int _numDimensions = 1;

    std::vector<QString> _dimNames;

    QMap<QString, QVariant> _properties;
};

class IndexSet : public mv::Set
{
public:
    IndexSet(mv::CoreInterface* core, QString dataName) : Set(core, dataName) { }
    ~IndexSet() override { }

    PointsPlugin& getData() const
    {
        return dynamic_cast<PointsPlugin&>(_core->requestData(getDataName()));
    }

    Set* copy() const override;

    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class PointsPluginFactory : public RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
        Q_OBJECT
        Q_PLUGIN_METADATA(IID   "nl.tudelft.PointsPlugin"
            FILE  "PointsPlugin.json")

public:
    PointsPluginFactory(void) {}
    ~PointsPluginFactory(void) override {}

    RawData* produce() override;
};
