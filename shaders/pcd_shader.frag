in lowp vec4 interpolatedColor;

layout(location = COLOR_OUTPUT_ATTRIBUTE_LOCATION)
out lowp vec4 fragmentColor;
void main() {

    vec2 circCoord = 2.0 * gl_PointCoord - 1.0;
    if (dot(circCoord, circCoord) > 1.0) {
        discard;
    }

    fragmentColor.rgb = color;
    fragmentColor.a = 1.0;
}