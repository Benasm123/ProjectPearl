#include "StaticMesh.h"

StaticMesh::StaticMesh(VulkanRenderer& renderer, pearl::typesRender::MeshData data)
	: data_(data)
	, renderer_(renderer)
{
	// TODO ->	Create resources
	//			Renderer needs to be made aware of this object.
	renderer_.DrawMesh(*this);
}

StaticMesh::~StaticMesh()
{
	// TODO ->	Destroy Resources
	//			Make sure renderer is aware this object no longer exists
	renderer_.DestroyMesh(*this);
}
