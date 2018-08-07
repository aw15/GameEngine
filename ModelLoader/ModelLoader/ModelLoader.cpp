// ModelLoader.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"
#include <map>
#include<algorithm>
#include<iostream>
string FILENAME = "walking";
#define NUM_BONES_PER_VEREX 96
typedef unsigned int uint;

struct VERTEX {
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
	vector<float> weights;
	vector<unsigned int> boneIndices;
};



struct VertexBoneData
{
	unsigned int IDs[NUM_BONES_PER_VEREX];
	float Weights[NUM_BONES_PER_VEREX];
};

struct BoneInfo
{
	string boneName;
	UINT index;
	UINT parentIndex = -1;
	XMFLOAT4X4 BoneOffset = MathHelper::Identity4x4();

};



XMFLOAT4X4 convertXM(const aiMatrix4x4& data)
{
	XMFLOAT4X4 ret;
	ret._11 = data.a1;	ret._12 = data.a2;	ret._13 = data.a3;	ret._14 = data.a4;
	ret._21 = data.b1;	ret._22 = data.b2;	ret._23 = data.b3;	ret._24 = data.b4;
	ret._31 = data.c1;	ret._32 = data.c2;	ret._33 = data.c3;	ret._34 = data.c4;
	ret._41 = data.d1;	ret._42 = data.d2;	ret._43 = data.d3;	ret._44 = data.d4;

	return ret;
}

struct AnimationClip
{

};




struct Texture {
	string type;
	string path;
	// *texture;
};



#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

//vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName, const aiScene* scene);
//string determineTextureType(const aiScene* scene, aiMaterial* mat);
//int getTextureIndex(aiString* str);
void LoadBones(const aiMesh* pMesh);
void processNode(aiNode * node, const aiScene * scene);
void processMesh(aiMesh* mesh, const aiScene* scene);
void ReadNodeHeirarchy(const aiNode* pNode,  const aiScene* scene, UINT);
//string determineTextureType(const aiScene * scene, aiMaterial * mat);
string textype;

unordered_map<string, int> boneMap;
vector<BoneInfo> boneInfo;
// Data to fill
vector<VERTEX> vertices;
vector<UINT> indices;


XMMATRIX globalInv;

int main()
{
	Assimp::Importer importer;
	importer.GetErrorString();


	const aiScene* pScene = importer.ReadFile(FILENAME+".fbx", ASSIMP_LOAD_FLAGS);


	if (pScene==NULL) {
		printf("FAIL");
		return 0;
	}
	processNode(pScene->mRootNode, pScene);
	
	ofstream out("Model/"+FILENAME+".model");
	out<< vertices.size()<<" " << "VERTEXDATA" << endl;
	for (auto& data : vertices)
	{
		out << data.position.x <<" "<< data.position.y <<" "<< data.position.z<<std::endl;
		out << data.texcoord.x << " " << data.texcoord.y << std::endl;
		out << data.normal.x << " " << data.normal.y << " " << data.normal.z << std::endl;
		out << data.weights[0] << " " << data.weights[1] << " " << data.weights[2] << std::endl;
		out << data.boneIndices[0] << " " << data.boneIndices[1] << " " << data.boneIndices[2] << " " << data.boneIndices[3] << std::endl;
		out << "==========================================================================" << std::endl;

	}
	out << "INDEX" << " " << indices.size() << endl;
	for (auto& data : indices)
	{
		out << data << " ";
	}
	out << "BONETREE" << endl;
	for (auto& data : boneInfo)
	{
		out << data.boneName.c_str() << " " << data.index << " " << data.parentIndex << endl;
		auto boneOffset = data.BoneOffset.m;
		out << boneOffset[0][0] << " " << boneOffset[0][1] << " " << boneOffset[0][2] << " " << boneOffset[0][3] << endl;
		out << boneOffset[1][0] << " " << boneOffset[1][1] << " " << boneOffset[1][2] << " " << boneOffset[1][3] << endl;
		out << boneOffset[2][0] << " " << boneOffset[2][1] << " " << boneOffset[2][2] << " " << boneOffset[2][3] << endl;
		out << boneOffset[3][0] << " " << boneOffset[3][1] << " " << boneOffset[3][2] << " " << boneOffset[3][3] << endl;
		out << "====================================================================" << endl;
	}

	//for (auto& data : boneInfo)
	//{
	//	printf("%s\n", data.boneName.c_str());
	//	auto trans = data.FinalTransformation;
	//	printf("%f %f %f %f\n", trans._11, trans._12, trans._13, trans._14);
	//	printf("%f %f %f %f\n", trans._21, trans._22, trans._23, trans._24);
	//	printf("%f %f %f %f\n", trans._31, trans._32, trans._33, trans._34);
	//	printf("%f %f %f %f\n", trans._41, trans._42, trans._43, trans._44);
	//	printf("\n==========================================================\n");
	//}



	//ifstream in("Model/" + FILENAME + ".model");

	//string text; 
	//int vertexSize = 0;
	//in >> vertexSize;

	//while (!in.eof())
	//{
	//	in >> text;
	//	if (text == "POSITION")
	//	{
	//		for (int i = 0; i < vertexSize; ++i)
	//		{
	//			float x, y, z;
	//			in >> x >> y >> z;
	//			printf("%f %f %f\n", x, y, z);
	//		}
	//	}
	//	if (text == "UV")
	//	{
	//		for (int i = 0; i < vertexSize; ++i)
	//		{
	//			float u, v;
	//			in >> u >> v;
	//			printf("%f %f\n", u, v);
	//		}
	//	}
	//	if (text == "NORMAL")
	//	{
	//		for (int i = 0; i < vertexSize; ++i)
	//		{
	//			float x, y, z;
	//			in >> x >> y >> z;
	//			printf("%f %f %f\n", x, y, z);
	//		}
	//	}
	//	if (text == "INDEX")
	//	{
	//		int indexSize;
	//		in >> indexSize;
	//		for (int i = 0; i < indexSize; ++i)
	//		{
	//			int index;
	//			in >> index;
	//			printf("%d ", index);
	//		}
	//	}
	//}

	//in.close();
	out.close();

    return 0;
}



