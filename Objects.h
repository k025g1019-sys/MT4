#pragma once
#include <vector>

struct Matrix4x4;
class Sphere;
class Plane;
class Segment;
class Triangle;
class AABB;
class OBB;
class Curve;
class Hierarchy;
class Spring;
class CircularMotion;
class Pendulum;
class ConicalPendulum;

class Objects {
public:
	Objects();
	// メンバのvectorの要素型はここでは前方宣言だけなので、解放処理は型が揃っているObjects.cppで定義する
	~Objects();
	void UpdateAllCollisions();
	void Draw(const Matrix4x4& viewProjectionMatrix, const Matrix4x4& viewportMatrix);
#ifdef _DEBUG
	void DrawImgui();
#endif

private:
	std::vector<Sphere> spheres;
	std::vector<Plane> planes;
	std::vector<Segment> segments;
	std::vector<Triangle> triangles;
	std::vector<AABB> aabbs;
	std::vector<OBB> obbs;
	std::vector<Curve> curves;
	std::vector<Hierarchy> hierarchies;
	std::vector<Spring> springs;
	std::vector<CircularMotion> circularMotions;
	std::vector<Pendulum> pendulums;
	std::vector<ConicalPendulum> conicalPendulums;
};