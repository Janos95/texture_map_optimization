layout(location = 0) in vec4 position;
layout(location = 1) in vec3 vertex;

out vec3 interpolatedVertex;

void main() {
    interpolatedVertex = vertex;
    gl_Position = position;
}