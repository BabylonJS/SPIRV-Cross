// Copyright 2026 Branimir Karadzic
// SPDX-License-Identifier: Apache-2.0
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#version 450

layout(location = 0) out float scalars[4];
layout(location = 4) out vec2 pairs[3];
layout(location = 7) out vec3 triples[2];
layout(location = 9) out vec4 wide[2];
layout(location = 11) out Varyings
{
    float scalars[4];
    vec2 pairs[3];
    vec3 triples[2];
    vec4 wide[2];
} blockData;

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
    blockData.scalars = scalars;
    blockData.pairs = pairs;
    blockData.triples = triples;
    blockData.wide = wide;
}
