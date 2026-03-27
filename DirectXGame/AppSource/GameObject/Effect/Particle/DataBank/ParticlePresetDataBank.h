#pragma once
#include <unordered_map>
#include <variant>
#include <string>

#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

using ParticlePresetVariant = std::variant<FountainConfig, OnTrailConfig>;

class ParticlePresetDataBank
{
public:
	/// <summary>
	/// Assets/Json/<name>.json の内容を取得する
	/// </summary>
	/// <param name="name">例："Fountain_01" → Assets/Json/Fountain_01.json</param>
	/// <returns>variant型に入っている</returns>
	const ParticlePresetVariant& Get(const std::string& name);

	/// <summary>
	/// キャッシュから削除して次回Getしたときに再読込させる
	/// <param name="name">例："Fountain_01" → Assets/Json/Fountain_01.json</param>
	/// </summary>
	void Invalidate(const std::string& name);

	/// <summary>
	/// 全キャッシュ削除。Getしたときに再読込させる
	/// <param name="name">例："Fountain_01" → Assets/Json/Fountain_01.json</param>
	/// </summary>
	void Clear();

	/// <summary>
	/// Getした場合の型を返す
	/// <param name="name">例："Fountain_01" → Assets/Json/Fountain_01.json</param>
	/// </summary>
	ParticleType GetTypeOf(const std::string& name);

private:
	ParticlePresetVariant Load_(const std::string& name);
	Particle::Config LoadConfig_(JsonManager& json);

private:
	JsonManager json_;
	static std::unordered_map<std::string, ParticlePresetVariant> cache_;
};