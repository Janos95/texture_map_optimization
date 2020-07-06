layout(location = 0)
in vec4 position;

layout(location = 1)
in vec2 textureCoordinates;

out vec2 interpolatedTextureCoordinates;

layout(location = 0)
uniform mat4 transformationProjectionMatrix;

void main()
{
    interpolatedTextureCoordinates = textureCoordinates;
    gl_Position =  transformationProjectionMatrix * position;
}