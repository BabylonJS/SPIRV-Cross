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
		require_text(source, std::string("static ") + types[group] + " " + name + "[" +
		                     std::to_string(counts[group]) + "]");
		for (unsigned i = 0; i < counts[group]; i++, location++)
		{
			const std::string member = name + "_" + std::to_string(i);
			const std::string element = name + "[" + std::to_string(i) + "]";
			require_text(source, std::string(types[group]) + " " + member + " : TEXCOORD" +
			                     std::to_string(location) + ";");
			require_text(source, vertex ? "stage_output." + member + " = " + element + ";" :
			                              element + " = stage_input." + member + ";");
		}
	}
	require_text(source, "float4 wide[2] : TEXCOORD9;");
}

int main(int argc, char **argv)
{
	try
	{
		if (argc != 4)
			throw std::runtime_error("Expected vertex SPIR-V, fragment SPIR-V, and an output directory.");
		check_shader(argv[1], true, std::string(argv[3]) + "/hlsl_narrow_varying.vert.hlsl");
		check_shader(argv[2], false, std::string(argv[3]) + "/hlsl_narrow_varying.frag.hlsl");
	}
	catch (const std::exception &error)
	{
		std::cerr << error.what() << std::endl;
		return 1;
	}
	return 0;
}
