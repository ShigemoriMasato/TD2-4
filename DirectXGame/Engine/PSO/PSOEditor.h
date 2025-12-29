#pragma once
#include <PSO/PSOManager.h>

//現在設定されてるPSOを変更するクラス
class PSOEditor {
public:

	PSOEditor(ID3D12Device* device);
	~PSOEditor() = default;

	void Initialize(ID3D12Device* device);

	void FrameInitialize(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 設定した内容でPSOを設定し、内部の設定をデフォルトに戻す。(デフォルトはEngine/PSO/PSOConfigへ)
	/// </summary>
	/// <param name="commandList_"></param>
	void SetPSO(ID3D12GraphicsCommandList* commandList, const PSOConfig& config);

private:

	std::unique_ptr<PSOManager> psoManager_;
	
	PSOConfig nowConfig_{};

	std::shared_ptr<spdlog::logger> logger_;

	bool isFirst_ = true;

};


