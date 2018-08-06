// ModelLoader.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include "stdafx.h"


string FILENAME = "walking";


struct VERTEX {
	XMFLOAT3 position;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
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
void processNode(aiNode * node, const aiScene * scene);
void processMesh(aiMesh* mesh, const aiScene* scene);
//string determineTextureType(const aiScene * scene, aiMaterial * mat);
string textype;


// Data to fill
vector<VERTEX> vertices;
vector<UINT> indices;
vector<Texture> textures;
vector<Texture> textures_loaded;
//SkinnedData skinData;


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
	out<< vertices.size()<<" " << "POSITION" << endl;
	for (auto& data : vertices)
	{
		out << data.position.x <<" "<< data.position.y <<" "<< data.position.z<<std::endl;
	}
	out << "UV" << endl;
	for (auto& data : vertices)
	{
		out << data.texcoord.x << " " << data.texcoord.y << std::endl;
	}
	out << "NORMAL" << endl;
	for (auto& data : vertices)
	{
		out << data.normal.x << " " << data.normal.y << " " << data.normal.z << std::endl;
	}

	out << "INDEX" << " " << indices.size() << endl;
	for (auto& data : indices)
	{
		out << data << " ";
	}

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
	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	//	if (textype.empty()) textype = determineTextureType(scene, mat);
	//	printf("%d", textype);
	//}

	// Walk through each of the mesh's vertices

	

	printf("%d\n", scene->mNumAnimations);
	for (int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
	{
		auto key = scene->mAnimations[0]->mChannels[i]->mPositionKeys;
		printf("%f %f %f \n", key->mValue.x, key->mValue.y, key->mValue.z);
	}

	//printf("%s\n", scene->mAnimations[0]->mChannels);


	//for (int i = 0; i < mesh->mNumBones; i++)
	//{
	//	printf("%s\n", mesh->mBones[i]->mName.C_Str());
	//}

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

	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	//	vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
	//	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	//}

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