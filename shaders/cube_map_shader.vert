
layout (location = 0)
in vec3 position;

out vec3 interpolatedPositions;

uniform mat4 projectionMatrix;
uniform mat4 transformationMatrix;

void main()
{
    interpolatedPositions = position;
    gl_Position =  projectionMatrix * transformationMatrix * vec4(position, 1.0);
}