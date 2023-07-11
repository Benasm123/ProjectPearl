#include "pch.h"
#include "..\..\PearlRenderer\source\Render\Vulkan\RendererInstance.h"
#include "../../PearlRenderer/source/Render/Vulkan/GraphicsUnit.h"

using namespace pearl;

TEST(Windows, Creation)
{
	const Window window1("Test", 1000, 1000);
	ASSERT_TRUE(window1.Get());

	const Window window2("", 0, 0);
	ASSERT_TRUE(window2.Get());

	const Window window3("Test1", -5, -5);
	ASSERT_TRUE(window3.Get());

	// ReSharper disable once StringLiteralTypo
	const Window window4("LOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOONG TITLE", 0, 1000);
	ASSERT_TRUE(window4.Get());
}