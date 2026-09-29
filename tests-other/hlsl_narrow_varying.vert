#version 450

layout(location = 0) out float scalars[4];
layout(location = 4) out vec2 pairs[3];
layout(location = 7) out vec3 triples[2];
layout(location = 9) out vec4 wide[2];

void main()
{
    int index = gl_VertexIndex & 1;
    for (int i = 0; i < 4; i++)
        scalars[i] = float(gl_VertexIndex + i);
    for (int i = 0; i < 3; i++)
        pairs[i] = vec2(float(i), float(gl_VertexIndex));
    for (int i = 0; i < 2; i++)
    {
        triples[i] = vec3(pairs[i], scalars[i]);
        wide[i] = vec4(triples[i], 1.0);
    }
    gl_Position = wide[index];
}
