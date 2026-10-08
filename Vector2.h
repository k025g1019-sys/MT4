#pragma once
#include <cmath>

struct Vector2 {
	float x, y;

	// 加算代入
	Vector2& operator+=(const Vector2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}

	// 減算代入
	Vector2& operator-=(const Vector2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}

	// スカラー倍代入
	Vector2& operator*=(float s) {
		x *= s;
		y *= s;
		return *this;
	}
};

// 加算
inline Vector2 operator+(const Vector2& v1, const Vector2& v2) { return {v1.x + v2.x, v1.y + v2.y}; }

// 減算
inline Vector2 operator-(const Vector2& v1, const Vector2& v2) { return {v1.x - v2.x, v1.y - v2.y}; }

// ベクトル * スカラー
inline Vector2 operator*(const Vector2& v, float s) { return {v.x * s, v.y * s}; }

// スカラー * ベクトル
inline Vector2 operator*(float s, const Vector2& v) { return {v.x * s, v.y * s}; }

// 長さ
inline float Length(const Vector2& v) { return std::sqrt(v.x * v.x + v.y * v.y); }
