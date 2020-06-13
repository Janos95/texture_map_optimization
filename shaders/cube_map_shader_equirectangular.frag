in vec3 interpolatedPositions;

out vec4 fragmentColor;

layout(binding = 0)
uniform sampler2D equirectangularMap;

const highp vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(highp vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(interpolatedPositions)); // make sure to normalize positions
    vec3 color = texture(equirectangularMap, uv).rgb;

    fragmentColor = vec4(color, 1.0);
}