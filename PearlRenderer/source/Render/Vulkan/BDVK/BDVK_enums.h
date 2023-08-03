#pragma once

// This include shouldnt be here, need to find a way to geet the vulkan enum balues without making it public.
// I dont think i can set them to vulkan values. I will need to set to correct value in the functions.

namespace bdvk 
{
	enum class BufferType
	{
		Index = 64u,
		Vertex = 128u,
	};

	enum class ShaderType 
	{
		Vertex = 1u,
		Geometry = 8u,
		Fragment = 16u,
		Compute = 32u
	};

	enum class PipelineStage
	{
		ColourOutput = 1024u,
	};

	enum class PipelineBindPoint
	{
		Graphics = 0u,
		Compute = 1u
	};

}