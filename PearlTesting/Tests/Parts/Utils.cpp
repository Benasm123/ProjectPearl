// ReSharper disable All
#include "pch.h"
#include "../../PearlRenderer/source/PearlCore.h"

TEST(Utils, ShaderRead)
{
	std::vector<char> code = utils::ReadShaderCode("Basic.vert.spv");
	ASSERT_FALSE(code.empty());
}