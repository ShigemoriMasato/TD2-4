#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include <Tool/Json/JsonManager.h>
#include "GameObject/Effect/Trail/Preset/TrailPreset.h"

using TrailPresetVariant = std::variant<RibbonTrailConfig, ShockwaveRingConfig>;

class TrailPresetDataBank
{
public:
	/// <summary>
	/// Assets/Json/Trail/<name>.jsonの内容を取得する
	/// </summary>
	/// <param name="name">例："Axe_Ribbon" → Assets/Json/Trail/Axe_Ribbon.json</param>
	/// <returns> variant型に入っている </returns>
	const TrailPresetVariant& Get(const std::string& name);

	/// <summary>
	/// キャッシュから削除。エディタ用。
	/// </summary>
	/// <param name="name"> 例："Axe_Ribbon" → Assets/Json/Trail/Axe_Ribbon.json </param>
	void Invalidate(const std::string& name);

	/// <summary>
	/// 全キャッシュ削除。エディタ用。
	/// </summary>
	void Clear();

	/// <summary>
	/// Getした場合の型を返す
	/// </summary>
	/// <param name="name"> 例："Axe_Ribbon" → Assets/Json/Trail/Axe_Ribbon.json </param>
	/// <returns> Getしたときの型 </returns>
	TrailType GetTypeOf(const std::string& name);

	/// <summary>
	/// 生成済みのConfigを Assets/Json/Trail/<name>.json として保存する
	/// </summary>
	void Save(const std::string& name, const Trail::Config& cfg, RibbonTrailConfig& ribbonPreset);
	void Save(const std::string& name, const Trail::Config& cfg, ShockwaveRingConfig& shockPreset);

	/// <summary>
	/// Assets/Json/Trail/<name>.json からプリセットを読み込む
	/// </summary>
	/// <param name="name">例："Fountain_01" → Assets/Json/Trail/Fountain_01.json</param>
	/// <returns></returns>
	TrailPresetVariant Load(const std::string& name);
	Trail::Config LoadConfig(JsonManager& json);

private:
	// 読み込みにのみ使用
	JsonManager json_;
	// データの保存にのみ使用。
	static std::unordered_map<std::string, TrailPresetVariant> cache_;
};