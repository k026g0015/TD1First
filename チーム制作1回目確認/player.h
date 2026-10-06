#pragma once

#pragma region 構造体

struct Vector2 {
	float x;
	float y;
};

struct Size {
	float width;
	float height;
};

struct Player {
	Vector2 pos;
	Vector2 velocity;
	bool isGrounded;

	float moveRemaining;
	bool isMoving;
};

struct Camera {
	Vector2 pos;
};

struct KeyInputState {
	int keyHoldFrames;
	int keyPressInterval;
	int tapCount;
};

struct JumpParams {
	float baseJumpPower;
	float jumpAddPower;
	float maxJumpPower;
	int holdThreshold;
	int tapIntervalMax;
};

#pragma endregion


#pragma region 関数宣言

void CameraMove(
	float& cameraX,
	float playerX,
	int screenWidth
);

void PlayerMoveJamp(
	Player& p,
	bool isDash,
	float jumpPowerToApply,
	float moveDistance,
	//float normalSpeed,
	float boostSpeed,
	float gravity,
	float groundY
);

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
);

#pragma endregion


#pragma region 外部変数

extern Player player;
extern Camera camera;

extern KeyInputState inputState;
extern JumpParams jumpParams;

extern const float kNormalSpeed;
extern const float kBoostSpeed;
extern const float kGravity;
extern const float kGroundY;

#pragma endregion