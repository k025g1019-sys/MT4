#pragma once
#include "Vector2.h"

#pragma region CircleFollow

/// <summary>
/// 線形補間による円の追従(追従カメラの考え方を2Dの円で確認する)
/// </summary>
class CircleFollow {
public:
	CircleFollow();

	void Update();
	void Draw();
#ifdef _DEBUG
	void DrawImGui();
#endif

private:
	// 円A : マウスカーソルの位置に表示する(追従先)
	Vector2 targetPos_;
	float targetRadius_;
	unsigned int targetColor_;

	// 円B : 円Aへ線形補間で追従する
	Vector2 followerPos_;
	float followerRadius_;
	unsigned int followerColor_;

	// 比較用 : 指数減衰で追従する円
	Vector2 decayPos_;
	unsigned int decayColor_;
	bool showDecay_ = false;

	// 追従速度(大きいほど速く吸い付く)
	float speed_ = 6.0f;

	// 60fps前提なのでdeltaTimeは1/60秒で固定
	const float deltaTime_ = 1.0f / 60.0f;
};

#pragma endregion
