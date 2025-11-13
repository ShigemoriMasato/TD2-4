#include "DXCommonFunction.h"
#include <Utility/ConvertString.h>
#include <cassert>

IDxcBlob* CompileShader(
    //CompilerするShaderファイルへのパス
    const std::wstring& filePath,
    //Compilerに使用するprofile
    const wchar_t* profile,
    //初期化で生成したものを3つ
    IDxcUtils* dxcUtils,
    IDxcCompiler3* dxcCompiler,
    IDxcIncludeHandler* includeHandler,
    spdlog::logger* logger) {

    logger->info(ConvertString(std::format(L"Begin CompileShader, path: {}, profile: {}\n", filePath, profile)));

    //hlslファイルを読む
    IDxcBlobEncoding* shaderSource = nullptr;
    HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
    //読めなかったら止める
    assert(SUCCEEDED(hr));
    //読み込んだファイルの内容を設定する
    DxcBuffer shaderSourceBuffer;
    shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
    shaderSourceBuffer.Size = shaderSource->GetBufferSize();
    shaderSourceBuffer.Encoding = DXC_CP_UTF8;//utf8の文字コードであることを通知

    LPCWSTR arguments[] = {
        filePath.c_str(),	//コンパイル対象のhlslファイル名
        L"-E", L"main",     //エントリーポイントの指定。基本的にmain以外にはしない
        L"-T", profile,    //ShaderProfileの設定
        L"-Zi", L"-Qembed_debug", //デバッグ用の情報を埋め込む
        L"-Od",     //最適化を行わない
        L"-Zpr",     //メモリレイアウトは行優先
    };
    //実際にShaderをコンパイルする
    IDxcResult* shaderResult = nullptr;
    hr = dxcCompiler->Compile(
        &shaderSourceBuffer,	//読み込んだファイル
        arguments,			    //コンパイルオプション
        _countof(arguments),	//コンパイルオプションの数
        includeHandler,		    //includeが含まれた諸々
        IID_PPV_ARGS(&shaderResult)		//コンパイル結果
    );
    //コンパイルエラーではなくdxcが起動できないなどの致命的な状況
    assert(SUCCEEDED(hr));

    //警告・エラーが出てたらログに出して止める
    IDxcBlobUtf8* shaderError = nullptr;
    shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
    if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
        logger->error(shaderError->GetStringPointer());
        logger->flush();
        //警告・エラーが起きている状態なので止める
        assert(false);
    }

    //コンパイル結果から実行用のバイナリ部分を取得
    IDxcBlob* shaderBlob = nullptr;
    hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
    assert(SUCCEEDED(hr));
    //成功したログを出す
    logger->info(ConvertString(std::format(L"Compile Successd, path: {}, profile: {}\n", filePath, profile)));
    //もう使わないリソースを開放
    shaderSource->Release();
    shaderResult->Release();
    //実行用のバイナリを返却
    return shaderBlob;
}

ID3D12Resource* CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
    //生成するResourceの設定
    D3D12_RESOURCE_DESC resourceDesc{};
    resourceDesc.Width = width;
    resourceDesc.Height = height;
    resourceDesc.MipLevels = 1;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    //利用するヒープの設定
    D3D12_HEAP_PROPERTIES heapProperties{};
    heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

    //深度値のクリア設定
    D3D12_CLEAR_VALUE depthClearValue{};
    depthClearValue.DepthStencil.Depth = 1.0f;
    depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

    //Resourceの生成
    ID3D12Resource* resource = nullptr;
    HRESULT hr = device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        &depthClearValue,
        IID_PPV_ARGS(&resource));
    assert(SUCCEEDED(hr));

    return resource;
}
