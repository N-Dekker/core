// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include "Dataset.h"
#include "Set.h"

#include "actions/WidgetAction.h"
#include "actions/OptionAction.h"
#include "event/EventListener.h"

#include <QAbstractListModel>

namespace mv::gui {

/**
 * Dataset picker action class
 *
 * Action class for picking a dataset from a list
 * Automatically removes items when datasets are removed and renamed
 *
 * @author Thomas Kroes
 */
class DatasetPickerAction : public OptionAction
{
Q_OBJECT

protected:

    /**
     * Datasets list model class
     *
     * List model class which serves as input to the dataset picker option action class
     *
     * @author Thomas Kroes
     */
    class DatasetsModel : public QAbstractListModel {
    public:

        /** Model columns */
        enum class Column {
            Name,
            GUID
        };

        /** (Default) constructor */
        DatasetsModel(QObject* parent = nullptr);

        /**
         * Get the number of row
         * @param parent Parent model index
         * @return Number of rows in the model
         */
        int rowCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * Get the row index of a dataset
         * @param parent Parent model index
         * @return Row index of the dataset
         */
        int rowIndex(const mv::Dataset<mv::DatasetImpl>& dataset) const;

        /**
         * Get the number of columns
         * @param parent Parent model index
         * @return Number of columns in the model
         */
        int columnCount(const QModelIndex& parent = QModelIndex()) const;

        /**
         * Get data
         * @param index Model index to query
         * @param role Data role
         * @return Data
         */
        QVariant data(const QModelIndex& index, int role) const;

        /**
         * Get datasets
         * @return Vector of smart pointers to datasets
         */
        const QVector<mv::Dataset<mv::DatasetImpl>>& getDatasets() const;

        /**
         * Get dataset at the specified row index
         * @param rowIndex Index of the row
         * @return Smart pointer to dataset
         */
        mv::Dataset<mv::DatasetImpl> getDataset(const std::int32_t& rowIndex) const;

        /**
         * Set datasets (resets the model)
         * @param datasets Vector of smart pointers to datasets
         */
        void setDatasets(const QVector<mv::Dataset<mv::DatasetImpl>>& datasets);

        /**
         * Add dataset
         * @param dataset Smart pointer to dataset
         */
        void addDataset(const mv::Dataset<mv::DatasetImpl>& dataset);

        /**
         * Remove specific dataset
         * @param dataset Smart pointer to dataset
         */
        void removeDataset(const mv::Dataset<mv::DatasetImpl>& dataset);

        /** Remove all datasets from the model */
        void removeAllDatasets();

        /** Get whether to show the dataset icon */
        bool getShowIcon() const;

        /**
         * Set whether to show the icon
         * @param showFullPathName Whether to show the icon
         */
        void setShowIcon(bool showIcon);

        /** Get to show the dataset location */
        bool getShowLocation() const;

        /**
         * Set whether to show the full path name in the GUI
         * @param showFullPathName Whether to show the full path name in the GUI
         */
        void setShowLocation(bool showLocation);

        /** Updates the model from the datasets */
        void updateData();

    protected:
        QVector<mv::Dataset<mv::DatasetImpl>>   _datasets;          /** Datasets from which can be picked */
        bool                                        _showIcon;          /** Whether to show the dataset icon */
        bool                                        _showLocation;      /** Whether to show the dataset location */
    };

public:

    /** Pickable datasets */
    enum class Mode {
        Manual,         /** ...are set manually with \p setDatasets() */
        Automatic       /** ...are a continuous reflection of the datasets in the data model and can be possibly filtered with \p setDatasetsFilterFunction() */
    };

    /** Filter function signature, the input are all datasets in the core and it returns the filtered datasets */
    using DatasetsFilterFunction = std::function<mv::Datasets(const mv::Datasets&)>;

public:

    /**
     * Constructor
     * @param parent Pointer to parent object
     * @param title Title of the action
     * @param mode Picker mode
     */
    Q_INVOKABLE DatasetPickerAction(QObject* parent, const QString& title, Mode mode = Mode::Automatic);

