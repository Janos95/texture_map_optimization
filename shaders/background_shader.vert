layout (location = 0)
in vec3 position;

uniform mat4 projectionMatrix;
uniform mat4 transformationMatrix;

out vec3 interpolatedPositions;

void main()
{
    interpolatedPositions = position;

    mat4 rotation = mat4(mat3(transformationMatrix)); // remove translation from the view matrix
    vec4 clipPos = projectionMatrix * rotation * vec4(position, 1.0);

    gl_Position = clipPos.xyww;
}
