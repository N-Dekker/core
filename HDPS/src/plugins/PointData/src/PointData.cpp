#include "PointData.h"

#include <QtCore>
#include <QtDebug>

#include <cstring>

#include "graphics/Vector2f.h"


Q_PLUGIN_METADATA(IID "nl.tudelft.PointData")

// =============================================================================
// PointData
// =============================================================================

PointData::~PointData(void)
{
    
}

void PointData::init()
{

}

hdps::DataSet* PointData::createDataSet() const
{
    return new Points(_core, getName());
}

unsigned int PointData::getNumPoints() const
{
    return _vectorHolder.size() / _numDimensions;
}

unsigned int PointData::getNumDimensions() const
{
    return _numDimensions;
}

const std::vector<float>& PointData::getData() const
{
    return _vectorHolder.getConstVector<float>();
}

const std::vector<QString>& PointData::getDimensionNames() const
{
    return _dimNames;
}

void PointData::setData(const std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    _vectorHolder.resize(numPoints * numDimensions);
    _numDimensions = numDimensions;
}

void PointData::setDimensionNames(const std::vector<QString>& dimNames)
{
    _dimNames = dimNames;
}

// Constant subscript indexing
const float& PointData::operator[](unsigned int index) const
{
    return _vectorHolder.getConstVector<float>()[index];
}

// Subscript indexing
float& PointData::operator[](unsigned int index)
{
    return _vectorHolder.getVector<float>()[index];
}

void PointData::extractFullDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    CheckDimensionIndex(dimensionIndex);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                result[i] = vec[i * _numDimensions + dimensionIndex];
            }
        });
}


void PointData::extractFullDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(getNumPoints());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = i * _numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}


void PointData::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2, const std::vector<unsigned int>& indices) const
{
    CheckDimensionIndex(dimensionIndex1);
    CheckDimensionIndex(dimensionIndex2);

    result.resize(indices.size());

    _vectorHolder.constVisit(
        [&result, this, dimensionIndex1, dimensionIndex2, indices](const auto& vec)
        {
            const auto resultSize = result.size();

            for (std::size_t i{}; i < resultSize; ++i)
            {
                const auto n = std::size_t{ indices[i] } *_numDimensions;
                result[i].set(vec[n + dimensionIndex1], vec[n + dimensionIndex2]);
            }
        });
}

// =============================================================================
// Point Set
// =============================================================================


const std::vector<float>& Points::getData() const
{
    return getRawData<PointData>().getData();
}


void Points::setData(std::nullptr_t, const std::size_t numPoints, const std::size_t numDimensions)
{
    getRawData<PointData>().setData(nullptr, numPoints, numDimensions);
}

void Points::extractDataForDimension(std::vector<float>& result, const int dimensionIndex) const
{
    // This overload assumes that the data set is "full".
    // Please remove the assert once non-full support is implemented (if necessary).
    assert(isFull());

    const auto& rawPointData = getRawData<PointData>();
    rawPointData.extractFullDataForDimension(result, dimensionIndex);
}


void Points::extractDataForDimensions(std::vector<hdps::Vector2f>& result, const int dimensionIndex1, const int dimensionIndex2) const
{
    const auto& rawPointData = getRawData<PointData>();

    if (isFull())
    {
        rawPointData.extractFullDataForDimensions(result, dimensionIndex1, dimensionIndex2);
    }
    else
    {
        rawPointData.extractDataForDimensions(result, dimensionIndex1, dimensionIndex2, indices);
    }
}


hdps::DataSet* Points::copy() const
{
    Points* set = new Points(_core, getDataName());
    set->setName(getName());
    set->indices = indices;
    return set;
}

void Points::createSubset() const
{
    const hdps::DataSet& selection = _core->requestSelection(getDataName());

    _core->createSubsetFromSelection(selection, getDataName(), "Subset");
}

const std::vector<QString>& Points::getDimensionNames() const
{
    return getRawData<PointData>().getDimensionNames();
}

void Points::setDimensionNames(const std::vector<QString>& dimNames)
{
    getRawData<PointData>().setDimensionNames(dimNames);
}

// Constant subscript indexing
const float& Points::operator[](unsigned int index) const
{
    return getRawData<PointData>()[index];
}

// Subscript indexing
float& Points::operator[](unsigned int index)
{
    return getRawData<PointData>()[index];
}

// =============================================================================
// Factory
// =============================================================================

hdps::RawData* PointDataFactory::produce()
{
    return new PointData();
}
