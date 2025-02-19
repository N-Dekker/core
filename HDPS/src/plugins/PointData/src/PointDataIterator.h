// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#ifndef HDPS_POINTDATAITERATOR_H
#define HDPS_POINTDATAITERATOR_H

#include <iterator>

#include "PointView.h"

namespace mv
{
    template <typename ValueIteratorType, typename IndexIteratorType, typename IndexFunctionType>
    class PointDataIterator
    {
        // Its data members: 
        ValueIteratorType _valueIterator{};
        IndexIteratorType _indexIterator{};
        unsigned _numberOfDimensions{};
        IndexFunctionType _indexFunction;

        using PointViewType = PointView<ValueIteratorType>;
    public:
        // Types conforming the iterator requirements of the C++ standard library:
        using difference_type = std::ptrdiff_t;
        using value_type = PointViewType;
        using reference = PointViewType;
        using pointer = const PointViewType*;
        using iterator_category = std::random_access_iterator_tag;


        /* Explicitly defaulted default-constructor
        */
        PointDataIterator() = default;


        PointDataIterator(
            const ValueIteratorType valueIterator,
            const IndexIteratorType indexIterator,
            const unsigned numberOfDimensions,
            const IndexFunctionType indexFunction)
            :
            _valueIterator{ valueIterator },
            _indexIterator{ indexIterator },
            _numberOfDimensions{ numberOfDimensions },
            _indexFunction{ indexFunction }
        {
        }


        /**  Returns a PointView object to the current point.
        */
        auto operator*() const
        {
            const auto index = _indexFunction(_indexIterator);
            const auto begin = _valueIterator + (index * _numberOfDimensions);
            const auto end = begin + _numberOfDimensions;
            return PointViewType(begin, end, index);
        }


        /** Prefix increment ('++it').
        */
        auto& operator++()
        {
            ++(_indexIterator);
            return *this;
        }


        /** Postfix increment ('it++').
         * \note Usually prefix increment ('++it') is preferable.
         */
        auto operator++(int)
        {
            auto result = *this;
            ++(*this);
            return result;
        }


        /** Prefix decrement ('--it').
        */
        auto& operator--()
        {
            --_indexIterator;
            return *this;
        }


        /** Postfix increment ('it--').
         * \note  Usually prefix increment ('--it') is preferable.
         */
        auto operator--(int)
        {
            auto result = *this;
            --(*this);
            return result;
        }



        /** Does (it += n) for iterator 'it' and integer value 'n'.
        */
        friend auto& operator+=(PointDataIterator& it, const difference_type n)
        {
            it._indexIterator += n;
            return it;
        }


        /** Returns (it1 - it2) for iterators it1 and it2.
        */
        friend difference_type operator-(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return it1._indexIterator - it2._indexIterator;
        }


        /** Returns (it + n) for iterator 'it' and integer value 'n'.
        */
        friend auto operator+(PointDataIterator it, const difference_type n)
        {
            return it += n;
        }


        /** Returns (n + it) for iterator 'it' and integer value 'n'.
        */
        friend auto operator+(const difference_type n, PointDataIterator it)
        {
            return it += n;
        }


        /** Returns (it - n) for iterator 'it' and integer value 'n'.
        */
        friend auto operator-(PointDataIterator it, const difference_type n)
        {
            return it += (-n);
        }


        /** Returns it[n] for iterator 'it' and integer value 'n'.
        */
        auto operator[](const difference_type n) const
        {
            return *(*this + n);
        }


        /** Returns (it1 == it2) for iterators it1 and it2.
        */
        friend bool operator==(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return it1._indexIterator == it2._indexIterator;
        }


        /** Returns (it1 != it2) for iterators it1 and it2.
        */
        friend bool operator!=(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return ! (it1 == it2);
        }


        /** Returns (it1 < it2) for iterators it1 and it2.
        */
        friend bool operator<(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            return it1._indexIterator < it2._indexIterator;
        }


        /** Returns (it1 > it2) for iterators it1 and it2.
        */
        friend bool operator>(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            // Implemented just like the corresponding std::rel_ops operator.
            return it2 < it1;
        }


        /** Returns (it1 <= it2) for iterators it1 and it2.
        */
        friend bool operator<=(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            // Implemented just like the corresponding std::rel_ops operator.
            return !(it2 < it1);
        }


        /** Returns (it1 >= it2) for iterators it1 and it2.
        */
        friend bool operator>=(const PointDataIterator& it1, const PointDataIterator& it2)
        {
            // Implemented just like the corresponding std::rel_ops operator.
            return !(it1 < it2);
        }


        /** Does (it -= n) for iterator 'it' and integer value 'n'.
        */
        friend auto& operator-=(PointDataIterator& it, const difference_type n)
        {
            it += (-n);
            return it;
        }


        /** Returns the index of the current point.
        */
        friend auto index(const PointDataIterator& arg)
        {
            return arg._indexFunction(arg._indexIterator);
        }

    };

}


#endif // HDPS_POINTDATAITERATOR_H
