#include "Status.h"
#include <algorithm>

void Status::AddModifier(const StatusModifier& mod) {
	modifiers_.push_back(mod);

	// ソート(Flat -> PercentAdd -> PercentMultの順)
	std::sort(modifiers_.begin(), modifiers_.end(), [](const StatusModifier& a, const StatusModifier& b) { return static_cast<int>(a.type_) < static_cast<int>(b.type_); });

	isDirty_ = true;
}

void Status::RemoveModifierFromSource(const void* source) {
	// 特定のsourceを持つModifierを削除
	auto it = std::remove_if(modifiers_.begin(), modifiers_.end(), [source](const StatusModifier& mod) { return mod.source_ == source; });

	if (it != modifiers_.end()) {
		modifiers_.erase(it, modifiers_.end());
		isDirty_ = true;
	}
}
