#include <Novice.h>
#include <Windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "player.h"

// プレイヤー
Player player = {
	{ 100.0f, 0.0f },
	{ 0.0f, 0.0f },
	true,
	0.0f,
	false
};

// カメラ
Camera camera = {
	{ 0.0f, 0.0f }
};

// 移動・重力パラメータ
const float kNormalSpeed = 0.0f;
const float kBoostSpeed = 30.0f;
const float kGravity = 1.2f;
const float kGroundY = 0.0f;

// 入力状態
KeyInputState inputState = {
	0, 0, 0
};

// ジャンプパラメータ
JumpParams jumpParams = {
	16.0f,
	6.0f,
	40.0f,
	12,
	10
};


#pragma region カメラ移動

void CameraMove(float& cameraX, float playerX, int screenWidth) {

	cameraX = playerX - static_cast<float>(screenWidth) / 2.0f;

	if (cameraX < 0.0f) {
		cameraX = 0.0f;
	}
}

#pragma endregion


#pragma region プレイヤー移動ジャンプ

void PlayerMoveJamp(
	Player& p,
	bool isDash,
	float jumpPowerToApply,
	float moveDistance,
	float boostSpeed,
	float gravity,
	float groundY
) {

	// スペース長押し
	if (isDash) {
		p.velocity.x = boostSpeed;
		p.pos.x += p.velocity.x;
	}

	// スペース1回押し
	// 単押しで120px移動開始
	if (moveDistance > 0.0f && !p.isMoving) {
		p.moveRemaining = 120.0f;
		p.isMoving = true;
	}

	// 120px歩く
	if (p.isMoving) {

		float moveSpeed = 12.0f;

		if (p.moveRemaining < moveSpeed) {
			moveSpeed = p.moveRemaining;
		}

		p.pos.x += moveSpeed;
		p.moveRemaining -= moveSpeed;

		// 120px移動したら停止
		if (p.moveRemaining <= 0.0f) {
			p.moveRemaining = 0.0f;
			p.isMoving = false;
		}
	}

	// ジャンプ
	if (jumpPowerToApply > 0.0f && p.isGrounded) {
		p.velocity.y = jumpPowerToApply;
		p.isGrounded = false;
	}

	// 重力
	if (!p.isGrounded) {
		p.velocity.y -= gravity;
		p.pos.y += p.velocity.y;

		if (p.velocity.y <= 0.0f && p.pos.y <= groundY) {
			p.pos.y = groundY;
			p.velocity.y = 0.0f;
			p.isGrounded = true;
		}
	}
}

#pragma endregion


#pragma region 連打・長押し処理

void UpdateKeyActionInput(
	const char* keys,
	const char* preKeys,
	int targetKey,
	const Player& p,
	KeyInputState& state,
	const JumpParams& params,
	bool& isDash,
	float& jumpPowerToApply,
	float& moveDistance
) {

	isDash = false;
	jumpPowerToApply = 0.0f;
	moveDistance = 0.0f;

	if (targetKey < 0 || targetKey >= 256) {
		return;
	}

	// 長押し
	if (keys[targetKey] != 0) {
		state.keyHoldFrames++;

		if (state.keyHoldFrames >= params.holdThreshold) {
			isDash = true;
		}
	}

	// スペースを押した瞬間
	if (preKeys[targetKey] == 0 && keys[targetKey] != 0) {

		if (p.isGrounded) {
			state.tapCount++;

			if (state.tapCount > 10) {
				state.tapCount = 10;
			}
		}

		state.keyPressInterval = 0;
	}

	// スペースを離した瞬間
	if (preKeys[targetKey] != 0 && keys[targetKey] == 0) {
		state.keyHoldFrames = 0;
	}

	// スペースを押していない
	if (keys[targetKey] == 0) {

		state.keyPressInterval++;

		// 連打判定の時間が終了
		if (state.keyPressInterval > params.tapIntervalMax) {

			// 1回押し → 120移動
			if (state.tapCount == 1 && p.isGrounded) {
				moveDistance = 120.0f;
			}

			// 2回以上 → ジャンプ
			else if (state.tapCount >= 2 && p.isGrounded) {

				jumpPowerToApply =
					params.baseJumpPower +
					(state.tapCount - 1) * params.jumpAddPower;

				if (jumpPowerToApply > params.maxJumpPower) {
					jumpPowerToApply = params.maxJumpPower;
				}
			}

			state.tapCount = 0;
		}
	}
}
#pragma endregion