#include "Objects.h"
#include "AABB.h"
#include "Collision.h"
#include "Matrix4x4.h"
#include "OBB.h"
#include "Plane.h"
#include "Segment.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Curve.h"
#include "Hierarchy.h"
#include "Spring.h"
#include "CircularMotion.h"
#include "Pendulum.h"
#include "ConicalPendulum.h"
#ifdef _DEBUG
#include <imgui.h>
#endif

Objects::Objects() {
	spheres = {
	    // Sphere({0.0f, 0.0f, 0.0f}, 0.5f),
	    // Sphere({0.8f, 0.0f, 1.0f}, 0.4f),

	    // 重力を有効にした球(平面に落として反射する)
	    // 引数: 位置, 半径, 色, 重力を有効にするかのフラグ, 質量, 反発係数e
	    Sphere({0.8f, 1.2f, 0.3f}, 0.05f, 0xFFFFFFFF, true, 2.0f, 0.8f),
	};

	planes = {
	    // Plane({0.0f, 1.0f, 0.0f}, 1.5f),

	    // 法線はSetNormal内で正規化される
	    Plane({-0.2f, 0.9f, -0.3f}, 0.0f),
	};

	segments = {
	    // Segment({-0.8f, 0.3f, 0.0f}, {0.5f, 0.5f, 0.5f}),
	};

	triangles = {
	    // Triangle(Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f)),
	};

	aabbs = {
	    // AABB({-0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}),
	    // AABB({0.2f, 0.2f, 0.2f}, {1.0f, 1.0f, 1.0f}),
	};

	obbs = {
	    //OBB({0.0f, 0.0f, 0.0f},
        //{0.0f, 0.0f, 0.0f},
        //{{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}},
        //{0.83f, 0.26f, 0.24f}
        //),
	    //OBB({0.9f, 0.66f, 0.78f},
        //{-0.05f, -2.49f, 0.15f},
        //{{{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}}},
        //{0.5f, 0.37f, 0.5f}
        //),
	};

	curves = {
	    //Curve({{{-0.8f, 0.58f, 1.0f}, {1.76f, 1.0f, -0.3f}, {0.94f, -0.7f, 2.3f}}},
        //0x0000FFFF),
	    //Curve({{{0.8f, -0.58f, -1.0f}, {-1.76f, -1.0f, 0.3f}, {-0.94f, 0.7f, -2.3f}}},
        //0xFFFF00FF),
	};

	hierarchies = {
		//Hierarchy()
	};

	springs = {
	    // Spring(),
	};

	circularMotions = {
	    // CircularMotion(),
	};

	pendulums = {
	    // Pendulum(),
	};

	conicalPendulums = {
	    // ConicalPendulum(),
	};
}

Objects::~Objects() = default;

#pragma region Color
namespace {

constexpr uint32_t kHitColor = 0xFF0000FF;
constexpr uint32_t kNormalColor = 0xFFFFFFFF;

} // namespace
#pragma endregion

#pragma region Collisions

