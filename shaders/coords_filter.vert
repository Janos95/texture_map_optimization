
layout(location = POSITION_ATTRIBUTE_LOCATION) in highp vec4 position;
layout(location = TEXTURECOORDINATES_ATTRIBUTE_LOCATION) in highp vec2 textureCoordinates;

uniform highp float texelWidthOffset;
uniform highp float texelHeightOffset;

out highp vec2 centerTextureCoordinate;
out highp vec2 oneStepPositiveTextureCoordinate;
out highp vec2 oneStepNegativeTextureCoordinate;
out highp vec2 twoStepsPositiveTextureCoordinate;
out highp vec2 twoStepsNegativeTextureCoordinate;
out highp vec2 threeStepsPositiveTextureCoordinate;
out highp vec2 threeStepsNegativeTextureCoordinate;
out highp vec2 fourStepsPositiveTextureCoordinate;
out highp vec2 fourStepsNegativeTextureCoordinate;

void main()
{
    gl_Position = position;

    vec2 offset = vec2(texelWidthOffset, texelHeightOffset);

    centerTextureCoordinate = textureCoordinate;
    oneStepNegativeTextureCoordinate = textureCoordinate - offset;
    oneStepPositiveTextureCoordinate = textureCoordinate + offset;
    twoStepsNegativeTextureCoordinate = textureCoordinate - (offset * 2.0);
    twoStepsPositiveTextureCoordinate = textureCoordinate + (offset * 2.0);
    threeStepsNegativeTextureCoordinate = textureCoordinate - (offset * 3.0);
    threeStepsPositiveTextureCoordinate = textureCoordinate + (offset * 3.0);
    fourStepsNegativeTextureCoordinate = textureCoordinate - (offset * 4.0);
    fourStepsPositiveTextureCoordinate = textureCoordinate + (offset * 4.0);
}