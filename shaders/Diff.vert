
uniform vec3 rod;
uniform vec3 translation;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;

uniform float fx;
uniform float fy;
uniform float cx;
uniform float cy;

layout(location = 0)
in highp vec4 position;

layout(location = 1)
in mediump vec2 textureCoordinates;

out vec2 interpolatedTextureCoordinates;

out mat3x2 IwrtRot;

out mat3x2 IwrtTrans;

void rotatePoint(in vec3 v, out vec3 result, out mat3 jacRot, out mat3 jacTrans){
    const float x = rod.x(), y = rod.y(), z = rod.z();
    const float x2 = x * x,  y2 = y * y, z2 = z*z;
    const float theta2 = x2 + y2 + z2;
    const float theta = sqrt(theta2);
    const vec3 k = rod / theta;
    const float cosTheta = cos(theta);
    const float sinTheta = sin(theta);
    const float subexpr1 = (1. - cosTheta) / theta2;
    const float subexpr2 = sinTheta / theta;

    result = v * cosTheta + cross(k, v) * sinTheta + k * dot(k, v) * (1. - cosTheta) + translation;
    /* jac is column major */
    jacRot = mat3(
        x2 * subexpr1 + cosTheta       ,     x * y * subexpr1 + z * subexpr2,   -y * subexpr2 + x * z * subexpr1,
        x * y * subexpr1 - z * subexpr2,    y2 * subexpr1 + cosTheta        ,   x * subexpr2 + y * z * subexpr1,
        x * z * subexpr1 + y * subexpr2,    -x * subexpr2 + y * z * subexpr1,   z2 * subexpr1 + cosTheta);
    jacTrans = mat3(1);
}

void main() {
    vec3 p; /* point in camera coordiante system */
    mat3 jacRot;
    mat3 jacTrans;
    rotatePoint(position.xyz, p, jacRot, jacTrans);

    mat3x2 jacProj = mat3x2(
        fx/p.z,             0,
        0,                  fy/p.z,
        -fx*p.x/(p.z*p.z), -fy*p.y/(p.z*p.z)
    );

    IwrtRot = jacProj * jacRot;
    IwrtTrans = jacProj * jacTrans;

    gl_Position = projectionMatrix * vec4(p, 1);
    interpolatedTextureCoordinates = textureCoordinates;
}