#include <Novice.h>
#include <Windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "player.h"
#pragma region カメラ移動
void CameraMove(float& cameraX, float playerX, int screenWidth) {
	// プレイヤーが画面の横中央（画面幅の半分）に来るようにカメラ位置を更新
	cameraX = playerX - static_cast<float>(screenWidth) / 2.0f;
	// カメラの最低X座標を0にして左端へ戻りすぎないように制限
	if (cameraX < 0.0f) {
		cameraX = 0.0f;
	}
}
#pragma endregion
#pragma region プレイヤー移動ジャンプ
void PlayerMoveJamp(Player& player, bool isDash, float jumpPowerToApply, float normalSpeed, float boostSpeed, float gravity, float groundY) {
	// スペース長押し中は加速
	player.velocity.x = isDash ? boostSpeed : normalSpeed;
	player.pos.x += player.velocity.x;
	// 計算されたジャンプ力でジャンプ発動
	if (jumpPowerToApply > 0.0f && player.isGrounded) {
		player.velocity.y = jumpPowerToApply;
		player.isGrounded = false;
	}
	// 物理演算（重力・落下処理）
	if (!player.isGrounded) {
		player.velocity.y -= gravity;
		player.pos.y += player.velocity.y;

		// 着地判定
		//【変更】落下中(velocity.y <= 0.0f)のみ着地判定する
		if (player.velocity.y <= 0.0f && player.pos.y <= groundY) {
			player.pos.y = groundY;
			player.velocity.y = 0.0f;
			player.isGrounded = true;
		}
	}
}
#pragma endregion
#pragma region 連打・長押し処理
void UpdateKeyActionInput(const char* keys, const char* preKeys, int targetKey, const Player& player, KeyInputState& inputState, const JumpParams& jumpParams, bool& isDash, float& jumpPowerToApply
) {
	isDash = false;
	jumpPowerToApply = 0.0f; // 今回発動するジャンプ力を初期化
	//【追加】配列外参照を防ぐガード処理
	if (targetKey < 0 || targetKey >= 256) {
		return;
	}
	// 1. スペースキーを押している間の処理（長押し判定）
	if (keys[targetKey] != 0) {
		inputState.keyHoldFrames++;

		// 長押し中はダッシュ状態
		if (inputState.keyHoldFrames >= jumpParams.holdThreshold) {
			isDash = true;
		}
	}
	// 2. スペースキーを「押した瞬間」（トリガー）
	if (preKeys[targetKey] == 0 && keys[targetKey] != 0) {
		if (player.isGrounded) {
			inputState.tapCount++;
			//【追加】連打数の上限を10回に制限
			if (inputState.tapCount > 10) {
				inputState.tapCount = 10;
			}
		}
		inputState.keyPressInterval = 0; // 入力からの経過時間をリセット
	}
	// 3. スペースキーを「離した瞬間」
	if (preKeys[targetKey] != 0 && keys[targetKey] == 0) {
		inputState.keyHoldFrames = 0; // 長押しカウントリセット
	}
	// 4. スペースキーを離している間の処理（連打確定判定）
	if (keys[targetKey] == 0) {
		inputState.keyPressInterval++;
		// 制限時間キーが押されなかったら連打確定 ＆ ジャンプ発動
		if (inputState.keyPressInterval > jumpParams.tapIntervalMax) {
			// 2回以上連打された場合のみジャンプを発動（単押し tapCount == 1 は無視）
			if (inputState.tapCount >= 2 && player.isGrounded) {
				// 連打数に応じたジャンプ力を計算
				jumpPowerToApply = jumpParams.baseJumpPower + (inputState.tapCount - 1) * jumpParams.jumpAddPower;
				// 上限制限
				if (jumpPowerToApply > jumpParams.maxJumpPower) {
					jumpPowerToApply = jumpParams.maxJumpPower;
				}
			}
			inputState.tapCount = 0; // カウントリセット
		}
	}
}
#pragma endregion