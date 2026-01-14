#include "DSVManager.h"
#include <stdexcept>

DSVManager::DSVManager(ID3D12Device* device, uint32_t size, int num) : maxCount(num), descriptorSizeDSV(size) {

	srvDescriptorHeap.Attach(CreateDescriptorHeap(device, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, maxCount, false));

	isUsed_.resize(maxCount, false);
}

uint32_t DSVManager::GetNextOffset(int startIndex) {
	uint32_t i = startIndex;

	for (i; i < maxCount; ++i) {
		if (!isUsed_[i]) {
			return i;
		}
	}

	throw std::runtime_error("No available DSV descriptor slots.");
}

DSVHandle::~DSVHandle() {
	if (manager_)
		manager_->isUsed_[offset_] = false;
}

void DSVHandle::UpdateHandle(DSVManager* manager, int operateIndex) {
	if (isUpdated_)
		return;

	offset_ = manager->GetNextOffset(operateIndex);
	manager_ = manager;
	CPU = GetCPUDescriptorHandle(manager->GetHeap(), manager->descriptorSizeDSV, offset_);
	manager_->isUsed_[offset_] = true;

	isUpdated_ = true;
}
