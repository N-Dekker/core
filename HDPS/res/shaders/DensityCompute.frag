// SPDX-License-Identifier: LGPL-3.0-or-later 
// A corresponding LICENSE file is located in the root directory of this source tree 
// Copyright (C) 2023 BioVault (Biomedical Visual Analytics Unit LUMC - TU Delft) 

#version 330 core

uniform sampler2D gaussSampler;

in vec2 pass_texCoord;

out float value;

void main() {
    value = texture(gaussSampler, pass_texCoord).r;
}
