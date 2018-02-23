#include "DensityPlotWidget.h"

#include <vector>
#include <algorithm>
#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

namespace hdps
{
namespace gui
{

void DensityPlotWidget::setRenderMode(RenderMode renderMode)
{
    _renderMode = renderMode;

    update();
}

// Positions need to be passed as a pointer as we need to store them locally in order
// to be able to find the subset of data that's part of a selection. If passed
// by reference then we can upload the data to the GPU, but not store it in the widget.
void DensityPlotWidget::setData(const std::vector<Vector2f>* positions)
{
    _numPoints = (unsigned int)positions->size();

    qDebug() << "Setting position data";
    _meanShift.setData(positions);

    update();
}

void DensityPlotWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(context(), &QOpenGLContext::aboutToBeDestroyed, this, &DensityPlotWidget::cleanup);

    glClearColor(1, 1, 1, 1);
    qDebug() << "Initializing density plot";

    _meanShift.init();

    bool loaded = true;
    loaded &= _shaderDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/DensityDraw.frag");
    loaded &= _shaderGradientDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientDraw.frag");
    loaded &= _shaderMeanShiftDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/Texture.frag");
    loaded &= _shaderIsoDensityDraw.loadShaderFromFile(":shaders/Quad.vert", ":shaders/IsoDensityDraw.frag");
    if (!loaded) {
        qDebug() << "Failed to load one of the MeanShift shaders";
    }

    colorMap.loadFromFile(":colormaps/Spectral.png");
}

void DensityPlotWidget::resizeGL(int w, int h)
{
    _windowSize.setWidth(w);
    _windowSize.setHeight(h);
}

void DensityPlotWidget::paintGL()
{
    std::vector<std::vector<unsigned int>> clusters;

    _meanShift.cluster(clusters);

    glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    glViewport(0, 0, _windowSize.width(), _windowSize.height());

    glClearColor(1, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    switch (_renderMode) {
        case DENSITY: drawDensity(); break;
        case GRADIENT: drawGradient(); break;
        case PARTITIONS: drawMeanShift(); break;
        case LANDSCAPE: drawLandscape(); break;
    }
}

void DensityPlotWidget::drawDensity()
{
    if (_numPoints == 0) return;

    _shaderDensityDraw.bind();

    _meanShift.getDensityTexture().bind(0);
    _shaderDensityDraw.uniform1i("tex", 0);
    _shaderDensityDraw.uniform1f("norm", 1 / _meanShift.getMaxDensity());

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawGradient()
{
    if (_numPoints == 0) return;

    _shaderGradientDraw.bind();

    _meanShift.getGradientTexture().bind(0);
    _shaderGradientDraw.uniform1i("tex", 0);

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawMeanShift()
{
    if (_numPoints == 0) return;

    _shaderMeanShiftDraw.bind();

    _meanShift.getMeanShiftTexture().bind(0);
    _shaderMeanShiftDraw.uniform1i("tex", 0);

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::drawLandscape()
{
    if (_numPoints == 0) return;

    _shaderIsoDensityDraw.bind();

    _meanShift.getDensityTexture().bind(0);
    _shaderIsoDensityDraw.uniform1i("tex", 0);

    _shaderIsoDensityDraw.uniform4f("renderParams", 1.0f / _meanShift.getMaxDensity(), 0, 1.0f / _numPoints, 0);

    colorMap.bind(1);
    _shaderIsoDensityDraw.uniform1i("colorMap", 1);

    _meanShift.drawFullscreenQuad();
}

void DensityPlotWidget::cleanup()
{
    qDebug() << "Deleting density plot widget, performing clean up...";
    makeCurrent();

    // Delete objects
    _meanShift.cleanup();
}

} // namespace gui

} // namespace hdps
