layout(location = POSITION_ATTRIBUTE_LOCATION)
in highp vec4 position;

layout(location = COLOR_ATTRIBUTE_LOCATION)
in lowp vec4 color;

layout(location = 0) uniform mat4 transformationProjectionMatrix = mat4(1.0);
layout(location = 1) uniform float pointSize = 1.0;

out lowp vec4 interpolatedColor;

void main() {
    interpolatedColor = color;
    gl_Position = transformationProjectionMatrix * position;
    gl_PointSize = pointSize;
}