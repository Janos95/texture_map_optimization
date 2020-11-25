layout(location = POSITION_ATTRIBUTE_LOCATION)
in highp vec4 position;

layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION)
in mediump vec2 textureCoordinates;

uniform mat4 projectionTransformationMatrix;

out vec2 interpolatedTextureCoordinates;
out vec3 worldPosition;
void main() {
    gl_Position = projectionTransformationMatrix*position;
    interpolatedTextureCoordinates = textureCoordinates;
    worldPosition = position.xyz;
}