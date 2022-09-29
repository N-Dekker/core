#version 330 core

// Scalar effects
#define EFFECT_NONE    0
#define EFFECT_COLOR   1
#define EFFECT_SIZE    2
#define EFFECT_OUTLINE 3

// Point properties
uniform float pointSize;        /** Point size */
uniform float pointSizeScale;   /** Scale factor in absolute point size mode */
uniform int   scalarEffect;
uniform float pointOpacity;     /** Point opacity */

uniform mat3 orthoM;            /** Projection matrix from bounds space to clip space */
uniform bool hasHighlights;     /** Whether a highlight buffer is used */
uniform bool hasScalars;        /** Whether a scalar buffer is used */
uniform vec3 colorMapRange;     /** Color map scalar range */
uniform bool hasColors;         /** Whether a color buffer is used */
uniform bool hasSizes;          /** Whether a point size buffer is used */
uniform bool hasOpacities;      /** Whether an opacity buffer is used */

layout(location = 0) in vec2    vertex;         /** Vertex input, always a [-1, 1] quad */
layout(location = 1) in vec2    position;       /** 2-Dimensional positions of points */
layout(location = 2) in int     highlight;      /** Mask of highlights over the points */
layout(location = 3) in float   scalar;         /** Point scalar */
layout(location = 4) in vec3    color;          /** Point color */
layout(location = 5) in float   size;           /** Point size */
layout(location = 6) in float   opacity;        /** Point opacity */

// Output variables
smooth out vec2  vTexCoord;
flat   out int   vHighlight;
smooth out float vScalar;
smooth out vec3  vColor;
smooth out float vOpacity;

void main()
{
    // The texture coordinates match vertex coordinates
    vTexCoord = vertex;
    // Pass input attributes to fragment shader if they are defined
    vHighlight = hasHighlights ? highlight : 0;
    
    vScalar = hasScalars ? (scalar - colorMapRange.x) / colorMapRange.z : 0;
    
    vColor = hasColors ? color : vec3(0.5);

    vOpacity = pointOpacity;
    if (hasOpacities)
        vOpacity = opacity;

    // Transform position to clip space
    vec2 pos = (orthoM * vec3(position, 1)).xy;
    
    // Resize point quad according to properties
    vec2 scaledVertex = vertex * pointSize * pointSizeScale;
    if (hasSizes)
        scaledVertex = vertex * size * pointSizeScale;
    
    // Move quad by position and output
    gl_Position = vec4(scaledVertex + pos, 0, 1);
}
