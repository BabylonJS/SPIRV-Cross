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

#include "spirv_hlsl.hpp"
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace SPIRV_CROSS_NAMESPACE;

static std::vector<uint32_t> read_spirv(const char *path)
{
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	const auto size = file.tellg();
	if (!file || size <= 0 || size % sizeof(uint32_t) != 0)
		throw std::runtime_error("Cannot read SPIR-V fixture.");
	std::vector<uint32_t> words(static_cast<size_t>(size) / sizeof(uint32_t));
	file.seekg(0);
	if (!file.read(reinterpret_cast<char *>(words.data()), size))
		throw std::runtime_error("Incomplete SPIR-V fixture.");
	return words;
}

static void require_text(const std::string &source, const std::string &expected)
{
	if (source.find(expected) == std::string::npos)
		throw std::runtime_error("Missing HLSL: " + expected);
}

static void check_shader(const char *path, bool vertex, const std::string &output)
{
	CompilerHLSL compiler(read_spirv(path));
	auto options = compiler.get_hlsl_options();
	options.shader_model = 50;
	compiler.set_hlsl_options(options);
	const std::string source = compiler.compile();
	std::ofstream file(output);
	if (!(file << source))
		throw std::runtime_error("Cannot write generated HLSL.");

	const char *names[] = { "scalars", "pairs", "triples" };
	const char *types[] = { "float", "float2", "float3" };
	const unsigned counts[] = { 4, 3, 2 };
	unsigned location = 0;
	for (unsigned group = 0; group < 3; group++)
	{
		const std::string name = names[group];
		require_text(source,
		             std::string("static ") + types[group] + " " + name + "[" + std::to_string(counts[group]) + "]");
		for (unsigned i = 0; i < counts[group]; i++, location++)
		{
			const std::string member = name + "_" + std::to_string(i);
			const std::string element = name + "[" + std::to_string(i) + "]";
			require_text(source,
			             std::string(types[group]) + " " + member + " : TEXCOORD" + std::to_string(location) + ";");
			require_text(source, vertex ? "stage_output." + member + " = " + element + ";" :
			                              element + " = stage_input." + member + ";");
		}
	}
	require_text(source, "float4 wide[2] : TEXCOORD9;");
	location = 11;
	for (unsigned group = 0; group < 3; group++)
	{
		const std::string name = names[group];
		for (unsigned i = 0; i < counts[group]; i++, location++)
		{
			const std::string member = "Varyings_" + name + "_" + std::to_string(i);
			const std::string element = "blockData." + name + "[" + std::to_string(i) + "]";
			require_text(source,
			             std::string(types[group]) + " " + member + " : TEXCOORD" + std::to_string(location) + ";");
			require_text(source, vertex ? "stage_output." + member + " = " + element + ";" :
			                              element + " = stage_input." + member + ";");
		}
	}
	require_text(source, "float4 Varyings_wide[2] : TEXCOORD20;");
	require_text(source, vertex ? "stage_output.wide = wide;" : "wide = stage_input.wide;");
	require_text(source, vertex ? "stage_output.Varyings_wide = blockData.wide;" :
	                              "blockData.wide = stage_input.Varyings_wide;");
}

static void check_64_bit_shader(const char *path, bool vertex)
{
	CompilerHLSL compiler(read_spirv(path));
	auto options = compiler.get_hlsl_options();
	options.shader_model = 50;
	compiler.set_hlsl_options(options);
	const std::string source = compiler.compile();
	require_text(source, "nointerpolation double3 doubles[2] : TEXCOORD0;");
	require_text(source, "nointerpolation double3 WideVaryings_doubles[2] : TEXCOORD4;");
	require_text(source, vertex ? "stage_output.doubles = doubles;" : "doubles = stage_input.doubles;");
	require_text(source, vertex ? "stage_output.WideVaryings_doubles = doubleData.doubles;" :
	                              "doubleData.doubles = stage_input.WideVaryings_doubles;");
	if (source.find("double3 doubles_0 :") != std::string::npos ||
	    source.find("double3 WideVaryings_doubles_0 :") != std::string::npos)
		throw std::runtime_error("64-bit varying arrays must not be flattened.");
}

int main(int argc, char **argv)
{
	try
	{
		if (argc != 6)
			throw std::runtime_error("Expected vertex/fragment SPIR-V, an output directory, and 64-bit fixtures.");
		check_shader(argv[1], true, std::string(argv[3]) + "/hlsl_narrow_varying.vert.hlsl");
		check_shader(argv[2], false, std::string(argv[3]) + "/hlsl_narrow_varying.frag.hlsl");
		check_64_bit_shader(argv[4], true);
		check_64_bit_shader(argv[5], false);
	}
	catch (const std::exception &error)
	{
		std::cerr << error.what() << std::endl;
		return 1;
	}
	return 0;
}
