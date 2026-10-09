# Final27-Assignment：3D FPS ゲームフレームワーク

これは C++20、Visual Studio 2022、MSVC v143、DxLib を使用した Windows x64 向け 3D ファーストパーソンゲームフレームワークです。現在は TestScene を統合テスト用シーンとして使用し、レンダリングパイプライン、リソースのライフサイクル、衝突処理の基礎、VFX Production Foundation V1 を検証しています。

## 現在の完成状況

実装済みの主な機能：

- Model、Shader、Texture のリソース抽象化と RAII ライフサイクル管理
- Scene Color、Linear Scene Depth、Volumetric Lighting、Screen Overlay の各レンダリングパス
- Texture、Alpha/Additive Blend、幅・高さ、回転に対応した Billboard バッチ描画
- 速度、加速度、寿命、サイズ補間、透明度減衰、回転、固定乱数 Seed に対応した Particle Burst
- Metal Spark、Blood Spray、Low Health Screen Effect
- VFX 単体テスト、メインプログラムのビルド、起動・終了確認

## 依存方向

プロジェクトの依存方向は次のとおりです。

    Game
      ↓
    Engine
      ↓
    Platform Abstraction
      ↓
    DxLib

Game Recipe はゲーム固有のパラメータだけを扱い、DxRenderer に依存しません。Engine の Effect は Renderer だけを呼び出し、DxLib を直接呼び出しません。DxLib 固有の型とハンドルは Platform/DxLib 境界に限定します。

## 1フレームのレンダリングフロー

    BeginFrame
      → Scene Color Pass
        → BeginSceneRender
        → SceneManager::Render
        → EndSceneRender
      → Linear Scene Depth Pass
        → BeginSceneDepthRender
        → SceneManager::RenderDepth
        → EndSceneDepthRender
      → Volumetric Lighting Composition
      → Scene Overlay Pass
        → SceneManager::RenderOverlay
      → EndFrame

Scene Depth には、Volumetric Lighting を遮蔽する Opaque Geometry だけを描画します。透明パーティクルは SceneDepth に書き込みません。Low Health Overlay は Volumetric Lighting の後に描画されます。

## ディレクトリ構成

    Final27-Assignment/
    ├─ Assets/
    │  ├─ Models/
    │  ├─ Textures/BloodDroplet.png
    │  └─ Shaders/Source/
    │     ├─ LowHealthOverlayPS.hlsl / .pso
    │     └─ VolumetricLightingPS.hlsl / .pso
    ├─ src/
    │  ├─ Engine/
    │  │  ├─ Core、Effects、Input、Math、Physics
    │  │  ├─ Rendering/Billboard、Camera、Model、Shader、Texture
    │  │  ├─ Resources
    │  │  └─ Scene
    │  ├─ Game/Effects、Player、Scenes
    │  ├─ Platform/DxLib
    │  └─ Main.cpp
    ├─ tests/VisualEffectTests.cpp
    ├─ CMakeLists.txt
    ├─ DxLib.sln
    ├─ DxLib.vcxproj
    └─ README.md

## コアモジュール

### Application、Scene、Platform

Application は初期化、メインループ、SceneManager、ResourceSystem、終了処理を担当します。SceneManager は unique_ptr で現在の Scene を所有し、Scene 切り替えを遅延適用します。IScene は OnEnter、OnExit、Update、Render、RenderDepth、RenderOverlay のライフサイクルを提供します。

DxApplication は DxLib の初期化、ウィンドウイベント、画面クリア、画面更新を管理します。DxInput は DxLib のキーボード・マウス状態を Engine の入力インターフェースへ変換します。

### Renderer とリソース

Renderer は Engine 層の描画 Facade です。IRendererBackend はリソース作成、モデル描画、Billboard、Depth、Volumetric Lighting、Screen Overlay のインターフェースを定義します。DxRenderer は DxLib のリソース作成、モデル描画、Billboard バッチ描画、Linear Scene Depth、全画面合成を実装します。

ResourceSystem は ModelLoader、ShaderLoader、TextureLoader を通してリソースを作成し、正規化したパスと weak_ptr によるキャッシュを使用します。Model、Shader、Texture は Game 層へ DxLib ハンドルを公開しません。DxModelResource、DxShaderResource、DxTextureResource が RAII による解放を担当します。

### Billboard と Particle

BillboardRenderData は位置、個別の幅・高さ、回転、RGB カラー、透明度を持ちます。BillboardDrawSettings は Alpha、Additive、Batch 全体で共有する Texture に対応します。

DxLib Billboard Backend の動作：

- すべてのパーティクルを Triangle List として構築
- 1回の DrawBillboards() につき通常1回の DxLib Draw Call
- Depth Test は有効
- Depth Write は無効
- Lighting、Fog、Cull、Blend などの状態を描画後に復元
- 独立した SceneDepth には書き込まない

ParticleBurstEffect は毎フレーム、速度、加速度、位置、回転、サイズ、透明度を更新します。各 Burst は自身の randomSeed で mt19937 を初期化し、グローバルな乱数管理クラスには依存しません。

## 現在の VFX

### Metal Spark

MakeMetalSparkBurst() は src/Game/Effects/ にあります。

