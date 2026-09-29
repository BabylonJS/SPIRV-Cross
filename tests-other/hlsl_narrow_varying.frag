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

layout(location = 0) in float scalars[4];
layout(location = 4) in vec2 pairs[3];
layout(location = 7) in vec3 triples[2];
layout(location = 9) in vec4 wide[2];
layout(location = 11) in Varyings
{
    float scalars[4];
    vec2 pairs[3];
    vec3 triples[2];
    vec4 wide[2];
} blockData;
layout(set = 0, binding = 0) uniform Parameters
{
    int index;
};
layout(location = 0) out vec4 color;

void main()
{
    color = vec4(scalars[index & 3]) + vec4(pairs[0] + pairs[1] + pairs[2], 0.0, 0.0) +
            vec4(triples[0] + triples[1], 0.0) + wide[index & 1];
    color += vec4(blockData.scalars[index & 3]) +
             vec4(blockData.pairs[0] + blockData.pairs[1] + blockData.pairs[2], 0.0, 0.0) +
             vec4(blockData.triples[0] + blockData.triples[1], 0.0) + blockData.wide[index & 1];
}
