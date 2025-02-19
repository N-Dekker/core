// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Plugin.h"

#include "DataHierarchyItem.h"
#include "Set.h"

#include <memory>

namespace mv
{

namespace plugin
{

class AnalysisPlugin : public Plugin
{

public:
    AnalysisPlugin(const PluginFactory* factory);

    ~AnalysisPlugin() override {};

    /**
     * Set name of the object
     * @param name Name of the object
     */
    void setObjectName(const QString& name);

    /**
     * Set input dataset smart pointer
     * @param inputDataset Smart pointer to the input dataset
     */
    void setInputDataset(Dataset<DatasetImpl> inputDataset);

    /** Get input dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getInputDataset() {
        return Dataset<DatasetType>(_input.get<DatasetType>());
    }

    /**
     * Set output dataset smart pointer
     * @param outputDataset Smart pointer to output dataset
     */
    void setOutputDataset(Dataset<DatasetImpl> outputDataset);

    /** Get output dataset smart pointer */
    template<typename DatasetType = DatasetImpl>
    Dataset<DatasetType> getOutputDataset() {
        return Dataset<DatasetType>(_output.get<DatasetType>());
    }

protected:
    Dataset<DatasetImpl>    _input;       /** Input dataset smart pointer */
    Dataset<DatasetImpl>    _output;      /** Output dataset smart pointer */
};

class AnalysisPluginFactory : public PluginFactory
{
    Q_OBJECT
    
public:
    AnalysisPluginFactory() :
        PluginFactory(Type::ANALYSIS)
    {

    }
    ~AnalysisPluginFactory() override {};
    
    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    /**
    * Produces an instance of an analysis plugin. This function gets called by the plugin manager.
    */
    AnalysisPlugin* produce() override = 0;
};

} // namespace plugin

} // namespace mv

Q_DECLARE_INTERFACE(mv::plugin::AnalysisPluginFactory, "hdps.AnalysisPluginFactory")
