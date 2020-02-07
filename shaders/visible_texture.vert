layout(location = 0) uniform highp mat4 transformationProjectionMatrix = mat4(1.0);

layout(location = POSITION_ATTRIBUTE_LOCATION) in highp vec4 position;
layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION) in mediump vec2 textureCoordinates;

out mediump vec2 interpolatedTextureCoordinates;


void main() {
    gl_Position = transformationProjectionMatrix*position;
    interpolatedTextureCoordinates = textureCoordinates;
}