    /** Get current mode */
    Mode getMode() const;

    /**
     * Set current mode
     * @param mode Mode
     */
    void setMode(Mode mode);

    /**
     * Get datasets
     * @return Datasets
     */
    mv::Datasets getDatasets() const;

    /**
     * Set the datasets from which can be picked (mode is set to Mode::Manual)
     * @param datasets Datasets from which can be picked
     */
    void setDatasets(mv::Datasets datasets);

    /**
     * Set datasets filter function (mode is set to Mode::Automatic)
     * @param datasetsFilterFunction Filter lambda (triggered when datasets are added and/or removed from the datasets model)
     */
    void setDatasetsFilterFunction(const DatasetsFilterFunction& datasetsFilterFunction);

    /** Get the current dataset */
    mv::Dataset<mv::DatasetImpl> getCurrentDataset() const;

    /** Get the current dataset */
    template<typename DatasetType>
    mv::Dataset<DatasetType> getCurrentDataset() const
    {
        return getCurrentDataset();
    }

    /**
     * Set the current dataset
     * @param currentDataset Smart pointer to current dataset
     */
    void setCurrentDataset(mv::Dataset<mv::DatasetImpl> currentDataset);

    /**
     * Set the current dataset by \p guid
     * @param guid Current dataset globally unique identifier
     */
    void setCurrentDataset(const QString& guid);

    /**
     * Get current dataset globally unique identifier
     * @return The globally unique identifier of the currently selected dataset (if any)
     */
    QString getCurrentDatasetGuid() const;

public: // Datasets model facade

    /** Get whether to show the dataset icon */
    bool getShowIcon() const {
        return _datasetsModel.getShowIcon();
    }

    /**
     * Set whether to show the icon
     * @param showFullPathName Whether to show the icon
     */
    void setShowIcon(bool showIcon) {
        _datasetsModel.setShowIcon(showIcon);
    }

    /** Get whether to show the location */
    bool getShowLocation() const {
        return _datasetsModel.getShowLocation();
    }

    /**
     * Set whether to show the location
     * @param showLocation Boolean determining whether to show the location
     */
    void setShowFullPathName(bool showLocation) {
        _datasetsModel.setShowLocation(showLocation);
    }

private:

    /** Populates the datasets from the core */
    void populateDatasetsFromCore();

protected: // Linking

    /**
     * Connect this action to a public action
     * @param publicAction Pointer to public action to connect to
     * @param recursive Whether to also connect descendant child actions
     */
    void connectToPublicAction(WidgetAction* publicAction, bool recursive) override;

    /**
     * Disconnect this action from its public action
     * @param recursive Whether to also disconnect descendant child actions
     */
    void disconnectFromPublicAction(bool recursive) override;

public: // Serialization

    /**
     * Load widget action from variant map
     * @param Variant map representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant map
     * @return Variant map representation of the widget action
     */
    QVariantMap toVariantMap() const override;

signals:

    /**
     * Signals that a dataset has been picked
     * @param Smart pointer to picked dataset
     */
    void datasetPicked(mv::Dataset<mv::DatasetImpl> pickedDataset);

    /**
     * Signals that selectable datasets changed
     * @param Selectable datasets
     */
    void datasetsChanged(mv::Datasets datasets);

protected:
    Mode                        _mode;                      /** Picker mode (e.g. manual or automatic) */
    DatasetsFilterFunction      _datasetsFilterFunction;    /** Datasets filter lambda */
    DatasetsModel               _datasetsModel;             /** Datasets list model */
    mv::EventListener         _eventListener;             /** Listen to events from the core */

    friend class AbstractActionsManager;
};

}

Q_DECLARE_METATYPE(mv::gui::DatasetPickerAction)

inline const auto datasetPickerActionMetaTypeId = qRegisterMetaType<mv::gui::DatasetPickerAction*>("mv::gui::DatasetPickerAction");
