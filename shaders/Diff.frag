in vec2 interpolatedTextureCoordinates;

in mat3x2 IwrtRot;

in mat3x2 IwrtTrans;

layout(binding = 0)
uniform sampler2D tex;

layout(location = 0)
out vec3 color;

layout(location = 1)
out vec2 IwrtRot1;

layout(location = 2)
out vec2 IwrtRot2;

layout(location = 3)
out vec2 IwrtRot3;

layout(location = 4)
out vec2 IwrtTrans1;

layout(location = 5)
out vec2 IwrtTrans2;

layout(location = 6)
out vec2 IwrtTrans3;

void main() {
   color = texture(tex, interpolatedTextureCoordinates).rgb;

   IwrtRot1 = IwrtRot[0];
   IwrtRot2 = IwrtRot[1];
   IwrtRot3 = IwrtRot[2];

   IwrtTrans1 = IwrtTrans[0];
   IwrtTrans2 = IwrtTrans[1];
   IwrtTrans3 = IwrtTrans[2];
}
