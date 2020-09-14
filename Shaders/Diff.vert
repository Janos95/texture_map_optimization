layout(location = POSITION_ATTRIBUTE_LOCATION)
in highp vec4 position;

layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION)
in mediump vec2 textureCoordinates;

layout(location = TRANSFORMATION_MATRIX_ATTRIBUTE_LOCATION)
in highp mat4 instancedTransformationMatrix;

uniform mat4 projectionMatrix;

out vec2 interpolatedTextureCoordinates;

void main() {
    gl_Position = projectionMatrix*instancedTransformationMatrix*position;
    interpolatedTextureCoordinates = textureCoordinates;
}