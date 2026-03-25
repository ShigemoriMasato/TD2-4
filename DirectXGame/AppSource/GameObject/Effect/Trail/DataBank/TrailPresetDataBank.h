#pragma once
#include <unordered_map>
#include <variant>
#include "GameObject/Effect/Trail/Preset/TrailPreset.h"
#include <Tool/Json/JsonManager.h>

using TrailPresetVariant = std::variant<RibbonTrailConfig, ShockwaveRingConfig>;

class TrailPresetDataBank
{
public:
	/// <summary>
	/// Assets/Json/<name>.jsonの内容を取得する
	/// </summary>
	/// <param name="name">例："Axe_Ribbon" → Assets/Json/Axe_Ribbon.json</param>
	/// <returns> variant型に入っている </returns>
	const TrailPresetVariant& Get(const std::string& name);

	/// <summary>
	/// キャッシュから削除して次回Getしたときに再読込させる
	/// </summary>
	/// <param name="name"> 例："Axe_Ribbon" → Assets/Json/Axe_Ribbon.json </param>
	void Invalidate(const std::string& name);

	/// <summary>
	/// 全キャッシュ削除。Getしたときに再読込させる
	/// </summary>
	void Clear();

	/// <summary>
	/// Getした場合の型を返す
	/// </summary>
	/// <param name="name"> 例："Axe_Ribbon" → Assets/Json/Axe_Ribbon.json </param>
	/// <returns> Getしたときの型 </returns>
	TrailType GetTypeOf(const std::string& name);

private:
	TrailPresetVariant Load_(const std::string& name);
	Trail::Config LoadConfig_(JsonManager& json);

private:
	// 読み込みにのみ使用
	JsonManager json_;
	// データの保存にのみ使用。
	static std::unordered_map<std::string, TrailPresetVariant> cache_;
};