void processNode(aiNode * node, const aiScene * scene)
{
	
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		processMesh(mesh, scene);
	}

	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

void processMesh(aiMesh * mesh, const aiScene * scene)
{


	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		VERTEX vertex;

		vertex.position.x = (float)mesh->mVertices[i].x;
		vertex.position.y = (float)mesh->mVertices[i].y;
		vertex.position.z = (float)mesh->mVertices[i].z;
		//printf("%F %F %f\n", vertex.X, vertex.Y, vertex.Z);
		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex.texcoord.y = (float)mesh->mTextureCoords[0][i].y;
		}
		if (mesh->HasNormals())
		{
			vertex.normal.x = mesh->mNormals[i].x;
			vertex.normal.y = mesh->mNormals[i].y;
			vertex.normal.z = mesh->mNormals[i].z;
		}
		else
		{
			vertex.normal.x = 0;
			vertex.normal.y = 1;
			vertex.normal.z = 0;
		}


		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	LoadBones(mesh);
	ReadNodeHeirarchy(scene->mRootNode, scene, -1);
	sort(boneInfo.begin(), boneInfo.end(), [](BoneInfo a,BoneInfo b) {
		return a.parentIndex > b.parentIndex;
	});


	return;
}


#pragma region 텍스쳐
//ID3D11ShaderResourceView * ModelLoader::getTextureFromModel(const aiScene * scene, int textureindex)
//{
//	HRESULT hr;
//	ID3D11ShaderResourceView *texture;
//
//	int* size = reinterpret_cast<int*>(&scene->mTextures[textureindex]->mWidth);
//
//	hr = CreateWICTextureFromMemory(dev, devcon, reinterpret_cast<unsigned char*>(scene->mTextures[textureindex]->pcData), *size, nullptr, &texture);
//	if (FAILED(hr))
//		MessageBox(hwnd, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);
//
//	return texture;
//}

