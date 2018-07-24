#include "DensityRenderer.h"

namespace hdps
{
    namespace gui
    {

        DensityRenderer::DensityRenderer(RenderMode renderMode)
            :
            _renderMode(renderMode)
        {

        }

        DensityRenderer::~DensityRenderer()
        {
            // Delete objects
            _densityComputation.cleanup();
        }

        void DensityRenderer::setRenderMode(RenderMode renderMode)
        {
            _renderMode = renderMode;
        }

        // Points need to be passed as a pointer as we need to store them locally in order
        // to be able to recompute the densities when parameters change.
        void DensityRenderer::setData(const std::vector<Vector2f>* points)
        {
            _densityComputation.setData(points);
        }

        void DensityRenderer::setSigma(const float sigma)
        {
            _densityComputation.setSigma(sigma);
        }

        void DensityRenderer::setColormap(const QString colormap)
        {
            _colormap.loadFromFile(colormap);
            _hasColorMap = true;
        }

        void DensityRenderer::init()
        {
            qDebug() << "Initializing density plot GL";
            initializeOpenGLFunctions();

            // Create a simple VAO for full-screen quad rendering
            glGenVertexArrays(1, &_quad);

            // Load the necessary shaders for density drawing
            bool loaded = true;
            loaded &= _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
            loaded &= _shaderIsoDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/IsoDensityDraw.frag");
            if (!loaded) {
                qDebug() << "Failed to load one of the Density shaders";
            }

            // Initialize the density computation
            _densityComputation.init(QOpenGLContext::currentContext());
            // Compute the density in case data was already set
            _densityComputation.compute();
            qDebug() << "Initialized density plot GL";
        }

        void DensityRenderer::resize(QSize renderSize)
        {
            int w = renderSize.width();
            int h = renderSize.height();

            _windowSize.setWidth(w);
            _windowSize.setHeight(h);
        }

        void DensityRenderer::render()
        {
            qDebug() << "Rendering densityplot";

            glViewport(0, 0, _windowSize.width(), _windowSize.height());

            int w = _windowSize.width();
            int h = _windowSize.height();
            int size = w < h ? w : h;
            glViewport(w / 2 - size / 2, h / 2 - size / 2, size, size);

            // Draw density or isolines map
            switch (_renderMode) {
            case DENSITY: drawDensity(); break;
            case LANDSCAPE: drawLandscape(); break;
            }
            qDebug() << "Done rendering densityplot";
        }

        void DensityRenderer::destroy()
        {
            _shaderDensityDraw.destroy();
            _shaderIsoDensityDraw.destroy();
            _densityComputation.cleanup();
            _colormap.destroy();

            glDeleteVertexArrays(1, &_quad);
        }

        void DensityRenderer::drawFullscreenQuad()
        {
            glBindVertexArray(_quad);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glBindVertexArray(0);
        }

        void DensityRenderer::drawDensity()
        {
            float maxDensity = _densityComputation.getMaxDensity();
            if (maxDensity <= 0) { return; }

            _shaderDensityDraw.bind();

            _densityComputation.getDensityTexture().bind(0);
            _shaderDensityDraw.uniform1i("tex", 0);
            _shaderDensityDraw.uniform1f("norm", 1 / maxDensity);

            drawFullscreenQuad();
        }

        void DensityRenderer::drawLandscape()
        {
            if (!_hasColorMap)
                return;

            float maxDensity = _densityComputation.getMaxDensity();
            if (maxDensity <= 0) { return; }

            _shaderIsoDensityDraw.bind();

            _densityComputation.getDensityTexture().bind(0);
            _shaderIsoDensityDraw.uniform1i("tex", 0);

            _shaderIsoDensityDraw.uniform2f("renderParams", 1.0f / maxDensity, 1.0f / _densityComputation.getNumPoints());

            _colormap.bind(1);
            _shaderIsoDensityDraw.uniform1i("colormap", 1);

            drawFullscreenQuad();
        }

        void DensityRenderer::onSelecting(Selection selection)
        {
            _selection = selection;

            _isSelecting = true;
        }

        void DensityRenderer::onSelection(Selection selection)
        {
            _isSelecting = false;
        }

    } // namespace gui

} // namespace hdps
