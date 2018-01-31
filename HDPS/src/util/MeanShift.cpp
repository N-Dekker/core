#include "MeanShift.h"

namespace hdps
{

void GaussianTexture::generate()
{
    int w = 32;
    int h = 32;

    float* data = new float[w * h];

    double sigma = w / 6.0;
    double i_c = w / 2.0;
    double j_c = h / 2.0;

    for (int j = 0; j < h; j++){
        for (int i = 0; i < w; i++){

            const double sqrt_dist = (i - i_c)*(i - i_c) + (j - j_c)*(j - j_c);
            const double val = exp(sqrt_dist / (-2.0 * sigma * sigma)) / (2.0 * 3.1415926535 * sigma * sigma);

            data[j*w + i] = val*1000.0;// texture_multiplier;
        }
    }

    glBindTexture(GL_TEXTURE_2D, _handle);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, w, h, 0, GL_RED, GL_FLOAT, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    delete[] data;
}

void MeanShift::init()
{
    initializeOpenGLFunctions();

    glClearColor(1, 1, 1, 1);
    qDebug() << "Initializing density plot";

    _gaussTexture.create();
    _gaussTexture.generate();

    glGenVertexArrays(1, &_quad);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    std::vector<float> quad(
    {
        -1, -1, 0, 0,
        1, -1, 1, 0,
        -1, 1, 0, 1,
        -1, 1, 0, 1,
        1, -1, 1, 0,
        1, 1, 1, 1
    }
    );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    BufferObject quadBuffer;
    quadBuffer.create();
    quadBuffer.bind();
    quadBuffer.setData(quad);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)(sizeof(float) * 2));
    glEnableVertexAttribArray(1);

    _positionBuffer.create();
    _positionBuffer.bind();
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    bool loaded = _shaderDensityCompute.loadShaderFromFile(":shaders/DensityCompute.vert", ":shaders/DensityCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load DensityCompute shader";
    }

    loaded = _shaderGradientCompute.loadShaderFromFile(":shaders/Quad.vert", ":shaders/GradientCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load GradientCompute shader";
    }

    loaded = _shaderMeanshiftCompute.loadShaderFromFile(":shaders/Quad.vert", ":shaders/MeanshiftCompute.frag");
    if (!loaded) {
        qDebug() << "Failed to load MeanshiftCompute shader";
    }

    _meanshiftFramebuffer.create();
    _meanshiftFramebuffer.bind();

    _densityTexture.create();
    _densityTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _msTexSize, _msTexSize, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(0, &_densityTexture);
    _meanshiftFramebuffer.validate();


    _gradientTexture.create();
    _gradientTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _msTexSize, _msTexSize, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(1, &_gradientTexture);
    _meanshiftFramebuffer.validate();

    _meanshiftTexture.create();
    _meanshiftTexture.bind();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, _msTexSize, _msTexSize, 0, GL_RGB, GL_FLOAT, NULL);

    _meanshiftFramebuffer.addColorTexture(2, &_meanshiftTexture);
    _meanshiftFramebuffer.validate();
}

void MeanShift::cleanup()
{
    _gaussTexture.destroy();

    glDeleteVertexArrays(1, &_vao);
    _positionBuffer.destroy();
}

void MeanShift::setData(const std::vector<Vector2f>* points)
{
    _numPoints = (unsigned int) points->size();

    _positionBuffer.bind();
    _positionBuffer.setData(*points);
}

void MeanShift::drawFullscreenQuad()
{
    glBindVertexArray(_quad);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glBindVertexArray(0);
}

