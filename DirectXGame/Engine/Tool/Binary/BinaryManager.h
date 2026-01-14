#pragma once
#include <memory>
#include "Data/BinaryOutput.h"
#include "Data/BinaryInput.h"
#include <string>
#include <vector>

//std::vectorと、std::stringやstd::vectorを含むクラス以外の型に対応

/// <summary>
/// Assets/Binの中身をバイナリ形式で色々するクラス
/// </summary>
class BinaryManager {
public:

	BinaryManager();
	~BinaryManager();

	template<typename T>
	void RegisterOutput(T value, std::string name = "") {
		values_.push_back(std::make_shared<Value<T>>(value, name));
	};

	void RegisterOutput(ValueBase* value) {
		values_.push_back(value->Clone());
	}

	template<typename T>
	static T Reverse(ValueBase* value) {
		return static_cast<Value<T>*>(value)->value;
	}

	/// <summary>
	/// 実行後RegisterしたValueはflushします
	/// </summary>
	void Write(std::string fileName);

	/// <summary>
	/// 指定したファイルからValueを作成します
	/// </summary>
	/// <returns>新たに生成したValue配列</returns>
	std::vector<std::shared_ptr<ValueBase>> Read(std::string fileName);

private:

	static std::unique_ptr<BinaryInput> input_;
	static std::unique_ptr<BinaryOutput> output_;
	std::vector<std::shared_ptr<ValueBase>> values_;

	static const std::string basePath_; // データの保存先パス
};
