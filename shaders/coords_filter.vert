
layout(location = 0)
in highp vec4 position;

uniform highp float texelWidthOffset;
uniform highp float texelHeightOffset;

out highp vec2 centerTextureCoordinate;
out highp vec2 bottomTextureCoordinate;
out highp vec2 topTextureCoordinate;
out highp vec2 rightTextureCoordinate;
out highp vec2 leftTextureCoordinate;
out highp vec2 topLeftTextureCoordinate;
out highp vec2 topRightTextureCoordinate;
out highp vec2 bottomRightTextureCoordinate;
out highp vec2 bottomLeftTextureCoordinate;

void main()
{
    gl_Position = vec4((gl_VertexID == 2) ?  3.0 : -1.0, (gl_VertexID == 1) ? -3.0 :  1.0, 0.0, 1.0);

    vec2 textureCoordinate = gl_Position.xy*0.5 + vec2(0.5);

    centerTextureCoordinate = textureCoordinate;
    bottomTextureCoordinate = textureCoordinate + vec2(0, -texelHeightOffset);
    topTextureCoordinate = textureCoordinate + vec2(0, texelHeightOffset);
    rightTextureCoordinate = textureCoordinate + vec2(texelWidthOffset, 0);
    leftTextureCoordinate = textureCoordinate + vec2(-texelWidthOffset, 0);
    topLeftTextureCoordinate = textureCoordinate + vec2(-texelWidthOffset, texelHeightOffset);
    topRightTextureCoordinate = textureCoordinate + vec2(texelWidthOffset, texelHeightOffset);
    bottomRightTextureCoordinate = textureCoordinate + vec2(texelWidthOffset, -texelHeightOffset);
    bottomLeftTextureCoordinate = textureCoordinate + vec2(-texelWidthOffset, -texelHeightOffset);
}