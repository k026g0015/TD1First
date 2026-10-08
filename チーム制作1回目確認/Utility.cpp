#include <Novice.h>
#include <Windows.h>
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include "Utility.h"

#pragma region テストテキスト
void TestText(const char* text) {
	Novice::ScreenPrintf(
		540,
		960,
		"%s", text
	);
}
#pragma endregion