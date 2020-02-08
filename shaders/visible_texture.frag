uniform highp ivec2 textureSize;

in mediump vec2 interpolatedTextureCoordinates;

layout(location = 0) out mediump ivec2 roundedTextureCoorinates;

void main() {
    roundedTextureCoorinates.x = int(interpolatedTextureCoordinates.x * textureSize[0]);
    roundedTextureCoorinates.y = int(interpolatedTextureCoordinates.y * textureSize[1]);
}