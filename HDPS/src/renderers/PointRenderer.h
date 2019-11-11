#pragma once

#include "Renderer.h"
#include "../SelectionListener.h"

#include "../graphics/BufferObject.h"

#include "../graphics/Vector2f.h"
#include "../graphics/Vector3f.h"
#include "../graphics/Matrix3f.h"
#include "../graphics/Texture.h"

#include <QRectF>

namespace hdps
{
    namespace gui
    {
        enum PointScaling {
            Absolute, Relative
        };

        enum PointEffect {
            Color, Size, Outline
        };

        struct PointArrayObject : private QOpenGLFunctions_3_3_Core
        {
        public:
            GLuint _handle;

            BufferObject _positionBuffer;
            BufferObject _highlightBuffer;
            BufferObject _scalarBuffer;

            void init();
            void setPositions(const std::vector<Vector2f>& positions);
            void setHighlights(const std::vector<char>& highlights);
            void setScalars(const std::vector<float>& scalars);
            void enableHighlights(bool enable);
            void enableScalars(bool enable);
            bool hasHighlights() { return _hasHighlights; }
            bool hasScalars() { return _hasScalars; }
            Vector3f getScalarRange() { return Vector3f(_scalarLow, _scalarHigh, _scalarRange); }
            void draw();
            void destroy();

        private:
            /* Point attributes */
            std::vector<Vector2f> _positions;
            std::vector<char>     _highlights;
            std::vector<float>    _scalars;

            float _scalarLow;
            float _scalarHigh;
            float _scalarRange;

            bool _hasPositions = false;
            bool _hasHighlights = false;
            bool _hasScalars = false;

            bool _dirtyPositions = false;
            bool _dirtyHighlights = false;
            bool _dirtyScalars = false;
        };

        struct PointSettings
        {
            // Constants
            const float         DEFAULT_POINT_SIZE       = 15;
            const float         DEFAULT_ALPHA_VALUE      = 0.5f;
            const PointScaling  DEFAULT_POINT_SCALING    = PointScaling::Relative;

            PointScaling        _scalingMode             = DEFAULT_POINT_SCALING;
            float               _pointSize               = DEFAULT_POINT_SIZE;
            float               _alpha                   = DEFAULT_ALPHA_VALUE;
        };

        class PointRenderer : public Renderer
        {
        public:
            void setData(const std::vector<Vector2f>& points);
            void setHighlights(const std::vector<char>& highlights);
            void setScalars(const std::vector<float>& scalars);

            void setScalarEffect(const PointEffect effect);
            void setColormap(const QString colormap);
            void setBounds(float left, float right, float bottom, float top);
            void setPointSize(const float size);
            void setAlpha(const float alpha);
            void setPointScaling(PointScaling scalingMode);
            void setOutlineColor(Vector3f color);

            void init() override;
            void resize(QSize renderSize) override;
            void render() override;
            void destroy() override;

        private:
            /* Point properties */
            PointSettings _pointSettings;
            PointEffect   _pointEffect = PointEffect::Size;
            Vector3f      _outlineColor = Vector3f(0, 0, 1);

            /* Window properties */
            QSize _windowSize;

            /* Rendering variables */
            ShaderProgram _shader;

            PointArrayObject _gpuPoints;
            Texture2D _colormap;

            Matrix3f _orthoM;
            QRectF _bounds = QRectF(-1, 1, 2, 2);
        };

    } // namespace gui

} // namespace hdps
