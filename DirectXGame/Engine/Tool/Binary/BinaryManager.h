#pragma once
#include <string>
#include "Value.h"

using BinaryData = std::string;

class BinaryManager {
public:

	template<typename T>
	void RegisterOutput(T* data);
	void Write(const std::string& fileName);

	BinaryData Read(const std::string& fileName);
	template<typename T>
	T Reverse(BinaryData& buffer);

private:

	BinaryData binaryBuffer_;

	static inline const std::string basePath = "Assets/Binary/";

	static constexpr size_t idSize = sizeof(TypeID);
	static constexpr size_t sizeSize = sizeof(size_t);
	static constexpr size_t headerSize = idSize + sizeSize;
};

template<typename T>
void BinaryManager::RegisterOutput(T* data) {
	constexpr TypeID currentID = TypeIDResolver<T>::id;
	size_t size = sizeof(T);

	// 未対応の型の場合は登録しない
	if (currentID == TypeID::kUnknown) {
		return;
	}

	/* ID->Size->値 */
	binaryBuffer_.append(reinterpret_cast<const char*>(&currentID), sizeof(TypeID));
	binaryBuffer_.append(reinterpret_cast<const char*>(&size), sizeof(size_t));
	binaryBuffer_.append(reinterpret_cast<const char*>(data), size);
}

template<typename T>
T BinaryManager::Reverse(BinaryData& buffer) {
	T value{};

	if (buffer.size() < headerSize) {
		return value;
	}

	TypeID id;
	size_t size;

	std::memcpy(&id, buffer.data(), idSize);
	std::memcpy(&size, buffer.data() + idSize, sizeSize);

	if (id != TypeIDResolver<T>::id) {
		return value;
	}

	if (buffer.size() < headerSize + size) {
		return value;
	}

	std::memcpy(&value, buffer.data() + headerSize, sizeof(T));
	buffer.erase(0, headerSize + size);
	return value;
}
