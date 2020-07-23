
in vec2 interpolatedTextureCoordinates;

layout(location = 0)
out vec4 textureCoordinate;

void main()
{
    textureCoordinate = vec4(interpolatedTextureCoordinates,0,1);
}