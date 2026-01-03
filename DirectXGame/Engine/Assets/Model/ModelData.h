#pragma once
#include <Utility/Vector.h>
#include <Utility/Matrix.h>
#include <Utility/Quaternion.h>
#include <Utility/DataStructures.h>
#include <vector>
#include <string>
#include <unordered_map>

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
	int drawDataIndex = -1;
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

template <typename T>
struct Keyframe {
	float time;
	T value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template<typename T>
struct AnimationCurve {
	std::vector<Keyframe<T>> keyframes;
};

struct NodeAnimation {
	AnimationCurve<Vector3> position;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};

struct Animation {
	float duration;
	std::unordered_map<std::string, NodeAnimation> nodeAnimations;
};
