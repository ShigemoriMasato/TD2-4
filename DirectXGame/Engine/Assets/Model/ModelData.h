#pragma once
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Utility/DataStructures.h>
#include <vector>
#include <string>

struct Material {
	int textureIndex;
};

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

struct NodeModelData {
	//VertexBufferView関連
	std::vector<VertexData> vertices{};
	std::vector<uint32_t> materialIndex{};

	//IndexBufferView関連
	std::vector<uint32_t> indices{};

	//その他
	Node rootNode{};
	std::vector<Material> materials{};
};

struct SkinningModelData {
	//VertexBufferView関連
	std::vector<VertexData> vertices{};
	std::vector<uint32_t> materialIndex{};

	//IndexBufferView関連
	std::vector<uint32_t> indices{};

	//その他
	std::vector<Material> materials{};
};
