// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <vector>

using namespace mv;
using namespace mv::plugin;

// =============================================================================
// Data Type
// =============================================================================

const mv::DataType TextType = mv::DataType(QString("Text"));

// =============================================================================
// Raw Data
// =============================================================================

class TextData : public mv::plugin::RawData
{
public:
    TextData(PluginFactory* factory) : mv::plugin::RawData(factory, TextType) { }
    ~TextData(void) override;
    
    void init() override;

    /**
     * Create dataset for raw data
     * @param guid Globally unique dataset identifier (use only for deserialization)
     * @return Smart pointer to dataset
     */
    Dataset<DatasetImpl> createDataSet(const QString& guid = "") const override;

private:
    std::vector<QString> _data;
};

class Text : public DatasetImpl
{
public:
    Text(mv::CoreInterface* core, QString dataName, const QString& guid = "") :
        DatasetImpl(core, dataName, guid)
    {
    }

    ~Text() override { }

    Dataset<DatasetImpl> copy() const override
    {
        auto text = new Text(_core, getRawDataName());

        text->setText(this->text());
        text->indices = indices;
        
        return text;
    }

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    Dataset<DatasetImpl> createSubsetFromSelection(const QString& guiName, const Dataset<DatasetImpl>& parentDataSet = Dataset<DatasetImpl>(), const bool& visible = true) const override
    {
        return _core->createSubsetFromSelection(getSelection(), toSmartPointer(), guiName, parentDataSet, visible);
    }

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

public: // Selection

    /**
     * Get selection
     * @return Selection indices
     */
    std::vector<std::uint32_t>& getSelectionIndices() override;

    /**
     * Select by indices
     * @param indices Selection indices
     */
    void setSelectionIndices(const std::vector<std::uint32_t>& indices) override;

    /** Determines whether items can be selected */
    bool canSelect() const override;

    /** Determines whether all items can be selected */
    bool canSelectAll() const override;

    /** Determines whether there are any items which can be deselected */
    bool canSelectNone() const override;

    /** Determines whether the item selection can be inverted (more than one) */
    bool canSelectInvert() const override;

    /** Select all items */
    void selectAll() override;

    /** Deselect all items */
    void selectNone() override;

    /** Invert item selection */
    void selectInvert() override;


    std::vector<unsigned int> indices;
};

// =============================================================================
// Factory
// =============================================================================

class TextDataFactory : public RawDataFactory
{
    Q_INTERFACES(mv::plugin::RawDataFactory mv::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "hdps.TextData"
                      FILE  "TextData.json")
    
public:
    TextDataFactory(void) {}
    ~TextDataFactory(void) override {}

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

    mv::plugin::RawData* produce() override;
};
