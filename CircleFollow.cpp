#include "CircleFollow.h"
#include <Novice.h>
#include <cmath>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region CircleFollow

namespace {
// 円Bの初期座標(画面中央)
const Vector2 kStartPos{640.0f, 360.0f};
} // namespace

CircleFollow::CircleFollow() {
	// 円A(赤・半径12)
	targetPos_ = kStartPos;
	targetRadius_ = 12.0f;
	targetColor_ = RED;

	// 円B(緑・半径20)
	followerPos_ = kStartPos;
	followerRadius_ = 20.0f;
	followerColor_ = GREEN;

	// 比較用(黄色の枠線)
	decayPos_ = kStartPos;
	decayColor_ = 0xFFFF00FF;
}

void CircleFollow::Update() {
	// 毎フレームのマウス座標を取得し、円Aの位置にする
	int mouseX = 0;
	int mouseY = 0;
	Novice::GetMousePosition(&mouseX, &mouseY);
	targetPos_ = {float(mouseX), float(mouseY)};

	// 線形補間で円Bを円Aへ近づける
	// 毎フレーム「残りの距離(target - pos)」の speed * deltaTime 倍だけ進むので、
	// 遠いときは大きく動き、近づくほど動きが小さくなって滑らかに吸い付く
	followerPos_ += (speed_ * deltaTime_) * (targetPos_ - followerPos_);

	// 比較用 : 指数減衰
	// 補間係数を 1 - e^(-speed * deltaTime) にすると、フレームレートが変わっても同じ追従になる
	// speed * deltaTime が小さいうちは線形補間とほぼ同じ値になる
	float t = 1.0f - std::exp(-speed_ * deltaTime_);
	decayPos_ += t * (targetPos_ - decayPos_);
}

void CircleFollow::Draw() {
	// 円Aと円Bの中心を結ぶ線(線の長さ = 追従の遅れ)
	Novice::DrawLine(int(targetPos_.x), int(targetPos_.y), int(followerPos_.x), int(followerPos_.y), WHITE);

	// 比較用の円
	if (showDecay_) {
		Novice::DrawEllipse(int(decayPos_.x), int(decayPos_.y), int(followerRadius_), int(followerRadius_), 0.0f, decayColor_, kFillModeWireFrame);
	}

	// 円B
	Novice::DrawEllipse(int(followerPos_.x), int(followerPos_.y), int(followerRadius_), int(followerRadius_), 0.0f, followerColor_, kFillModeSolid);

	// 円A(カーソル位置が隠れないよう一番手前に描く)
	Novice::DrawEllipse(int(targetPos_.x), int(targetPos_.y), int(targetRadius_), int(targetRadius_), 0.0f, targetColor_, kFillModeSolid);
}

#ifdef _DEBUG
void CircleFollow::DrawImGui() {
	ImGui::Begin("Interpolation Controller");

	ImGui::Text("Target: Mouse Position (Red Circle)");
	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.4f, 1.0f, 0.4f, 1.0f), "[Linear Interpolation - Student Mode]");
	ImGui::SliderFloat("Speed", &speed_, 0.0f, 30.0f, "%.1f");
	ImGui::Text("Mouse Pos: (%.1f, %.1f)", targetPos_.x, targetPos_.y);
	ImGui::Text("Circle Pos: (%.1f, %.1f)", followerPos_.x, followerPos_.y);
	ImGui::Text("Distance to target: %.1f px", Length(targetPos_ - followerPos_));
	ImGui::Separator();

	ImGui::TextColored(ImVec4(1.0f, 0.85f, 0.3f, 1.0f), "[Teacher Comparison Option]");
	ImGui::Checkbox("Show Exponential Decay (Compare)", &showDecay_);

	// 円Aの位置へ一瞬で移動させる
	if (ImGui::Button("Snap to Target")) {
		followerPos_ = targetPos_;
		decayPos_ = targetPos_;
	}
	ImGui::SameLine();
	// 初期座標へ戻す
	if (ImGui::Button("Reset to Center")) {
		followerPos_ = kStartPos;
		decayPos_ = kStartPos;
	}

	ImGui::End();
}
#endif

#pragma endregion
