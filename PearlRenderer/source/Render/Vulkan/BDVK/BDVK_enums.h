#pragma once

namespace bdvk 
{

	enum class BufferType
	{
		Vertex = (int)vk::BufferUsageFlagBits::eVertexBuffer
		, Index = (int)vk::BufferUsageFlagBits::eIndexBuffer
	};

	enum class ShaderType 
	{
		Vertex = (int)vk::ShaderStageFlagBits::eVertex
		, Fragment = (int)vk::ShaderStageFlagBits::eFragment
		, Geometry = (int)vk::ShaderStageFlagBits::eGeometry
		, Compute = (int)vk::ShaderStageFlagBits::eCompute
	};

	enum class PipelineStage
	{
		ColourOutput = (int)vk::PipelineStageFlagBits::eColorAttachmentOutput
	};

}