void MeanShift::computeDensity()
{
    if (_numPoints == 0) return;
    qDebug() << "Computing density";
    glViewport(0, 0, _msTexSize, _msTexSize);

    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    // Set background color
    glClearColor(0, 0, 0, 1);
    // Clear fbo
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Enable additive blending
    glBlendFunc(GL_ONE, GL_ONE);

    // Bind shader
    _shaderDensityCompute.bind();

    // Set sigma uniform
    _shaderDensityCompute.uniform1f("sigma", _sigma*2);

    // Set advanced parameters uniform
    //>>>> float params[4] = { (_isScaleAvailable ? 1.0f / _maxScale : -1.0f), 0.0f, 0.0f, 0.0f };
    //>>>> _offscreenDensityShader.setParameter4fv(advancedParamsUniform, params);

    // Set gauss texture
    _gaussTexture.bind(0);
    _shaderDensityCompute.uniform1i("gaussSampler", 0);

    // Set the texture containing flags for the active state of each sample 
    //>>>> _offscreenDensityShader.uniform1i("activeSampleSampler", 1);
    //>>>> glActiveTexture(GL_TEXTURE1);
    //>>>> glBindTexture(GL_TEXTURE_1D, _activeSampleTexture);

    // Bind the vao
    glBindVertexArray(_vao);

    // Bind file ID buffer
    //if (_isFileIdxAvailable)
    //{
    //    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferSample);
    //    glVertexAttribIPointer(sampleIdAttribute, 1, GL_INT, 0, 0);
    //    glEnableVertexAttribArray(sampleIdAttribute);
    //}

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, _numPoints);

    // Unbind vao
    glBindVertexArray(0);



    // Read pixels from framebuffer
    std::vector<float> kde(_msTexSize * _msTexSize * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT0);
    glReadPixels(0, 0, _msTexSize, _msTexSize, GL_RGB, GL_FLOAT, kde.data());

    // Calculate max value for normalization
    _maxKDE = -99999999.9f;
    for (int i = 0; i < kde.size(); i += 3) // only lookup red channel
    {
        _maxKDE = std::max(_maxKDE, kde[i]);
    }

    qDebug() << "	Max KDE Value = " << _maxKDE << ".\n";
    qDebug() << "Done computing density";
    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

void MeanShift::computeGradient()
{
    if (_numPoints == 0) return;

    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT1);

    glViewport(0, 0, _msTexSize, _msTexSize);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderGradientCompute.bind();

    _densityTexture.bind(0);
    _shaderGradientCompute.uniform1i("densityTexture", 0);

    _shaderGradientCompute.uniform2f("renderParams", 1.0f / _maxKDE, 1.0f / 1000);

    drawFullscreenQuad();

    _shaderGradientCompute.release();

    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
}

void MeanShift::computeMeanShift()
{
    if (_numPoints == 0) return;

    _meanshiftFramebuffer.bind();
    glDrawBuffer(GL_COLOR_ATTACHMENT2);

    glViewport(0, 0, _msTexSize, _msTexSize);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shaderMeanshiftCompute.bind();

    _gradientTexture.bind(0);
    _shaderMeanshiftCompute.uniform1i("gradientTexture", 0);

    _shaderMeanshiftCompute.uniform4f("renderParams", 0.25f, 0.0f, 1.0f / _msTexSize, 1.0f / _msTexSize);

    drawFullscreenQuad();

    _shaderMeanshiftCompute.release();

    _meanShiftMapCPU.resize(_msTexSize * _msTexSize * 3);

    glReadBuffer(GL_COLOR_ATTACHMENT2);
    glReadPixels(0, 0, _msTexSize, _msTexSize, GL_RGB, GL_FLOAT, _meanShiftMapCPU.data());

    //glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
    qDebug() << "Computing meanshift";
    //{
    //	double t = 0.0;
    //	{
    //		nut::ScopedTimer<double> timer(t);
    //
    //		for (int i = 0; i < 100; i++)
    //		{
    //			clusterOld();
    //		}
    //	}
    //	std::cout << "	Clustered 100 times in old mode in " << t << "\n";
    //	t = 0.0;
    //	{
    //		nut::ScopedTimer<double> timer(t);
    //
    //		for (int i = 0; i < 100; i++)
    //		{
    //			cluster();
    //		}
    //	}
    //	std::cout << "	Clustered 100 times in new mode in " << t << "\n";
    //}

    //cluster();
}

Texture2D& MeanShift::getDensityTexture()
{
    return _densityTexture;
}

Texture2D& MeanShift::getGradientTexture()
{
    return _gradientTexture;
}

Texture2D& MeanShift::getMeanShiftTexture()
{
    return _meanshiftTexture;
}

} // namespace hdps
