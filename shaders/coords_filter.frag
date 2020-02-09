uniform highp sampler2D depthTexture;
uniform highp sampler2D coordsTexture;
uniform highp float threshold;

out highp vec2 centerTextureCoordinate;
out highp vec2 bottomTextureCoordinate;
out highp vec2 topCoordinate;
out highp vec2 rightTextureCoordinate;
out highp vec2 leftTextureCoordinate;
out highp vec2 topLeftTextureCoordinate;
out highp vec2 topRightTextureCoordinate;
out highp vec2 bottomRightTextureCoordinate;
out highp vec2 bottomLeftTextureCoordinate;

layout(location = 0) out highp ivec2 coord;

void main() {

  float centerDepth = texture2D(depthTexture, centerTextureCoordinate).r;
  float bottomDepth = texture2D(depthTexture, bottomTextureCoordinate).r;
  float topDepth = texture2D(depthTexture, topTextureCoordinate).r;
  float rightDepth = texture2D(depthTexture, rightTextureCoordinate).r;
  float leftDepth = texture2D(depthTexture, leftTextureCoordinate).r;
  float topLeftDepth = texture2D(depthTexture, topLeftTextureCoordinate).r;
  float topRightDepth = texture2D(depthTexture, topRightTextureCoordinate).r;
  float bottomLeftDepth = texture2D(depthTexture, bottomLeftTextureCoordinate).r;
  float bottomRightDepth = texture2D(depthTexture, bottomRightTextureCoordinate).r;

  highp float maxValue = max(centerDepth, bottomDepth);
  maxValue = max(maxValue, topDepth);
  maxValue = max(maxValue, rightDepth);
  maxValue = max(maxValue, leftDepth);
  maxValue = max(maxValue, topLeftDepth);
  maxValue = max(maxValue, topRightDepth);
  maxValue = max(maxValue, bottomLeftDepth);
  maxValue = max(maxValue, bottomRightDepth);


  highp float minValue = min(centerDepth, bottomDepth);
  maxValue = min(maxValue, topDepth);
  maxValue = min(maxValue, rightDepth);
  maxValue = min(maxValue, leftDepth);
  maxValue = min(maxValue, topLeftDepth);
  maxValue = min(maxValue, topRightDepth);
  maxValue = min(maxValue, bottomRightDepth);
  maxValue = min(maxValue, bottomLeftDepth);

  if(maxValue - minValue> threshold)
    coord = ivec2(-1,-1);
  else
    coord = texture(coordsTexture, textureCoordinates);
}