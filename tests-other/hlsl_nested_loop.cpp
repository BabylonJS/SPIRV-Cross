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
#include <sstream>
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

static size_t count_text(const std::string &source, const std::string &text)
{
	size_t count = 0;
	for (size_t pos = source.find(text); pos != std::string::npos; pos = source.find(text, pos + text.size()))
		count++;
	return count;
}

int main(int argc, char **argv)
{
	try
	{
		if (argc != 3)
			throw std::runtime_error("Expected SPIR-V and an output directory.");
		const auto words = read_spirv(argv[1]);
		std::string sources[2];
		for (unsigned enabled = 0; enabled < 2; enabled++)
		{
			CompilerHLSL compiler(words);
			auto options = compiler.get_hlsl_options();
			if (options.enable_fxc_nested_loop_workaround)
				throw std::runtime_error("The workaround must default to disabled.");
			options.shader_model = 50;
			options.enable_fxc_nested_loop_workaround = enabled != 0;
			compiler.set_hlsl_options(options);
			sources[enabled] = compiler.compile();
			std::ofstream output(std::string(argv[2]) +
			                     (enabled ? "/hlsl_nested_loop.enabled.hlsl" : "/hlsl_nested_loop.disabled.hlsl"));
			if (!(output << sources[enabled]))
				throw std::runtime_error("Cannot write generated HLSL.");
		}
		if (count_text(sources[0], "[fastopt]") != 0 || count_text(sources[1], "[fastopt]") != 2)
			throw std::runtime_error("Only the two rolled outer loops should receive [fastopt].");
		if (count_text(sources[0], "[unroll]") != 1 || count_text(sources[1], "[unroll]") != 1)
			throw std::runtime_error("Enabling the workaround must not add an unroll hint to do-while loops.");

		std::istringstream lines(sources[1]);
		std::string stripped, line;
		while (std::getline(lines, line))
		{
			if (line.find("[fastopt]") == std::string::npos)
				stripped += line + "\n";
		}
		if (stripped != sources[0])
			throw std::runtime_error("Enabling the workaround must only add the loop hints.");
	}
	catch (const std::exception &error)
	{
		std::cerr << error.what() << std::endl;
		return 1;
	}
	return 0;
}
