#pragma once
#include <memory>
#include "Data/BinaryOutput.h"
#include "Data/BinaryInput.h"
#include <string>
#include <vector>

//std::vectorと、std::stringやstd::vectorを含むクラス以外の型に対応

/**
 * @class BinaryManager
 * @brief バイナリデータの読み書きを管理するクラス
 * 
 * Assets/Bin内のデータをバイナリ形式で保存・読み込みする機能を提供。
 * 様々な型のデータをValueとして登録し、ファイルへの書き込みや読み込みが可能。
 * std::vector、std::string、およびこれらを含むクラスに対応する。
 */
class BinaryManager {
public:

	/// @brief コンストラクタ。保存先ディレクトリを作成する
	BinaryManager();
	
	/// @brief デストラクタ
	~BinaryManager();

	/**
	 * @brief 出力データを登録
	 * @tparam T データ型
	 * @param value 登録する値
	 * @param name 値の名前（オプション）
	 */
	template<typename T>
	void RegisterOutput(T value, std::string name = "") {
		values_.push_back(std::make_shared<Value<T>>(value, name));
	};

	/**
	 * @brief ValueBaseを直接登録
	 * @param value 登録するValueBase
	 */
	void RegisterOutput(ValueBase* value) {
		values_.push_back(value->Clone());
	}

	/**
	 * @brief ValueBaseから元の型に復元
	 * @tparam T 復元する型
	 * @param value ValueBase
	 * @return 復元された値
	 */
	template<typename T>
	static T Reverse(ValueBase* value) {
		return static_cast<Value<T>*>(value)->value;
	}

	/**
	 * @brief 登録したデータをファイルに書き込む
	 * 
	 * 実行後、登録したValueはクリアされる。
	 * @param fileName ファイル名（拡張子なし）
	 */
	void Write(std::string fileName);

	/**
	 * @brief ファイルからValueを読み込む
	 * @param fileName ファイル名
	 * @return 読み込んだValueの配列
	 */
	std::vector<std::shared_ptr<ValueBase>> Read(std::string fileName);

private:

	/// @brief バイナリ入力処理クラス
	static std::unique_ptr<BinaryInput> input_;
	/// @brief バイナリ出力処理クラス
	static std::unique_ptr<BinaryOutput> output_;
	/// @brief 登録されたValueのリスト
	std::vector<std::shared_ptr<ValueBase>> values_;

	/// @brief データの保存先パス
	static const std::string basePath_;
};
