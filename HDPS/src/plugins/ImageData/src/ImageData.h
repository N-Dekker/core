#pragma once

#include "Common.h"
#include "Image.h"

#include <RawData.h>
#include <Set.h>

#include <QString>
#include <QSize>
#include <QDebug>

#include <vector>

using namespace hdps::plugin;

class Points;

const hdps::DataType ImageType = hdps::DataType(QString("Images"));

class IMAGEDATA_EXPORT ImageData : public hdps::RawData
{
public:
    ImageData();
    ~ImageData() override;

    void init() override;

public:
    ImageCollectionType imageCollectionType() const;
    void setImageCollectionType(const ImageCollectionType& imageCollectionType);
    std::uint32_t noImages() const;
    void setNoImages(const std::uint32_t& noImages);
    QSize imageSize() const;
    void setImageSize(const QSize& imageSize);
    std::uint32_t noComponents() const;
    void setNoComponents(const std::uint32_t& noComponents);
    std::vector<QString> imageFilePaths() const;
    void setImageFilePaths(const std::vector<QString>& imageFilePaths);
    std::vector<QString> dimensionNames() const;
    void setDimensionNames(const std::vector<QString>& dimensionNames);

    QString pointsName() const;
    Points* points() const;

public:
    //void setSequence(const std::vector<Image>& images, const QSize& size);
    //void setStack(const std::vector<Image>& images, const QSize& size);

    std::uint32_t noPoints() const;
    std::uint32_t noDimensions() const;
    
    float pointValue(const std::uint32_t& index) const;
    float pointValue(const std::uint32_t& x, const std::uint32_t& y) const;

    hdps::DataSet* createDataSet() const override;

private:
    ImageCollectionType     _imageCollectionType;
    std::uint32_t           _noImages;
    QSize                   _imageSize;
    std::uint32_t           _noComponents;
    std::vector<QString>    _imageFilePaths;
    std::vector<QString>    _dimensionNames;
    QString                 _pointsName;
    Points*                 _points;
};

class ImageDataFactory : public hdps::plugin::RawDataFactory
{
    Q_INTERFACES(hdps::plugin::RawDataFactory hdps::plugin::PluginFactory)
    Q_OBJECT
    Q_PLUGIN_METADATA(IID   "nl.tudelft.ImageData"
                      FILE  "ImageData.json")
    
public:
    ImageDataFactory() {}
    ~ImageDataFactory() override {}
    
    hdps::RawData* produce() override;
};