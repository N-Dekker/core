#include "ScatterplotDrawer.h"

#include <QOpenGLFunctions>
#include <QDebug>

#include "ShaderLoader.h"

#include <vector>
#include <cstdlib>

#define sign(x) (x < 0 ? -1 : 1)
#define GLSL(version, shader)  "#version " #version "\n" #shader

void ScatterplotDrawer::initializeGL()
{
    initializeOpenGLFunctions();

    glClearColor(1.0, 1.0, 1.0, 1.0);
    qDebug() << "Initializing scatterplot";

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    const char *vertexSource = GLSL(330,
        in vec4 position;
        in vec2 texCoords;
        in vec2 offset;

        out vec2 pass_texCoords;

        void main()
        {
            gl_Position = position + vec4(offset, 0, 0);
            pass_texCoords = texCoords;
        }
    );

    const char *fragmentSource = GLSL(330,
        in vec2 pass_texCoords;

        out vec4 fragColor;

        void main()
        {
            if (length(pass_texCoords) > 0.01)
            {
                //discard;
            }
            float a = smoothstep(0.05, 0.04, length(pass_texCoords));
            fragColor = vec4(0, 0.5, 1.0, a / 2.0);
        }
    );

    float vertices[6*2] = 
    {
        -0.5, -0.5,
         0.5, -0.5,
        -0.5,  0.5,
        -0.5,  0.5,
         0.5, -0.5,
         0.5,  0.5
    };

    float texCoords[6*2] =
    {
        -1, -1,
         1, -1,
        -1,  1,
        -1,  1,
         1, -1,
         1,  1
    };

    std::vector<float> positions;

    srand(0);
    for (int i = 0; i < 1000; i++)
    {
        float x = (float) (rand() % 1000 - 500) / 500;
        float y = (float) (rand() % 1000 - 500) / 500;
        
        positions.push_back(sign(x) * (x*x));
        positions.push_back(sign(y) * (y*y));
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    GLuint tbo;
    glGenBuffers(1, &tbo);
    glBindBuffer(GL_ARRAY_BUFFER, tbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), texCoords, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    GLuint pbo;
    glGenBuffers(1, &pbo);
    glBindBuffer(GL_ARRAY_BUFFER, pbo);
    glBufferData(GL_ARRAY_BUFFER, positions.size() * 2 * sizeof(float), positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(2);

    //shader = ShaderLoader::loadShaderProgram();

    shader.addShaderFromSourceCode(QOpenGLShader::Vertex, vertexSource);
    shader.addShaderFromSourceCode(QOpenGLShader::Fragment, fragmentSource);
    shader.link();
}

void ScatterplotDrawer::resizeGL(int w, int h)
{

}

void ScatterplotDrawer::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT);
    qDebug() << "Rendering scatterplot";

    shader.bind();
    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, 1000);
}
