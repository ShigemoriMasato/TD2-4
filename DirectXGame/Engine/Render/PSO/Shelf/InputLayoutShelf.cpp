#include "InputLayoutShelf.h"

using namespace SHEngine::PSO;

InputLayoutShelf::InputLayoutShelf() {

	inputLayouts_.resize(int(InputLayoutID::Count));
	inputElementsList_.resize(static_cast<int>(InputLayoutID::Count));
	{
		inputElementsList_[int(InputLayoutID::Default)].resize(3);

		//InputLayout
		inputElementsList_[int(InputLayoutID::Default)][0].SemanticName = "POSITION";
		inputElementsList_[int(InputLayoutID::Default)][0].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Default)][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementsList_[int(InputLayoutID::Default)][0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Default)][1].SemanticName = "TEXCOORD";
		inputElementsList_[int(InputLayoutID::Default)][1].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Default)][1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementsList_[int(InputLayoutID::Default)][1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Default)][2].SemanticName = "NORMAL";
		inputElementsList_[int(InputLayoutID::Default)][2].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Default)][2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementsList_[int(InputLayoutID::Default)][2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementsList_[int(InputLayoutID::Default)].data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementsList_[int(InputLayoutID::Default)].size());

		inputLayouts_[int(InputLayoutID::Default)] = inputLayoutDesc;
	}

	{
		inputElementsList_[int(InputLayoutID::Textured)].resize(4);

		//InputLayout
		inputElementsList_[int(InputLayoutID::Textured)][0].SemanticName = "POSITION";
		inputElementsList_[int(InputLayoutID::Textured)][0].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Textured)][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementsList_[int(InputLayoutID::Textured)][0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Textured)][1].SemanticName = "TEXCOORD";
		inputElementsList_[int(InputLayoutID::Textured)][1].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Textured)][1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementsList_[int(InputLayoutID::Textured)][1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Textured)][2].SemanticName = "NORMAL";
		inputElementsList_[int(InputLayoutID::Textured)][2].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Textured)][2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementsList_[int(InputLayoutID::Textured)][2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Textured)][3].SemanticName = "TEXTURE";
		inputElementsList_[int(InputLayoutID::Textured)][3].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Textured)][3].Format = DXGI_FORMAT_R32_UINT;
		inputElementsList_[int(InputLayoutID::Textured)][3].InputSlot = 1;
		inputElementsList_[int(InputLayoutID::Textured)][3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementsList_[int(InputLayoutID::Textured)].data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementsList_[int(InputLayoutID::Textured)].size());

		inputLayouts_[int(InputLayoutID::Textured)] = inputLayoutDesc;
	}

	{
		inputElementsList_[int(InputLayoutID::Vector3)].resize(1);

		//InputLayout
		inputElementsList_[int(InputLayoutID::Vector3)][0].SemanticName = "POSITION";
		inputElementsList_[int(InputLayoutID::Vector3)][0].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Vector3)][0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementsList_[int(InputLayoutID::Vector3)][0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementsList_[int(InputLayoutID::Vector3)].data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementsList_[int(InputLayoutID::Vector3)].size());

		inputLayouts_[int(InputLayoutID::Vector3)] = inputLayoutDesc;
	}

	{
		inputElementsList_[int(InputLayoutID::Skinning)].resize(5);

		//InputLayout
		inputElementsList_[int(InputLayoutID::Skinning)][0].SemanticName = "POSITION";
		inputElementsList_[int(InputLayoutID::Skinning)][0].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Skinning)][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementsList_[int(InputLayoutID::Skinning)][0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Skinning)][1].SemanticName = "TEXCOORD";
		inputElementsList_[int(InputLayoutID::Skinning)][1].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Skinning)][1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementsList_[int(InputLayoutID::Skinning)][1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Skinning)][2].SemanticName = "NORMAL";
		inputElementsList_[int(InputLayoutID::Skinning)][2].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Skinning)][2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputElementsList_[int(InputLayoutID::Skinning)][2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

		inputElementsList_[int(InputLayoutID::Skinning)][3].SemanticName = "WEIGHT";
		inputElementsList_[int(InputLayoutID::Skinning)][3].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Skinning)][3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementsList_[int(InputLayoutID::Skinning)][3].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementsList_[int(InputLayoutID::Skinning)][3].InputSlot = 1;

		inputElementsList_[int(InputLayoutID::Skinning)][4].SemanticName = "INDEX";
		inputElementsList_[int(InputLayoutID::Skinning)][4].SemanticIndex = 0;
		inputElementsList_[int(InputLayoutID::Skinning)][4].Format = DXGI_FORMAT_R32G32B32A32_SINT;
		inputElementsList_[int(InputLayoutID::Skinning)][4].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementsList_[int(InputLayoutID::Skinning)][4].InputSlot = 1;

		D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
		inputLayoutDesc.pInputElementDescs = inputElementsList_[int(InputLayoutID::Skinning)].data();
		inputLayoutDesc.NumElements = static_cast<UINT>(inputElementsList_[int(InputLayoutID::Skinning)].size());

		inputLayouts_[int(InputLayoutID::Skinning)] = inputLayoutDesc;
	}
}

InputLayoutShelf::~InputLayoutShelf() {
}

D3D12_INPUT_LAYOUT_DESC InputLayoutShelf::GetInputLayoutDesc(InputLayoutID id) const {
	return inputLayouts_[int(id)];
}
