out vec4 fragmentColor;

in vec3 interpolatedPositions;

uniform samplerCube environmentMap;

void main()
{
    vec3 envColor = texture(environmentMap, interpolatedPositions).rgb;

    envColor = envColor / (envColor + vec3(1.0));
    envColor = pow(envColor, vec3(1.0/2.2));

    fragmentColor = vec4(envColor, 1.0);
}