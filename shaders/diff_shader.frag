uniform highp ivec2 textureSize;

in mediump vec2 interpolatedTextureCoordinates;

layout(binding = 0)
uniform highp sampler2D ;

void main() {
    roundedTextureCoorinates.x = int(interpolatedTextureCoordinates.x * textureSize[0]);
    roundedTextureCoorinates.y = int(interpolatedTextureCoordinates.y * textureSize[1]);
}