//string determineTextureType(const aiScene * scene, aiMaterial * mat)
//{
//	aiString textypeStr;
//	mat->GetTexture(aiTextureType_DIFFUSE, 0, &textypeStr);
//	string textypeteststr = textypeStr.C_Str();
//	if (textypeteststr == "*0" || textypeteststr == "*1" || textypeteststr == "*2" || textypeteststr == "*3" || textypeteststr == "*4" || textypeteststr == "*5")
//	{
//		if (scene->mTextures[0]->mHeight == 0)
//		{
//			return "embedded compressed texture";
//		}
//		else
//		{
//			return "embedded non-compressed texture";
//		}
//	}
//	if (textypeteststr.find('.') != string::npos)
//	{
//		return "textures are on disk";
//	}
//}
//
//int getTextureIndex(aiString * str)
//{
//	string tistr;
//	tistr = str->C_Str();
//	tistr = tistr.substr(1);
//	return stoi(tistr);
//}
//
//vector<Texture> loadMaterialTextures(aiMaterial * mat, aiTextureType type, string typeName, const aiScene * scene)
//{
//	vector<Texture> textures;
//	for (UINT i = 0; i < mat->GetTextureCount(type); i++)
//	{
//		aiString str;
//		mat->GetTexture(type, i, &str);
//		// Check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
//		bool skip = false;
//		for (UINT j = 0; j < textures_loaded.size(); j++)
//		{
//			if (std::strcmp(textures_loaded[j].path.c_str(), str.C_Str()) == 0)
//			{
//				textures.push_back(textures_loaded[j]);
//				skip = true; // A texture with the same filepath has already been loaded, continue to next one. (optimization)
//				break;
//			}
//		}
//		if (!skip)
//		{   // If texture hasn't been loaded already, load it
//			HRESULT hr;
//			Texture texture;
//			int textureindex = getTextureIndex(&str);
//			getTextureFromModel(scene, textureindex);
//			if (textype == "embedded compressed texture")
//			{
//				int textureindex = getTextureIndex(&str);
//				getTextureFromModel(scene, textureindex);
//			}
//			else
//			{
//				string filename = string(str.C_Str());
//				//filename = directory + '/' + filename;
//				//wstring filenamews = wstring(filename.begin(), filename.end());
//		/*		hr = CreateWICTextureFromFile(dev, devcon, filenamews.c_str(), nullptr, &texture.texture);
//				if (FAILED(hr))
//					MessageBox(hwnd, "Texture couldn't be loaded", "Error!", MB_ICONERROR | MB_OK);*/
//			}
//			texture.type = typeName;
//			texture.path = str.C_Str();
//			textures.push_back(texture);
//			textures_loaded.push_back(texture);  // Store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
//		}
//	}
//	return textures;
//}
//
//void getTextureFromModel(const aiScene * scene, int textureindex)
//{
//	HRESULT hr;
//	
//
//	int* size = reinterpret_cast<int*>(&scene->mTextures[textureindex]->mWidth);
//	scene->mTextures[textureindex]->pcData;
//	//hr = CreateWICTextureFromMemory(dev, devcon, reinterpret_cast<unsigned char*>(scene->mTextures[textureindex]->pcData), *size, nullptr, &texture);
//	/*if (FAILED(hr))
//		MessageBox(hwnd, "Texture couldn't be created from memory!", "Error!", MB_ICONERROR | MB_OK);*/
//
//	
//}
#pragma endregion

void LoadBones(const aiMesh* pMesh)
{
	for (uint i = 0; i < pMesh->mNumBones; i++) {
		BoneInfo info;
		info.BoneOffset = convertXM(pMesh->mBones[i]->mOffsetMatrix);
		info.boneName = pMesh->mBones[i]->mName.C_Str();
		info.index = i;
		//printf("%s\n", pMesh->mBones[i]->mName.C_Str());
		boneInfo.push_back(info);
		
		
		

		boneMap[pMesh->mBones[i]->mName.C_Str()] = i;

		for (uint j = 0; j < pMesh->mBones[i]->mNumWeights; j++) {
			uint VertexID = pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			vertices[VertexID].weights.push_back(Weight);
			vertices[VertexID].boneIndices.push_back(i);
		}
	}
	for (auto& data : vertices)//무게가 4개가 아닐때 0을 넣어준다.
	{
		if (data.weights.size() < 4)
		{
			while (data.weights.size() != 4)
			{
				data.weights.push_back(0);
			}
		}
		if (data.boneIndices.size() < 4)
		{
			while (data.boneIndices.size() != 4)
			{
				data.boneIndices.push_back(0);
			}
		}
		//for (auto& weight : data.weights)
		//{
		//	printf("%f ",weight);
		//}
		//printf("	boneID : ");
		//for (auto& index : data.boneIndices)
		//{
		//	printf("%d ", index);
		//}
		//printf("\n==================================\n");
	}


}


void ReadNodeHeirarchy(const aiNode* pNode, const aiScene* scene, UINT parentIndex)
{

	string NodeName{ pNode->mName.C_Str() };
	
	

	uint BoneIndex = -1;
	if (boneMap.find(NodeName) != boneMap.end()) {
		BoneIndex = boneMap[NodeName];
		boneInfo[BoneIndex].parentIndex = parentIndex;
	}

	for (uint i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(pNode->mChildren[i], scene, BoneIndex);
	}
}
