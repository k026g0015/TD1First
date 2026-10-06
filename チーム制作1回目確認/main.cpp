#include <Novice.h>
#include <Windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "player.h"
#include "scene.h"
const char kWindowTitle[] = "チーム制作1回目1129班";
// Windowsアプリでのエントリーポイント(main関数
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
#pragma region 画面フルスクリーン
	// 1. 接続されているモニターの実際の幅と高さを取得
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);
	// 2. モニター解像度に合わせたサイズでNoviceを初期化
	Novice::Initialize(kWindowTitle, screenWidth, screenHeight);
	// 3. ウィンドウ枠を消して画面全体（フルスクリーン）に配置
	HWND hwnd = GetActiveWindow();
	SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE); // ウィンドウ枠を削除
	SetWindowPos(hwnd, HWND_TOP, 0, 0, screenWidth, screenHeight, SWP_FRAMECHANGED | SWP_SHOWWINDOW);
#pragma endregion
#pragma region 変数の宣言
	// プレイヤー
	Player player = {
		{ 100.0f, 0.0f }, // 座標
		{ 0.0f, 0.0f },   // 速度
		true              // 接地フラグ
	};
	// カメラ
	Camera camera = {
		{ 0.0f, 0.0f }    // カメラの初期座標
	};
	// 移動・重力パラメータ
	const float kNormalSpeed = 12.0f;      // 通常速度
	const float kBoostSpeed = 25.0f;       // スペース長押し時の加速速度
	const float kGravity = 1.2f;           // 重力加速度
	const float kGroundY = 0.0f;           // 地面の高さ
	// ★入力状態管理変数（ここで定義）
	KeyInputState inputState = { 0, 0, 0 };
	// ★ジャンプパラメータ（ここで定義）
	JumpParams jumpParams = {
		16.0f, // baseJumpPower
		6.0f,  // jumpAddPower
		40.0f, // maxJumpPower
		12,    // holdThreshold
		10     // tapIntervalMax
	};
#pragma endregion
	int bg = Novice::LoadTexture("./background_tentative.png");
	const int kBgWidth = 1920; // 画像の横幅（解像度に合わせて数値を変更してください
	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };
	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0) {
		// フレームの開始
		Novice::BeginFrame();
		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);
		///
		/// ↓更新処理ここから
		///
		const int targetKey = DIK_SPACE;
		// --- 連打・長押しの判定ロジック ---
		bool isDash = false;
		float jumpPowerToApply = 0.0f; // 今回発動するジャンプ力
		UpdateKeyActionInput(keys, preKeys, targetKey, player, inputState, jumpParams, isDash, jumpPowerToApply);
		// --- 移動・ジャンプ処理 ---
		PlayerMoveJamp(player, isDash, jumpPowerToApply, kNormalSpeed, kBoostSpeed, kGravity, kGroundY);
		// --- カメラ追従処理 ---
		CameraMove(camera.pos.x, player.pos.x, screenWidth);
		///
		/// ↑更新処理ここまで
		///
		///
		/// ↓描画処理ここから
		///
#pragma region 背景・地面の描画（ワールドスクロール）
			// 背景画像のループスクロール処理
			// カメラ座標を元にスクロールオフセット（0〜kBgWidth）を計算
		int camX = static_cast<int>(camera.pos.x);
		int bgOffsetX = (camX % kBgWidth + kBgWidth % kBgWidth);
		// 画面を埋めるために2枚並べて描画
		Novice::DrawSprite(-bgOffsetX, 0, bg, 1.0f, 1.0f, 0.0f, 0xffffffff);
		Novice::DrawSprite(-bgOffsetX + kBgWidth, 0, bg, 1.0f, 1.0f, 0.0f, 0xffffffff);
		// 地面ラインの描画（Y = 0 の位置）
		int screenGroundY = static_cast<int>(kGroundY * -1 + 700);
		Novice::DrawLine(0, screenGroundY + 100, screenWidth, screenGroundY + 100, 0x888888ff);
#pragma endregion
#pragma region プレイヤーの描画
		// ワールド座標からカメラ位置を引いてスクリーン座標を計算
		int playerScreenX = static_cast<int>(player.pos.x - camera.pos.x);
		int playerScreenY = static_cast<int>(player.pos.y * -1 + 700);
		// プレイヤーの色を状態に応じて決定
		unsigned int playerColor = 0xffffffff; // デフォルト：白
		if (isDash) {
			playerColor = 0xff0000ff; // 加速（長押し）中：赤
		}
		else if (inputState.tapCount > 0) {
			playerColor = 0x00ff00ff; // 連打溜め中：緑
		}
		Novice::DrawBox(
			playerScreenX,
			playerScreenY,
			100, 100,
			0.0f,
			playerColor,
			kFillModeSolid
		);
#pragma endregion
		///
		/// ↑描画処理ここまで
		///
		// フレームの終了
		Novice::EndFrame();
		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
			break;
		}
	}
	// ライブラリの終了
	Novice::Finalize();
	return 0;
}