#version 450

layout(location = 0) in float scalars[4];
layout(location = 4) in vec2 pairs[3];
layout(location = 7) in vec3 triples[2];
layout(location = 9) in vec4 wide[2];
layout(set = 0, binding = 0) uniform Parameters
{
    int index;
};
layout(location = 0) out vec4 color;

void main()
{
    color = vec4(scalars[index & 3]) + vec4(pairs[0] + pairs[1] + pairs[2], 0.0, 0.0) +
            vec4(triples[0] + triples[1], 0.0) + wide[index & 1];
}
