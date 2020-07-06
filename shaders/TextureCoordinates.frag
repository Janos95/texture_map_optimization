
in vec2 interpolatedTextureCoordinates;

layout(binding = 0)
uniform sampler2D colorMap;

layout(location = 1)
uniform float numPrimitives;

layout(location = 0)
out vec4 textureCoordinate;

void main()
{
    textureCoordinate = vec4(interpolatedTextureCoordinates,0,1);
}