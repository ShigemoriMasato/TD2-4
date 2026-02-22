# 説明
このエンジンは、トリプルバッファリングとシェーダーの自由度を重視したエンジンです

## 描画について
現在何かしらを描画するには、モデルを使うのがいちばん手っ取り早いです。
 
まずはエンジンに元からあるモデルを描画してみましょう。以下は簡単な流れです。
### 初期化処理
1. ModelManagerからモデルのデータを貰う
1. DrawDataManagerから頂点情報を貰う
1. DrawResourceを作成する
1. 頂点情報をセット
1. シェーダーをDrawResourceのPSO::Configにセット
1. CBV、SRVを作成
1. その他PSOの各項目を設定(基本は初期値で良い)1. 
### 更新処理
1. 作成したCBV、SRVに渡したい値をコピーする
### 描画処理
1. Command::Object*を引っ張ってきて描画

初期化が重いのは、カスタムできるDrawDataの作成と、Shaderの自由度を優先した代償です。受け入れてください。

以下、サンプルコードです

``` c++
//================
//初期化処理
//================
//インスタンスの作成
std::unique_ptr<SHEngine::RenderObject> renderObject = std::make_unique<SHEngine::RenderObject>("DebugName");
renderObject->Initialize();

//Shaderの設定
renderObject->psoConfig_.vs = "DebugVS.hlsl";
renderObject->psoConfig_.ps = "DebugPS.hlsl";

//CBV,SRVの設定
int colorIndex = renderObject->CreateCBV(sizeof(Vector4), ShaderType::PIXEL_SHADER, "DebugName");
int textureIndex = renderObject->CreateCBV(sizeof(int), ShaderType::PIXEL_SHADER, "DebugName");
int wvpIndex = renderObject->CreateSRV(sizeof(Matrix4x4), 100, ShaderType::VERTEX_SHADER, "DebugName");

//Textureを使用するかどうか
renderObject->SetUseTexture(true);

//================
//更新処理
//================
Vector4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
renderObject->CopyBufferData(colorIndex, &color, sizeof(Vector4));

int texture = 0;
renderObject->CopyBufferData(textureIndex, &texture, sizeof(int));

std::vector<Matrix4x4> wvp{};
renderObject->CopyBufferData(wvpIndex, wvp.data(), sizeof(Matrix4x4) * wvp.size());

//================
//描画処理
//================
CmdObj* cmdObj = commonData_->cmdObject.get();
renderObject->Draw(cmdObj);
```

以下、描画されない時のよくあるチェックシートです
- [ ] デバッグで描画してみてください。何かしらのアサートに引っかかるかもしれません。
- [ ] インスタンスの数は1以上になっているか
- [ ] コピーの時にエラーログが発生していないか
- [ ] PIX等で見てください。裏面を描画してたり、カメラ行列がおかしいかもしれません。

## ポストエフェクトについて
ポストエフェクトを使うには、ウィンドウを作成、ポストエフェクトを設定して描画という2ステップがあります。

### ウィンドウの作り方、使い方
ウィンドウの推奨される型はMultiDisplayです。基本これを使ってください。SingleDisplayはSwapChain又はMultiDisplayの素材です。

MultiDisplayのインスタンスを作ってInitialize()したらもうそれはウィンドウです。

PreDrawでレンダーターゲットの切り替え、塗りつぶしをします。

PostDrawとToTextureはどちらも同じ効果です。シェーダーで使えるようにバリアを貼り直します。

### ポストエフェクトの使い方
1. ポストエフェクトのインスタンスを作成します
1. ポストエフェクトのコンフィグのインスタンスを作成します
1. コンフィグにCommand::Object*と元の画像、出力先を設定します(この時、出力先がnullptrだと元の画像の場所に上書きされます)
1. コンフィグにどんな仕事をさせるかをjobにOR演算子でぶち込みます
1. ポストエフェクトのDraw()を作成したコンフィグを引数に実行。

ポストエフェクトが終わった時、どちらもPostDrawを行ったあとなので、次何か描画する際はPreDrawが必要です。
