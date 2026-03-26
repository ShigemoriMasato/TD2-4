#pragma once
#include <Core/DXDevice.h>

enum class BufferType : uint8_t {
	CBV = 1 << 0,
	SRV = 1 << 1,
	UAV = 1 << 2,
	RTV = 1 << 3,
	DSV = 1 << 4,
	ReadBack = 1 << 5,
};

namespace SHEngine {

	struct ResourceDesc {
		BufferType type;
		D3D12_RESOURCE_DESC desc;
	};

	class Resource {
	public:
		Resource(ID3D12Resource* resource);
		Resource(ResourceDesc& desc);

	private:

	};

}

