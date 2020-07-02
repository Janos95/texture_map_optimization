
in vec2 interpolatedTextureCoordinates;
in vec2 interpolatedImageCoordinates;

out vec4 fragmentColor;

layout(binding = 0)
uniform sampler2D colors;

layout(binding = 1)
uniform sampler2D image;

void main()
{
    vec4 colorOld = texture(colors, interpolatedTextureCoordinates);
    vec3 color = texture(image, interpolatedImageCoordinates).rgb;
    fragmentColor = vec4(color + colorOld.xyz, colorOld.w + 1.0);
}