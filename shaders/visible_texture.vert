
uniform highp mat4 transformationProjectionMatrix;

layout(location = 0)
in highp vec4 position;

layout(location = 1)
in mediump vec2 textureCoordinates;

out mediump vec2 interpolatedTextureCoordinates;

void main() {
    gl_Position = transformationProjectionMatrix*position;
    interpolatedTextureCoordinates = textureCoordinates;
}