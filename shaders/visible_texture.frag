in mediump vec2 interpolatedTextureCoordinates;
layout(location = 0) out mediump ivec2 roundedTextureCoorinates;

void main() {
    roundedTextureCoorinates.x = int(interpolatedTextureCoordinates.x);
    roundedTextureCoorinates.y = int(interpolatedTextureCoordinates.y);
}