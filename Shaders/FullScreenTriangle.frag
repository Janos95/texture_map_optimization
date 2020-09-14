layout(binding = 0)
uniform highp sampler2D image;

in highp vec2 interpolatedTextureCoordinate;

layout(location = 0)
out lowp vec4 fragmentColor;

void main() {
    fragmentColor = texture(image, interpolatedTextureCoordinate);
}