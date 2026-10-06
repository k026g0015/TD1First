#include <Novice.h>
#include <Windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>

#pragma region テストテキスト
void TestText(char text) {
	Novice::ScreenPrintf(
		540,
		960,
		"%S", text
	);
}
#pragma endregion