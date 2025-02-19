// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#pragma once

#include <QOpenGLFunctions_3_3_Core>

#include <vector>

namespace mv
{

class BufferObject : protected QOpenGLFunctions_3_3_Core
{
public:
    BufferObject();
    ~BufferObject() override;

    void create();
    void bind();
    template<typename T>
    void setData(const std::vector<T>& data)
    {
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
    }

    void destroy();
private:
    GLuint _object;
};

} // namespace mv
