#include "Shader.h"

#include "Vector3f.h"
#include "Matrix3f.h"
//#include "Util/Matrix4f.h"
#include "Util/FileUtil.h"

#include <QDebug>

namespace hdps
{

namespace
{
    const int LOG_SIZE = 1024;

    bool compileShader(QString path, GLuint shader)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

        f->glCompileShader(shader);

        char log[LOG_SIZE];
        GLint status;
        f->glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
        if (status == GL_FALSE) {
            f->glGetShaderInfoLog(shader, LOG_SIZE, nullptr, log);
            qDebug() << "Shader failed to compile: " << path << '\n' + log;
            return false;
        }
        return true;
    }

    bool linkProgram(const GLuint program)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

        f->glLinkProgram(program);

        GLint status = 0;
        f->glGetProgramiv(program, GL_LINK_STATUS, &status);

        if (status != GL_TRUE) {
            GLint logLength = 0;
            f->glGetShaderiv(program, GL_INFO_LOG_LENGTH, &logLength);
            qDebug() << logLength;
            std::vector<GLchar> infoLog(logLength);
            f->glGetProgramInfoLog(program, logLength, &logLength, infoLog.data());
            
            qDebug() << "Shader program failed to link: " << std::string(infoLog.begin(), infoLog.end()).c_str();
            return false;
        }

        return true;
    }

    bool validateProgram(const GLuint program)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

        f->glValidateProgram(program);

        GLint status = 0;
        f->glGetProgramiv(program, GL_VALIDATE_STATUS, &status);

        return status == GL_TRUE;
    }

    bool loadShader(QString path, int type, GLuint& shader)
    {
        QOpenGLFunctions_3_3_Core* f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

        QString source = hdps::util::loadFileContents(path);
        if (source.isEmpty()) return false;

        std::string ssource = source.toStdString();
        const char* csource = ssource.c_str();

        // Create the shader
        shader = f->glCreateShader(type);
        f->glShaderSource(shader, 1, &csource, NULL);
        
        return compileShader(path, shader);
    }
}

ShaderProgram::ShaderProgram() {
    
}

ShaderProgram::~ShaderProgram() {
    glDeleteProgram(_handle);
}

void ShaderProgram::bind() {
    glUseProgram(_handle);
}

void ShaderProgram::release() {
    glUseProgram(0);
}

void ShaderProgram::destroy() {
    glDeleteProgram(_handle);
}

void ShaderProgram::uniform1i(const char* name, int value) {
    glUniform1i(location(name), value);
}

void ShaderProgram::uniform1iv(const char* name, int count, int* values) {
    glUniform1iv(location(name), count, (GLint*)values);
}

void ShaderProgram::uniform2i(const char* name, int v0, int v1) {
    glUniform2i(location(name), v0, v1);
}

void ShaderProgram::uniform1f(const char* name, float value) {
    glUniform1f(location(name), value);
}

void ShaderProgram::uniform2f(const char* name, float v0, float v1) {
    glUniform2f(location(name), v0, v1);
}

void ShaderProgram::uniform3f(const char* name, float v0, float v1, float v2) {
    glUniform3f(location(name), v0, v1, v2);
}

void ShaderProgram::uniform3f(const char* name, Vector3f v) {
    glUniform3f(location(name), v.x, v.y, v.z);
}

void ShaderProgram::uniform3fv(const char* name, int count, Vector3f* v) {
    glUniform3fv(location(name), count, (GLfloat*)v);
}

void ShaderProgram::uniform4f(const char* name, float v0, float v1, float v2, float v3) {
    glUniform4f(location(name), v0, v1, v2, v3);
}

void ShaderProgram::uniformMatrix3f(const char* name, Matrix3f& m) {
    glUniformMatrix3fv(location(name), 1, false, m.toArray());
}

//void Shader::uniformMatrix4f(const char* name, Matrix4f& m) {
//    glUniformMatrix4fv(location(name), 1, false, m.toArray());
//}

int ShaderProgram::location(const char* name) {
    std::unordered_map<std::string, int>::const_iterator it = _locationMap.find(std::string(name));
    if (it != _locationMap.end()) {
        return it->second;
    }
    else {
        int location = glGetUniformLocation(_handle, name);
        _locationMap[std::string(name)] = location;
        return location;
    }
}

bool ShaderProgram::loadShaderFromFile(QString vertPath, QString fragPath) {
    initializeOpenGLFunctions();

    bool success = true;

    GLuint vertexShader, fragmentShader;
    success &= loadShader(vertPath, GL_VERTEX_SHADER, vertexShader);
    success &= loadShader(fragPath, GL_FRAGMENT_SHADER, fragmentShader);

    if (!success) return false;

    _handle = glCreateProgram();

    glAttachShader(_handle, vertexShader);
    glAttachShader(_handle, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    success &= linkProgram(_handle);
    success &= validateProgram(_handle);

    return success;
}

}