void Objects::UpdateAllCollisions() {
	for (auto& sphere : spheres) {
		sphere.Update();
		sphere.SetHit(false);
	}
	for (auto& aabb : aabbs) {
		aabb.Update();
		aabb.SetHit(false);
	}
	for (auto& obb : obbs) {
		obb.Update();
		obb.SetHit(false);
	}
	for (auto& segment : segments) {
		segment.SetHit(false);
	}
	for (auto& hierarchy : hierarchies) {
		hierarchy.Update();
	}
	for (auto& spring : springs) {
		spring.Update();
	}
	for (auto& circularMotion : circularMotions) {
		circularMotion.Update();
	}
	for (auto& pendulum : pendulums) {
		pendulum.Update();
	}
	for (auto& conicalPendulum : conicalPendulums) {
		conicalPendulum.Update();
	}

	///
	/// 衝突判定
	///
#pragma region Sphere Sphere
	// 球と球
	for (size_t i = 0; i < spheres.size(); ++i) {
		for (size_t j = i + 1; j < spheres.size(); ++j) {
			if (IsSphereSphereCollision(spheres[i], spheres[j])) {
				spheres[i].SetHit(true);
				spheres[j].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region Sphere Plane
	// 球と平面
	for (size_t i = 0; i < spheres.size(); ++i) {
		for (size_t j = 0; j < planes.size(); ++j) {
			if (spheres[i].IsGravityEnabled()) {
				// 重力有効時は1フレームの移動区間をスイープしたカプセルで判定し、トンネリングを防ぐ
				// 接地中は毎フレーム衝突扱いになるため、ヒット色は変えずに反射処理だけ行う
				if (IsCapsulePlaneCollision(spheres[i].GetPrevCenter(), spheres[i].GetCenter(), spheres[i].GetRadius(), planes[j])) {
					spheres[i].BounceOffPlane(planes[j]);
				}
			} else if (IsSpherePlaneCollision(spheres[i], planes[j])) {
				spheres[i].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region Segment Plane
	// 線と平面
	for (size_t i = 0; i < segments.size(); ++i) {
		for (size_t j = 0; j < planes.size(); ++j) {
			if (IsSegmentPlaneCollision(segments[i], planes[j])) {
				segments[i].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region Segment Triangle
	// 線と三角形
	for (size_t i = 0; i < segments.size(); ++i) {
		for (size_t j = 0; j < triangles.size(); ++j) {
			if (IsTriangleSegmentCollision(triangles[j], segments[i])) {
				segments[i].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region AABBs
	// AABB同士
	for (size_t i = 0; i < aabbs.size(); ++i) {
		for (size_t j = i + 1; j < aabbs.size(); ++j) {
			if (IsAABBCollision(aabbs[i].GetWorldAABB(), aabbs[j].GetWorldAABB())) {
				aabbs[i].SetHit(true);
				aabbs[j].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region AABB Sphere
	// AABBと球
	for (size_t i = 0; i < aabbs.size(); ++i) {
		AABB world = aabbs[i].GetWorldAABB();
		for (size_t j = 0; j < spheres.size(); ++j) {
			if (IsAABBSphereCollision(world, spheres[j])) {
				spheres[j].SetHit(true);
				aabbs[i].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region AABB Segment
	// AABBと線
	for (size_t i = 0; i < aabbs.size(); ++i) {
		AABB world = aabbs[i].GetWorldAABB();
		for (size_t j = 0; j < segments.size(); ++j) {
			if (IsAABBSegmentCollision(world, segments[j])) {
				aabbs[i].SetHit(true);
				segments[j].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region OBB Sphere
	// OBBと球
	for (size_t i = 0; i < obbs.size(); ++i) {
		for (size_t j = 0; j < spheres.size(); ++j) {
			if (IsOBBSphereCollision(obbs[i], spheres[j])) {
				obbs[i].SetHit(true);
				spheres[j].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region OBB Segment
	// OBBと線
	for (size_t i = 0; i < obbs.size(); ++i) {
		for (size_t j = 0; j < segments.size(); ++j) {
			if (IsOBBSegmentCollision(obbs[i], segments[j])) {
				obbs[i].SetHit(true);
				segments[j].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region OBBs
	// OBB同士
	for (size_t i = 0; i < obbs.size(); ++i) {
		for (size_t j = i + 1; j < obbs.size(); ++j) {
			if (IsOBBCollision(obbs[i], obbs[j])) {
				obbs[i].SetHit(true);
				obbs[j].SetHit(true);
			}
		}
	}
#pragma endregion
#pragma region OBB AABB
	// OBBとAABB
	for (size_t i = 0; i < obbs.size(); ++i) {
		for (size_t j = 0; j < aabbs.size(); ++j) {
			AABB world = aabbs[j].GetWorldAABB();
			if (IsOBBAABBCollision(obbs[i], world)) {
				obbs[i].SetHit(true);
				aabbs[j].SetHit(true);
			}
		}
	}
#pragma endregion

	// 最終的に当たっていたかで色を決定する
	for (auto& sphere : spheres) {
		sphere.SetColor(sphere.IsHit() ? kHitColor : kNormalColor);
	}

	for (auto& aabb : aabbs) {
		aabb.SetColor(aabb.IsHit() ? kHitColor : kNormalColor);
	}

	for (auto& obb : obbs) {
		obb.SetColor(obb.IsHit() ? kHitColor : kNormalColor);
	}

	for (auto& segment : segments) {
		segment.SetColor(segment.IsHit() ? kHitColor : kNormalColor);
	}
}

#pragma endregion

#pragma region Draw
void Objects::Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix) {

	auto drawList = [&](auto& container) {
		for (auto& obj : container) {
			obj.Draw(viewProjectionMatrix, viewportMatrix);
		}
	};

	drawList(spheres);
	drawList(planes);
	drawList(segments);
	drawList(triangles);
	drawList(aabbs);
	drawList(obbs);
	drawList(curves);
	drawList(hierarchies);
	drawList(springs);
	drawList(circularMotions);
	drawList(pendulums);
	drawList(conicalPendulums);
}
#pragma endregion

#ifdef _DEBUG

#pragma region ImGui

#pragma region ImGui Helpers

namespace {

template<class T> void DrawObjectTree(const char* treeName, std::vector<T>& objects, const char* labelName) {

	if (ImGui::TreeNode(treeName)) {

		for (size_t i = 0; i < objects.size(); ++i) {

			ImGui::PushID(static_cast<int>(i));

			ImGui::Text("%s[%zu]", labelName, i);

			objects[i].DrawImGui();

			ImGui::PopID();
		}

		ImGui::TreePop();
	}
}

} // namespace

#pragma endregion

void Objects::DrawImgui() {

	ImGui::Begin("window");

	DrawObjectTree("Spheres", spheres, "Sphere");
	DrawObjectTree("Planes", planes, "Plane");
	DrawObjectTree("Segments", segments, "Segment");
	DrawObjectTree("Triangles", triangles, "Triangle");
	DrawObjectTree("AABBs", aabbs, "AABB");
	DrawObjectTree("OBBs", obbs, "OBB");
	DrawObjectTree("Curves", curves, "Curve");
	DrawObjectTree("Hierarchies", hierarchies, "Hierarchy");
	DrawObjectTree("Springs", springs, "Spring");
	DrawObjectTree("CircularMotions", circularMotions, "CircularMotion");
	DrawObjectTree("Pendulums", pendulums, "Pendulum");
	DrawObjectTree("ConicalPendulums", conicalPendulums, "ConicalPendulum");

	ImGui::End();
}

#pragma endregion

#endif