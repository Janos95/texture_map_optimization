uniform highp sampler2D depthTexture;
uniform highp sampler2D coordsTexture;
uniform highp float threshold;

in highp vec2 centerTextureCoordinate;
in highp vec2 oneStepPositiveTextureCoordinate;
in highp vec2 oneStepNegativeTextureCoordinate;
in highp vec2 twoStepsPositiveTextureCoordinate;
in highp vec2 twoStepsNegativeTextureCoordinate;
in highp vec2 threeStepsPositiveTextureCoordinate;
in highp vec2 threeStepsNegativeTextureCoordinate;
in highp vec2 fourStepsPositiveTextureCoordinate;
in highp vec2 fourStepsNegativeTextureCoordinate;

layout(location = 0) out highp ivec2 coord;

void main() {

  float centerDepth = texture2D(depthTexture, centerTextureCoordinate).r;
  float oneStepPositiveDepth = texture2D(depthTexture, oneStepPositiveTextureCoordinate).r;
  float oneStepNegativeDepth = texture2D(depthTexture, oneStepNegativeTextureCoordinate).r;
  float twoStepsPositiveDepth = texture2D(depthTexture, twoStepsPositiveTextureCoordinate).r;
  float twoStepsNegativeDepth = texture2D(depthTexture, twoStepsNegativeTextureCoordinate).r;
  float threeStepsPositiveDepth = texture2D(depthTexture, threeStepsPositiveTextureCoordinate).r;
  float threeStepsNegativeDepth = texture2D(depthTexture, threeStepsNegativeTextureCoordinate).r;
  float fourStepsPositiveDepth = texture2D(depthTexture, fourStepsPositiveTextureCoordinate).r;
  float fourStepsNegativeDepth = texture2D(depthTexture, fourStepsNegativeTextureCoordinate).r;

  highp float maxValue = max(centerDepth, oneStepPositiveDepth);
  maxValue = max(maxValue, oneStepNegativeDepth);
  maxValue = max(maxValue, twoStepsPositiveDepth);
  maxValue = max(maxValue, twoStepsNegativeDepth);
  maxValue = max(maxValue, threeStepsPositiveDepth);
  maxValue = max(maxValue, threeStepsNegativeDepth);
  maxValue = max(maxValue, fourStepsPositiveDepth);
  maxValue = max(maxValue, fourStepsNegativeDepth);


  highp float minValue = min(centerDepth, oneStepPositiveDepth);
  maxValue = min(maxValue, oneStepNegativeDepth);
  maxValue = min(maxValue, twoStepsPositiveDepth);
  maxValue = min(maxValue, twoStepsNegativeDepth);
  maxValue = min(maxValue, threeStepsPositiveDepth);
  maxValue = min(maxValue, threeStepsNegativeDepth);
  maxValue = min(maxValue, fourStepsPositiveDepth);
  maxValue = min(maxValue, fourStepsNegativeDepth);
  if(maxValue - minValue> threshold)
    coord = ivec2(-1,-1);
  else
    coord = texture(coordsTexture, textureCoordinates);
}