layout(location = 0)
in highp vec4 position;

layout(location = 1)
in mediump vec2 textureCoordinates;

out vec2 interpolatedTextureCoordinates;
out vec2 interpolatedImageCoordinates;

uniform highp mat4 projectionMatrix;
uniform highp mat4 transformationMatrix;

void main()
{
    interpolatedTextureCoordinates = textureCoordinates;
    highp vec4 imageCoords = projectionMatrix*transformationMatrix * position;
    interpolatedImageCoordinates = 0.5 * (imageCoords.xy / imageCoords.w + 1); /* clip space -> [0,1] */
    gl_Position =  imageCoords;
}