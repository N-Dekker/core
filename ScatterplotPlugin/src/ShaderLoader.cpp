/* ---------------------------------------------------------------------------
** This software is in the public domain, furnished "as is", without technical
** support, and with no warranty, express or implied, as to its usefulness for
** any purpose.
**
** ShaderLoader.cpp
** Implements a class for loading vertex and fragment shaders and compiling
** them into a shader program.
**
** Author: Julian Thijssen
** -------------------------------------------------------------------------*/

#include "ShaderLoader.h"

#include <QOpenGLFunctions_3_3_Core.h>
#include <QDebug>

#define GLSL(version, shader)  "#version " #version "\n" #shader

const int ShaderLoader::LOG_SIZE = 1024;

int ShaderLoader::loadShaderProgram()
{
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

    QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    int vertexShader = loadShader(vertexSource, GL_VERTEX_SHADER);
    int fragmentShader = loadShader(fragmentSource, GL_FRAGMENT_SHADER);

    int shaderProgram = f->glCreateProgram();

    f->glAttachShader(shaderProgram, vertexShader);
    f->glAttachShader(shaderProgram, fragmentShader);

    f->glBindAttribLocation(shaderProgram, 0, "position");
    f->glBindAttribLocation(shaderProgram, 1, "texCoords");
    f->glBindAttribLocation(shaderProgram, 2, "offset");

    f->glLinkProgram(shaderProgram);

    checkLinkStatus(shaderProgram);

    f->glValidateProgram(shaderProgram);

    qDebug() << "Successfully compiled shader program";

    return shaderProgram;
}

int ShaderLoader::loadShader(const char* source, int type)
{
    QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    int handle = 0;

    // Create the shader
    handle = f->glCreateShader(type);
    f->glShaderSource(handle, 1, &source, NULL);
    f->glCompileShader(handle);

    checkCompilationStatus(handle);

    return handle;
}

void ShaderLoader::checkCompilationStatus(int shader)
{
    QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    char log[LOG_SIZE];
    GLint status;
    f->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        f->glGetShaderInfoLog(shader, LOG_SIZE, nullptr, log);
        qDebug() << "Failed to compile shader" << log;
    }
}

void ShaderLoader::checkLinkStatus(const int program)
{
    QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

    GLint status = 0;
    f->glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        qFatal("Shader linking failed");
    }
}
