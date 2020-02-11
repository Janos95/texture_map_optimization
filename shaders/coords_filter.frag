uniform highp sampler2D depthTexture;
uniform highp isampler2D coordsTexture;
uniform highp float threshold;


in highp vec2 centerTextureCoordinate;
in highp vec2 bottomTextureCoordinate;
in highp vec2 topTextureCoordinate;
in highp vec2 rightTextureCoordinate;
in highp vec2 leftTextureCoordinate;
in highp vec2 topLeftTextureCoordinate;
in highp vec2 topRightTextureCoordinate;
in highp vec2 bottomRightTextureCoordinate;
in highp vec2 bottomLeftTextureCoordinate;

layout(location = 0) out highp ivec2 coord;

void main() {

  float centerDepth = texture(depthTexture, centerTextureCoordinate).r;
  float bottomDepth = texture(depthTexture, bottomTextureCoordinate).r;
  float topDepth = texture(depthTexture, topTextureCoordinate).r;
  float rightDepth = texture(depthTexture, rightTextureCoordinate).r;
  float leftDepth = texture(depthTexture, leftTextureCoordinate).r;
  float topLeftDepth = texture(depthTexture, topLeftTextureCoordinate).r;
  float topRightDepth = texture(depthTexture, topRightTextureCoordinate).r;
  float bottomLeftDepth = texture(depthTexture, bottomLeftTextureCoordinate).r;
  float bottomRightDepth = texture(depthTexture, bottomRightTextureCoordinate).r;

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
  coord = texture(coordsTexture, centerTextureCoordinate).xy;
}