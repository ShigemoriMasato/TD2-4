#pragma once
#include <unordered_map>
#include <variant>
#include <string>
#include <Tool/Json/JsonManager.h>
#include <GameObject/Effect/Particle/Preset/ParticlePreset.h>

using ParticlePresetVariant = std::variant<
	PhysicsConfig,
	OnTrailConfig,
	GoToTargetConfig,
	BillboardScaleConfig,
	BillboardScale2Config,
	BillboardColorConfig
>;

class ParticlePresetDataBank
{
public:
	/// <summary>
	/// Assets/Json/Particle/<name>.json の内容を取得する
	/// </summary>
	/// <param name="name">例："Fountain_01" → Assets/Json/Particle/Fountain_01.json</param>
	/// <returns>variant型に入っている</returns>
	const ParticlePresetVariant& Get(const std::string& name);

	/// <summary>
	/// キャッシュから削除。エディタ用。
	/// </summary>
	/// <param name="name">例："Fountain_01" → Assets/Json/Fountain_01.json</param>
	void Invalidate(const std::string& name);

	/// <summary>
	/// 全キャッシュ削除。エディタ用。
	/// </summary>
	void Clear();

	/// <summary>
	/// Getした場合の型を返す
	/// <param name="name">例："Fountain_01" → Assets/Json/Fountain_01.json</param>
	/// </summary>
	/// <returns> Getしたときの型 </returns>
	ParticleType GetTypeOf(const std::string& name);

	/// <summary>
	/// 生成済みのConfigを Assets/Json/Particle/<name>.json として保存する
	/// </summary>
	void Save(const std::string& name, PhysicsConfig& uniqueConfig);
	void Save(const std::string& name, GoToTargetConfig& uniqueConfig);
	void Save(const std::string& name, OnTrailConfig& uniqueConfig);
	void Save(const std::string& name, BillboardScaleConfig& uniqueConfig);
	void Save(const std::string& name, BillboardScale2Config& uniqueConfig);
	void Save(const std::string& name, BillboardColorConfig& uniqueConfig);
	void SaveParticleSRT(JsonManager& json, const std::string& keyPrefix, const ParticleSRT& srt);
	void SaveParticleSRTfloat4(JsonManager& json, const std::string& keyPrefix, const ParticleSRTfloat4& srt);


	/// <summary>
	/// Assets/Json/Particle/<name>.json からプリセットを読み込む
	/// </summary>
	/// <param name="name">例："Fountain_01" → Assets/Json/Particle/Fountain_01.json</param>
	/// <returns></returns>
	ParticlePresetVariant Load(const std::string& name);
	Particle::Config LoadConfig(JsonManager& json);
	ParticleSRT LoadParticleSRT(JsonManager& json, const std::string& keyPrefix);
	ParticleSRTfloat4 LoadParticleSRTfloat4(JsonManager& json, const std::string& keyPrefix);


private:
	// 読み込みにのみ使用
	JsonManager json_;
	// データの保存にのみ使用。
	static std::unordered_map<std::string, ParticlePresetVariant> cache_;
};