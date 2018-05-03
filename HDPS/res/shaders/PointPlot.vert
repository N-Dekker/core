#version 330 core

uniform float pointSize;
uniform mat3 projMatrix;
uniform int scalarEffect;

in vec2 vertex;
in vec2 position;
in vec3 color;
in uint highlight;
in float scalar;

out vec2 pass_texCoords;
out vec3 pass_color;
flat out uint pass_highlight;
out float pass_scalar;

void main()
{
    pass_color = color;
    pass_highlight = highlight;
    pass_scalar = scalar;
    
    pass_texCoords = vertex;
    
    float scale = 1.0;
    if (pass_highlight == 1u) {
        scale *= 1.2;
    }
    if (scalarEffect == 1) {
        scale *= scalar;
    }
    
    vec2 pos = (projMatrix * vec3(position, 1)).xy;
    gl_Position = vec4(vec3(vertex * pointSize * scale + pos, 1), 1);
}
