#include "Scene.h"
#include "DebugGrid.h"
#include <Novice.h>
#include <algorithm>
#ifdef _DEBUG
#include <imgui.h>
#endif

#pragma region SceneManagerB

SceneManager::~SceneManager() {}

void SceneManager::SetScene(std::unique_ptr<Scene> scene) { next = std::move(scene); }

void SceneManager::Update() {
	if (next) {
		current = std::move(next);
	}
	if (current) {
		current->Update(*this);
	}
}

void SceneManager::Draw() {
	if (current) {
		current->Draw();
	}
}

#pragma endregion

#pragma region TitleScene

TitleScene::TitleScene() {
	Vector3 a{0.2f, 1.0f, 0.0f};
	Vector3 b{2.4f, 3.1f, 1.2f};

	c = a + b;
	d = a - b;
	e = a * 2.4f;

	Vector3 rotate{0.4f, 1.43f, -0.8f};

	Matrix4x4 rotateXMatrix = MakeRotateXMatrix(rotate.x);
	Matrix4x4 rotateYMatrix = MakeRotateYMatrix(rotate.y);
	Matrix4x4 rotateZMatrix = MakeRotateZMatrix(rotate.z);

	rotateMatrix = rotateXMatrix * rotateYMatrix * rotateZMatrix;
}

#pragma region Update
void TitleScene::Update(SceneManager& manager) {
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

#ifdef _DEBUG
	ImGui::Begin("Window");
	ImGui::Text("c :%f, %f, %f", c.x, c.y, c.z);
	ImGui::Text("d :%f, %f, %f", d.x, d.y, d.z);
	ImGui::Text("e :%f, %f, %f", e.x, e.y, e.z);
	ImGui::Text(
	    "matrix:\n%f, %f, %f, %f\n%f, %f, %f, %f\n%f, %f, %f,%f\n%f, %f, %f, %f\n", rotateMatrix.m[0][0], rotateMatrix.m[0][1], rotateMatrix.m[0][2], rotateMatrix.m[0][3], rotateMatrix.m[1][0],
	    rotateMatrix.m[1][1], rotateMatrix.m[1][2], rotateMatrix.m[1][3], rotateMatrix.m[2][0], rotateMatrix.m[2][1], rotateMatrix.m[2][2], rotateMatrix.m[2][3], rotateMatrix.m[3][0],
	    rotateMatrix.m[3][1], rotateMatrix.m[3][2], rotateMatrix.m[3][3]);
	ImGui::End();
#endif

	if (keys[DIK_RETURN] && !preKeys[DIK_RETURN]) {
		manager.SetScene(std::make_unique<GameScene>());
	}
}
#pragma endregion

#pragma region Draw
void TitleScene::Draw() {}
#pragma endregion

#pragma endregion

#pragma region GameScene

#pragma region Initialize
GameScene::GameScene() {
	objects = std::make_unique<Objects>();
	viewProjectionMatrix = MakePerspectiveFovMatrix(0.50f, 1280.0f / 720.0f, 0.1f, 2000.0f);
	viewportMatrix = MakeViewportMatrix(0, 0, 1280.0f, 720.0f, 0.0f, 2000.0f);
}
#pragma endregion

#pragma region Update
void GameScene::Update(SceneManager& manager) {

	// キー入力を受け取る
	memcpy(preKeys, keys, 256);
	Novice::GetHitKeyStateAll(keys);

	if (keys[DIK_SPACE] && !preKeys[DIK_SPACE]) {
		manager.SetScene(std::make_unique<TitleScene>());
		return;
	}

#pragma region 入力処理

#pragma endregion

#pragma region 衝突判定

	objects->UpdateAllCollisions();

#pragma endregion

	camera.Update(kWindowWidth, kWindowHeight, viewProjectionMatrix, viewportMatrix, keys);

#pragma region 球面座標

	// 距離0・真上・真下ではカメラの前Fや右Rが決まらないため、計算前にクランプする
	const float limit = std::numbers::pi_v<float> / 2.0f - 0.01f;
	// Windows.hのmaxマクロと衝突しないよう、(std::max)と括弧で囲む
	spherical.radius = (std::max)(spherical.radius, 0.1f);
	spherical.theta = std::clamp(spherical.theta, -limit, limit);

	// 球面座標を直交座標に変換し、注視点に足してカメラ位置を求める
	Vector3 target{0.0f, 0.0f, 0.0f};
	sphericalEye = target + ToCartesian(spherical);
	// 注視点を向くカメラ行列を作る
	sphericalCameraMatrix = MakeLookAtCameraMatrix(sphericalEye, target);

#pragma endregion
}
#pragma endregion

#pragma region Draw
// 描画
void GameScene::Draw() {
	DrawGrid(viewProjectionMatrix, viewportMatrix);
	objects->Draw(viewProjectionMatrix, viewportMatrix);
	
#ifdef _DEBUG
#pragma region ImGui

	objects->DrawImgui();

#pragma region Camera
	ImGui::Begin("Camera");

	Vector3 pos = camera.GetPosition();
	Vector3 rot = camera.GetRotation();

	ImGui::DragFloat3("Position", &pos.x, 0.01f);
	ImGui::DragFloat3("Rotation", &rot.x, 0.01f);

	camera.SetPosition(pos);
	camera.SetRotation(rot);

	ImGui::End();
#pragma endregion

#pragma region Spherical Coordinates
	ImGui::Begin("Spherical Coordinates");

	ImGui::Text("Target: (0, 0, 0) / +Y up / Camera +Z forward");
	ImGui::Separator();

	// 球面座標を編集する(角度はラジアン)
	ImGui::InputFloat("Radius", &spherical.radius, 0.1f, 1.0f, "%.3f");
	ImGui::InputFloat("Theta: elevation (rad)", &spherical.theta, 0.01f, 0.1f, "%.3f");
	ImGui::InputFloat("Phi (rad)", &spherical.phi, 0.01f, 0.1f, "%.3f");

	// 変換結果とカメラ行列はUpdateで計算済みのものを表示する
	ImGui::Separator();
	ImGui::Text("Spherical: r = %.3f, theta = %.3f rad, phi = %.3f rad", spherical.radius, spherical.theta, spherical.phi);
	ImGui::Text("Cartesian: x = %.3f, y = %.3f, z = %.3f", sphericalEye.x, sphericalEye.y, sphericalEye.z);
	ImGui::Separator();

	ImGui::Text("Camera matrix");
	for (int row = 0; row < 4; ++row) {
		ImGui::Text(
		    "%8.3f  %8.3f  %8.3f  %8.3f", sphericalCameraMatrix.m[row][0], sphericalCameraMatrix.m[row][1], sphericalCameraMatrix.m[row][2], sphericalCameraMatrix.m[row][3]);
	}

	ImGui::End();
#pragma endregion

#endif

#pragma endregion
}

#pragma endregion

#pragma endregion

#pragma region FollowScene

void FollowScene::Update(SceneManager&) { circleFollow.Update(); }

void FollowScene::Draw() {
	circleFollow.Draw();

#ifdef _DEBUG
	circleFollow.DrawImGui();
#endif
}

#pragma endregion
