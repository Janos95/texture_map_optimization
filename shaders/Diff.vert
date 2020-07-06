
uniform highp vec3 rodriguezVector;
uniform highp vec3 translation;

uniform highp mat4 projectionMatrix;

layout(location = 0)
in highp vec4 position;

layout(location = 1)
in mediump vec2 textureCoordinates;

out mediump vec2 interpolatedTextureCoordinates;

out highp mat2x3 derivative;

void rotatePoint(in vec3 rod, in vec3 pt, out vec3 result, out mat3 jac){
    const float x = rod.x(), y = rod.y(), z = rod.z();
    const float x2 = x * x,  y2 = y * y, z2 = z*z;
    const float theta2 = x2 + y2 + z2;
    const float theta = sqrt(theta2);
    const vec3 k = rod / theta;
    const float cosTheta = cos(theta);
    const float sinTheta = sin(theta);
    const float subexpr1 = (1. - cosTheta) / theta2;
    const float subexpr2 = sinTheta / theta;

    result = v * cosTheta + cross(k, v) * sinTheta + k * dot(k, v) * (1. - cosTheta);
    /* jac is column major */
    jac = mat3(
        x2 * subexpr1 + cosTheta      ,     x * y * subexpr1 + z * subexpr2 ,   -y * subexpr2 + x * z * subexpr1,
        x * y * subexpr1 - z * subexpr2,    y2 * subexpr1 + cosTheta        ,   x * subexpr2 + y * z * subexpr1,
        x * z * subexpr1 + y * subexpr2,    -x * subexpr2 + y * z * subexpr1,   z2 * subexpr1 + cosTheta);
}

void main() {
    vec3 viewPosition;
    mat3 jac;
    rotatePoint(rodriguezVector, position.xyz, viewPosition, jac);
    mat4 jacHom = mat4(jac);
    vec4 viewPositionHom = vec4(viewPosition, 1);
    gl_Position = projectionMatrix * viewPositionHom;
    interpolatedTextureCoordinates = textureCoordinates;

}