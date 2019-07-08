#include "stdafx.h"
#include "Mesh.h"


Mesh::Mesh()
{
}


Mesh::~Mesh()
{
	DisposeUploaders();
}

void Mesh::LoadMeshData(char * path, ComPtr<ID3D12Device> device, ComPtr<ID3D12GraphicsCommandList> commandList)
{
	//std::ifstream in;

	//std::string ignore;
	//int size = 0;

	//in >> ignore >> size;

	//Vertex temp;
	//std::vector<Vertex> vertices;
	//for (int i = 0; i < size; i++)
	//{
	//	in >> temp.Pos.x >> temp.Pos.y >> temp.Pos.z;
	//	in >> temp.Normal.x >> temp.Normal.y >> temp.Normal.z;
	//	in >> temp.tex.x >> temp.tex.y;
	//	vertices.push_back(temp);
	//}
	//in >> ignore;
	//in >> size;


	std::array<ColorVertex, 8> vertices =
	{
		ColorVertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
		ColorVertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
		ColorVertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
		ColorVertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
		ColorVertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
		ColorVertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
		ColorVertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
		ColorVertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
	};

	std::array<std::uint16_t, 36> indices =
	{
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};


	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);


	ThrowIfFailed(D3DCreateBlob(vbByteSize, &vertexBufferCPU));
	CopyMemory(vertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

	ThrowIfFailed(D3DCreateBlob(ibByteSize,&indexBufferCPU));
	CopyMemory(indexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	vertexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		commandList.Get(), vertices.data(), vbByteSize, vertexBufferUploader);

	indexBufferGPU = d3dUtil::CreateDefaultBuffer(device.Get(),
		commandList.Get(), indices.data(), ibByteSize,indexBufferUploader);

	vertexByteStride = sizeof(Vertex);
	vertexBufferByteSize = vbByteSize;
	indexFormat = DXGI_FORMAT_R16_UINT;
	indexBufferByteSize = ibByteSize;

	SubMesh submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	subMeshArr.push_back(submesh);
}

D3D12_VERTEX_BUFFER_VIEW Mesh::VertexBufferView() const
{
	D3D12_VERTEX_BUFFER_VIEW vbv;
	vbv.BufferLocation = vertexBufferGPU->GetGPUVirtualAddress();
	vbv.StrideInBytes = vertexByteStride;
	vbv.SizeInBytes = vertexBufferByteSize;

	return vbv;
}

D3D12_INDEX_BUFFER_VIEW Mesh::IndexBufferView() const
{
	D3D12_INDEX_BUFFER_VIEW ibv;
	ibv.BufferLocation = indexBufferGPU->GetGPUVirtualAddress();
	ibv.SizeInBytes = indexBufferByteSize;
	ibv.Format = indexFormat;

	return ibv;
}

void Mesh::DisposeUploaders()
{
	vertexBufferUploader = nullptr;
	indexBufferUploader = nullptr;
}
