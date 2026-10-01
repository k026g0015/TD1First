#include <Novice.h>
#include <Windows.h>

const char kWindowTitle[] = "チーム制作1回目1129班";

#pragma region 構造体
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

// カメラ
struct Camera {
	Vector2 pos;     // 座標 (ワールド座標)
};
#pragma endregion

// Windowsアプリでのエントリーポイント(main関数)
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

	// 移動・ジャンプパラメータ
	const float kNormalSpeed = 12.0f;      // 通常速度
	const float kBoostSpeed = 25.0f;       // スペース長押し時の加速速度
	const float kBaseJumpPower = 16.0f;    // 基本ジャンプ力
	const float kJumpAddPower = 6.0f;      // 連打1回あたりの追加ジャンプ力
	const float kMaxJumpPower = 40.0f;     // 最大ジャンプ力の上限
	const float kGravity = 1.2f;           // 重力加速度
	const float kGroundY = 0.0f;           // 地面の高さ

	// 入力判定用変数（スペースキー用）
	int keyHoldFrames = 0;           // キーを押し続けているフレーム数
	int keyPressInterval = 0;        // キーを離してからのカウントフレーム数
	int tapCount = 0;                // 連続タップ回数

	// 閾値の設定（60fps想定）
	const int kHoldThreshold = 12;   // 長押しとみなすフレーム数（約0.20秒）
	const int kTapIntervalMax = 10;  // 連打とみなす許容間隔フレーム数（約0.16秒）
#pragma endregion

	int bg = Novice::LoadTexture("./background_tentative.png");
	const int kBgWidth = 1920; // 画像の横幅（解像度に合わせて数値を変更してください）

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

		// 1. スペースキーを押している間の処理（長押し判定）
		if (keys[targetKey] != 0) {
			keyHoldFrames++;

			// 長押し中はダッシュ状態
			if (keyHoldFrames >= kHoldThreshold) {
				isDash = true;
			}
		}

		// 2. スペースキーを「押した瞬間」（トリガー）
		if (preKeys[targetKey] == 0 && keys[targetKey] != 0) {
			if (player.isGrounded) {
				tapCount++;
			}
			keyPressInterval = 0; // 入力からの経過時間をリセット
		}

		// 3. スペースキーを「離した瞬間」
		if (preKeys[targetKey] != 0 && keys[targetKey] == 0) {
			keyHoldFrames = 0; // 長押しカウントリセット
		}

		// 4. スペースキーを離している間の処理（連打確定判定）
		if (keys[targetKey] == 0) {
			keyPressInterval++;

			// 制限時間キーが押されなかったら連打確定 ＆ ジャンプ発動
			if (keyPressInterval > kTapIntervalMax) {
				// 2回以上連打された場合のみジャンプを発動（単押し tapCount == 1 は無視）
				if (tapCount >= 2 && player.isGrounded) {
					// 連打数に応じたジャンプ力を計算
					jumpPowerToApply = kBaseJumpPower + (tapCount - 1) * kJumpAddPower;

					// 上限制限
					if (jumpPowerToApply > kMaxJumpPower) {
						jumpPowerToApply = kMaxJumpPower;
					}
				}
				tapCount = 0; // カウントリセット
			}
		}


		// --- 移動・ジャンプ処理 ---

		// スペース長押し中は加速
		player.velocity.x = isDash ? kBoostSpeed : kNormalSpeed;
		player.pos.x += player.velocity.x;

		// 計算されたジャンプ力でジャンプ発動
		if (jumpPowerToApply > 0.0f && player.isGrounded) {
			player.velocity.y = jumpPowerToApply;
			player.isGrounded = false;
		}

		// 物理演算（重力・落下処理）
		if (!player.isGrounded) {
			player.velocity.y -= kGravity;
			player.pos.y += player.velocity.y;

			// 着地判定
			if (player.pos.y <= kGroundY) {
				player.pos.y = kGroundY;
				player.velocity.y = 0.0f;
				player.isGrounded = true;
			}
		}


		// --- カメラ追従処理 ---

		// プレイヤーが画面の横中央（画面幅の半分）に来るようにカメラ位置を更新
		camera.pos.x = player.pos.x - static_cast<float>(screenWidth) / 2.0f;

		// カメラの最低X座標を0にして左端へ戻りすぎないように制限
		if (camera.pos.x < 0.0f) {
			camera.pos.x = 0.0f;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///
#pragma region 背景・地面の描画（ワールドスクロール）
			// 背景画像のループスクロール処理
			// カメラ座標を元にスクロールオフセット（0〜kBgWidth）を計算
			int bgOffsetX = static_cast<int>(camera.pos.x) % kBgWidth;

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
		else if (tapCount > 0) {
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