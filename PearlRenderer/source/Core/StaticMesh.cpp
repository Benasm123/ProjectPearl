#include "StaticMesh.h"

StaticMesh::StaticMesh(const VulkanRenderer& renderer, pearl::typesRender::MeshData data)
	: data_(data)
	, renderer_(renderer)
{
	// TODO ->	Create resources
	//			Renderer needs to be made aware of this object.
}

StaticMesh::~StaticMesh()
{
	// TODO ->	Destroy Resources
	//			Make sure renderer is aware this object no longer exists
}
