#pragma once
struct Vector2 {
	float x;
	float y;
};

// プレイヤー
struct Player {
	Vector2 pos;     // 座標 (ワールド座標)
	Vector2 velocity;// 速度 (x: 移動速度, y: ジャンプ速度)
	bool isGrounded; // 接地判定
};

// キー入力判定用の状態変数構造体
struct KeyInputState {
	int keyHoldFrames;      // キーを押し続けているフレーム数
	int keyPressInterval;   // キーを離してからのカウントフレーム数
	int tapCount;           // 連続タップ回数
};

// ジャンプ計算用の固定パラメータ構造体
struct JumpParams {
	float baseJumpPower; // 基本ジャンプ力
	float jumpAddPower;  // 1回あたりの追加ジャンプ力
	float maxJumpPower;  // 最大ジャンプ力
	int holdThreshold;   // 長押し判定フレーム数
	int tapIntervalMax;  // 連打許容フレーム数
};
void CameraMove(float& cameraX, float playerX, int screenWidth);
void PlayerMoveJamp(Player& player, bool isDash, float jumpPowerToApply, float normalSpeed, float boostSpeed, float gravity, float groundY);
// 連打・長押しの判定を行う関数
void UpdateKeyActionInput(const char* keys, const char* preKeys, int targetKey, const Player& player, KeyInputState& inputState, const JumpParams& jumpParams, bool& isDash, float& jumpPowerToApply);