- Additive Blend
- 細長く明るいパーティクル
- 短いライフタイム
- 外側へ飛散
- Y 軸方向の重力

TestScene では Space キーで発生します。

### Blood Spray

MakeBloodSprayBurst() は次の設定を使用します。

- Alpha Blend
- BloodDroplet.png
- 暗い赤色
- 明確な重力落下
- ランダム回転
- 空中の血液飛沫だけを表現し、Decal、Bullet Hole、血痕は作成しない

TestScene では B キーで発生します。

### Low Health Screen Effect

LowHealthScreenEffect は VisualEffectSystem とは分離された、状態駆動の継続的な画面エフェクトです。

- healthRatio が 0.30 以上：赤い枠を描画しない
- healthRatio が 0.30 未満：危険度に応じて強度を上げる
- 強度は正弦波で周期的に変化
- 呼吸カーブの最低値は 0.0
- Shader は Screen UV から端部 Vignette を計算
- SceneDepth と SceneColor はサンプリングしない
- Alpha Blend を使用
- 描画中は Depth Test と Depth Write を無効にし、描画後に復元

TestScene は起動時のテスト HP を 10% に設定します。H キーで減少し、J キーで回復します。

## TestScene の操作

| 操作 | 効果 |
| --- | --- |
| マウス移動 | FPS 視点の回転 |
| W / A / S / D | 水平移動 |
| Space | Metal Spark |
| B | Blood Spray |
| H | テスト HP を減少 |
| J | テスト HP を回復 |
| ウィンドウを閉じる | プログラムを終了 |

Space、B、H、J はキーの押下エッジで処理されるため、押し続けても毎フレーム Effect を生成しません。

## ビルドと実行

DxLib の既定の依存パス：

    C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用

プロジェクトルートから Debug x64 をビルド：

    & "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" DxLib.vcxproj /m /p:Configuration=Debug /p:Platform=x64 /p:LinkIncremental=false
    .\x64\Debug\DxLib.exe

実行時の作業ディレクトリはプロジェクトルートにしてください。Assets/ の相対パスは作業ディレクトリを基準に解決されます。

CMake を使用する場合：

    cmake -B build -S . -G "Visual Studio 17 2022" -A x64 -DDXLIB_DIR="C:/DxLib_VC/プロジェクトに追加すべきファイル_VC用"
    cmake --build build --config Debug
    .\build\Debug\3DFPSGameFramework.exe

## テスト

VFX テストプロジェクトは tests/VisualEffectTests.vcxproj です。次の内容を検証します。

- パーティクルの移動、加速度、ライフタイム
- 幅・高さの補間、透明度減衰、回転
- Alpha / Additive Blend
- Effect の所有権、解放、繰り返し生成
- Space キーのエッジトリガー
- Metal Spark Recipe
- Blood Spray Recipe
- Low Health の閾値
- Low Health の脈動範囲

    & "C:\Program Files\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe" tests/VisualEffectTests.vcxproj /m /p:Configuration=Debug /p:Platform=x64 /p:LinkIncremental=false
    .\x64\VFXTests\VisualEffectTests.exe

成功時の出力：

    VFX tests passed: movement, fade, ownership, cleanup, repeated spawn, Space edge, Blood recipe, Low health overlay.

メインプログラムのビルド、VFX テスト、起動・終了確認は完了しています。

## 現在の制約

- 現在は TestScene のみで、完成したゲームフロー、武器システム、敵 AI はありません。
- テスト HP は TestScene 内の一時変数であり、正式な Health Component ではありません。
- Metal Spark の位置と法線はテスト値で、射線検出や実際の命中イベントには接続していません。
- Blood Spray は壁面の血痕、Decal、Bullet Hole、Trail、Ribbon を持ちません。
- GPU Particle、Compute Shader、Particle Collision、Volumetric Smoke、Shadowed Particle は未実装です。
- 衝突処理は離散位置検出と有限回数の AABB 押し出しを使用するため、大きな移動量では障害物を通過する可能性があります。
- リソース読み込みは同期処理です。欠落リソースは主に assert で通知され、実行時の復旧処理は未実装です。
- TestScene には一部 DxLib 直接呼び出しのデバッグ描画が残っています。正式な Game 層の分離では今後移行できます。
- ビルド時にファイルエンコーディング警告や非致命的なリンカー最適化警告が出る場合がありますが、現在の検証結果は 0 エラーです。
- MSVC で LNK1101 MSPDB140.DLL のデバッグデータベース競合が発生した場合は、該当する Debug 設定の GenerateDebugInformation を一時的に無効にして再リンクし、完了後に設定を戻してください。

## 今後の拡張ルール

World VFX は次の経路を使用します。

    Game Effect Recipe
      → ParticleBurstEffect
      → Renderer
      → IRendererBackend
      → DxRenderer
      → DxLib

Screen VFX は次の経路を使用します。

    LowHealthScreenEffect
      → Renderer::DrawLowHealthOverlay
      → DxRenderer
      → BackBuffer

現在の段階では ParticleManager、EffectManager、TextureManager、ScreenEffectManager などの重複 Manager を追加しません。ResourceSystem、VisualEffectSystem、個別の Effect と Recipe で十分に構成できます。
