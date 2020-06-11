layout(location = 0)
in highp vec4 position;

layout(location = 1)
in mediump vec2 textureCoordinates;

layout(location = 5)
in mediump vec3 normal;

out highp vec2 interpolatedTextureCoordinates;
out highp vec3 cameraDirection;
out mediump vec3 transformedNormal;
out highp vec3 lightDirections[LIGHT_COUNT];

uniform highp mat4 projectionMatrix;
uniform highp mat4 transformationMatrix;
uniform mediump mat3 normalMatrix;
uniform highp vec3 lightPositions[LIGHT_COUNT];

void main()
{
    interpolatedTextureCoordinates = textureCoordinates;
    transformedNormal = normalMatrix * normal;

    highp vec4 transformedPosition4 = transformationMatrix * position;
    highp vec3 transformedPosition = transformedPosition4.xyz/transformedPosition4.w;

    cameraDirection = -normalize(transformedPosition);

    for(int i = 0; i < LIGHT_COUNT; ++i)
        lightDirections[i] = lightPositions[i] - transformedPosition;

    gl_Position =  projectionMatrix * transformedPosition4;
}