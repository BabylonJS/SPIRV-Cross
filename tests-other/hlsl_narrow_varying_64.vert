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

layout(location = 0) flat out dvec3 doubles[2];
layout(location = 4) out WideVaryings
{
    flat dvec3 doubles[2];
} doubleData;

void main()
{
    doubles[0] = dvec3(double(gl_VertexIndex));
    doubles[1] = doubles[0] + 1.0;
    doubleData.doubles = doubles;
    gl_Position = vec4(doubles[0], 1.0);
}
