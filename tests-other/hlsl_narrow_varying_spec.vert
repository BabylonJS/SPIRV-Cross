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

layout(constant_id = 0) const int arraySize = 2;
layout(location = 0) out float specialized[arraySize];
layout(location = 8) out SpecializedVaryings
{
    vec2 pairs[arraySize];
} specData;

void main()
{
    for (int i = 0; i < arraySize; i++)
    {
        specialized[i] = float(gl_VertexIndex + i);
        specData.pairs[i] = vec2(specialized[i], float(i));
    }
    gl_Position = vec4(float(gl_VertexIndex), 0.0, 0.0, 1.0);
}
