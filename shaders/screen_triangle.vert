layout(location = 0)
in highp vec4 position;

out highp vec2 interpolatedTextureCoordinate;

void main()
{
    gl_Position = vec4((gl_VertexID == 2) ?  3.0 : -1.0, (gl_VertexID == 1) ? -3.0 :  1.0, 0.0, 1.0);
    interpolatedTextureCoordinate = gl_Position.xy*0.5 + vec2(0.5);
}