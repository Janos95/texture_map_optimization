//not for now
//#extension GL_ARB_fragment_layer_viewport : require

in vec2 interpolatedTextureCoordinates;

in vec3 worldPosition;

uniform float fx;
uniform float fy;
uniform float cx;
uniform float cy;

layout(binding = 0)
uniform sampler2D groundTruthImage;

layout(binding = 1)
uniform sampler2D optimizationTexture;

uniform vec3 rotation;

uniform vec3 translation;

layout(location = 0)
out vec4 gradRotation;

layout(location = 1)
out vec4 gradTranslation;

vec3 rotatePoint(in vec3 v, out mat3 jacRot){
    const float x = rotation.x, y = rotation.y, z = rotation.z;
    const float x2 = x * x,  y2 = y * y, z2 = z*z;
    const float theta2 = x2 + y2 + z2;
    const float theta = sqrt(theta2);
    const vec3 k = rotation / theta;
    const float cosTheta = cos(theta);
    const float sinTheta = sin(theta);
    const float subexpr1 = (1. - cosTheta) / theta2;
    const float subexpr2 = sinTheta / theta;

    /* jac is column major */
    jacRot = mat3(
          x2 * subexpr1 + cosTheta       ,     x * y * subexpr1 + z * subexpr2,   -y * subexpr2 + x * z * subexpr1,
          x * y * subexpr1 - z * subexpr2,    y2 * subexpr1 + cosTheta        ,   x * subexpr2 + y * z * subexpr1,
          x * z * subexpr1 + y * subexpr2,    -x * subexpr2 + y * z * subexpr1,   z2 * subexpr1 + cosTheta);

    return v * cosTheta + cross(k, v) * sinTheta + k * dot(k, v) * (1. - cosTheta) + translation;
}

mat2x3 computeSobel(vec2 uv){
    vec2 texelSize = 1.0 / vec2(textureSize(groundTruthImage, 0));

    vec3 upperLeft = texture(groundTruthImage, uv + vec2(-texelSize.x, texelSize.y)).rgb;
    vec3 upperMiddle = texture(groundTruthImage, uv + vec2(0, texelSize.y)).rgb;
    vec3 upperRight = texture(groundTruthImage, uv + vec2(texelSize.x, texelSize.y)).rgb;
    vec3 left = texture(groundTruthImage, uv + vec2(-texelSize.x, 0)).rgb;
    vec3 right = texture(groundTruthImage, uv + vec2(texelSize.x, 0)).rgb;
    vec3 lowerLeft = texture(groundTruthImage, uv + vec2(-texelSize.x, -texelSize.y)).rgb;
    vec3 lowerMiddle = texture(groundTruthImage, uv + vec2(0, -texelSize.y)).rgb;
    vec3 lowerRight = texture(groundTruthImage, uv + vec2(texelSize.x, -texelSize.y)).rgb;

    vec3 derX = upperLeft + 2*left + lowerLeft - upperRight - 2*right - lowerRight;
    vec3 derY = upperLeft + 2*upperMiddle + upperRight - lowerRight - 2*lowerMiddle - lowerRight;

    return mat2x3(derX.x, derY.x, derX.y, derY.y, derX.z, derY.z);
}

void main() {
   vec3 color = texture(optimizationTexture, interpolatedTextureCoordinates).rgb;

   mat3 jacRot;
   vec3 p = rotatePoint(worldPosition.xyz, jacRot);

   mat3x2 jacProj = mat3x2(
      fx/p.z,             0,
      0,                  fy/p.z,
      -fx*p.x/(p.z*p.z), -fy*p.y/(p.z*p.z)
   );

   vec2 uv = gl_FragCoord.xy/textureSize(groundTruthImage, 0);
   mat2x3 CwrtI = computeSobel(uv);
   vec3 lossGrad = color.rgb - texture(groundTruthImage, uv).rgb; /* corresponds to a simple squared loss */

   gradRotation.xyz = lossGrad * CwrtI * jacProj * jacRot;
   gradTranslation.xyz = lossGrad * CwrtI * jacProj; /* jacobian wrt. Translation is the identity */

   gradRotation.w = 1;
   gradTranslation.w = 1;
}
