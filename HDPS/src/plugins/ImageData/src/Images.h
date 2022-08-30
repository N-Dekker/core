#pragma once

#include "Common.h"
#include "Image.h"
#include "ImageData.h"

#include <Set.h>

#include <QString>
#include <QImage>
#include <QColor>
#include <QRect>
#include <QSize>

#include <vector>

using namespace hdps::plugin;

class InfoAction;

/**
 * Images dataset class
 *
 * Class for dealing with high-dimensional image data
 *
 * @author Thomas Kroes
 */
class IMAGEDATA_EXPORT Images : public hdps::DatasetImpl
{
public: // Construction

    /**
     * Constructor
     * @param core Pointer to core interface
     * @param dataName Name of the dataset
     * @param guid Globally unique dataset identifier (use only for deserialization)
     */
    Images(hdps::CoreInterface* core, QString dataName, const QString& guid = "");

    /** Initializes the dataset */
    void init() override;

public: // Subsets

    /**
     * Create subset from the current selection and specify where the subset will be placed in the data hierarchy
     * @param guiName Name of the subset in the GUI
     * @param parentDataSet Smart pointer to parent dataset in the data hierarchy (default is below the set)
     * @param visible Whether the subset will be visible in the UI
     * @return Smart pointer to the created subset
     */
    hdps::Dataset<hdps::DatasetImpl> createSubsetFromSelection(const QString& guiName, const hdps::Dataset<hdps::DatasetImpl>& parentDataSet = hdps::Dataset<hdps::DatasetImpl>(), const bool& visible = true) const override;

public: // Image retrieval functions

    /** Obtain a copy of this dataset */
    hdps::Dataset<hdps::DatasetImpl> copy() const override;

    /** Gets the image collection type e.g. stack or sequence */
    ImageData::Type getType() const;

    /**
     * Sets the image collection type e.g. stack or sequence
     * @param type Image collection type
     */
    void setType(const ImageData::Type& type);

    /** Gets the number of images in this dataset */
    std::uint32_t getNumberOfImages() const;

    /**
     * Sets the number of images
     * @param numberOfImages Number of images
     */
    void setNumberOfImages(const std::uint32_t& numberOfImages);

    /** Gets the size of the images */
    QSize getImageSize() const;

    /**
     * Set the image size
     * @param imageSize Size of the image(s)
     */
    void setImageSize(const QSize& imageSize);

    /** Gets the number of components per pixel */
    std::uint32_t getNumberOfComponentsPerPixel() const;

    /**
     * Sets the number of components per pixel
     * @param numberOfComponentsPerPixel Number of components per pixel
     */
    void setNumberOfComponentsPerPixel(const std::uint32_t& numberOfComponentsPerPixel);

    /** Get the image file paths */
    QStringList getImageFilePaths() const;

    /**
     * Sets the image file paths
     * @param imageFilePaths Image file paths
     */
    void setImageFilePaths(const QStringList& imageFilePaths);

    /** Returns the number of pixels in total */
    std::uint32_t getNumberOfPixels() const;

    /** Returns the number of color channels per pixel */
    static std::uint32_t noChannelsPerPixel();

    /** Get the rectangle of the entire image */
    QRect getRectangle() const;

    /** Get the rectangle which bounds the visible pixels */
    QRect getVisibleRectangle() const;

    /**
     * Get plugin icon
     * @param color Icon color for flat (font) icons
     * @return Icon
     */
    QIcon getIcon(const QColor& color = Qt::black) const override;

public: // Selection

    /**
     * Get selection indices
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

public:

    /**
     * Get scalar image data
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarData(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get mask image data (for subsets)
     * @param maskData Mask scalar data
     */
    void getMaskData(std::vector<std::uint8_t>& maskData);

    /**
     * Get selection data
     * @param selectionImageData Image data for the selection
     * @param selectedIndices Selected pixel indices
     * @param selectionBoundaries Boundaries of the selection (in image coordinates)
     */
    void getSelectionData(std::vector<std::uint8_t>& selectionImageData, std::vector<std::uint32_t>& selectedIndices, QRect& selectionBoundaries);

protected:

    /**
     * Get scalar data for image sequence
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarDataForImageSequence(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /**
     * Get scalar data for image stack
     * @param dimensionIndex Dimension index
     * @param scalarData Scalar data for the specified dimension
     * @param scalarDataRange Scalar data range
     */
    void getScalarDataForImageStack(const std::uint32_t& dimensionIndex, QVector<float>& scalarData, QPair<float, float>& scalarDataRange);

    /** Computes and caches the mask data */
    void computeMaskData();

    /**
     * Get pixel coordinate from pixel index
     * @param pixelIndex Pixel index
     */
    QPoint getPixelCoordinateFromPixelIndex(const std::int32_t& pixelIndex) const;

    /**
     * Get pixel index from pixel coordinate
     * @param pixelCoordinate Pixel coordinate
     * @return Pixel index
     */
    std::int32_t getPixelIndexFromPixelCoordinate(const QPoint& pixelCoordinate) const;

public: // Serialization

    /**
     * Load widget action from variant
     * @param Variant representation of the widget action
     */
    void fromVariantMap(const QVariantMap& variantMap) override;

    /**
     * Save widget action to variant
     * @return Variant representation of the widget action
     */
    QVariantMap toVariantMap() const override;

private:
    std::vector<std::uint32_t>      _indices;               /** Selection indices */
    ImageData*                      _imageData;             /** Pointer to raw image data */
    QSharedPointer<InfoAction>      _infoAction;            /** Shared pointer to info action */
    QRect                           _visibleRectangle;      /** Rectangle which bounds the visible pixels */
    std::vector<std::uint8_t>       _maskData;              /** Mask data */
};