// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

/**
    Custom bounds class because the QRectF class in Qt should kindly remove itself from the library.

    This bounds class:
        * Has a bottom-left anchor point.
        * Does not allow negative sizes
        * Does not secretly clamp values set by the user
*/

#include "Vector2f.h"

namespace mv
{
    class Bounds
    {
    public:
        const static Bounds Max;

        Bounds();
        Bounds(float left, float right, float bottom, float top);

        void setBounds(float left, float right, float bottom, float top);
        void ensureMinimumSize(float width, float height);
        void moveToOrigin();
        void makeSquare();
        void expand(float fraction);

        float getWidth() const { return _right - _left; }
        float getHeight() const { return _top - _bottom; }
        Vector2f getCenter() const { return { (_left + _right) / 2, (_bottom + _top) / 2 }; }

        float getLeft() const { return _left; }
        float getRight() const { return _right; }
        float getBottom() const { return _bottom; }
        float getTop() const { return _top; }

        void setLeft(float left) { _left = left; }
        void setRight(float right) { _right = right; }
        void setBottom(float bottom) { _bottom = bottom; }
        void setTop(float top) { _top = top; }

    private:
        float _left;
        float _right;
        float _bottom;
        float _top;
    };
}
