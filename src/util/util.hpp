
#pragma once

#include <iostream>

#include <cmath>
#include <cassert>
#include <vector>
#include <set>
#include <string>

// 'additions' to STL

template <typename T>
std::vector<T> operator+(const std::vector<T>& a, const std::vector<T>& b) {
	std::vector<T> total(a);
	total.insert(total.end(), b.begin(), b.end());
	return total;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& self, const std::vector<T>& other) {
	self.insert(self.end(), other.begin(), other.end());
	return self;
}

template <typename T>
std::set<T>& operator+=(std::set<T>& self, const std::vector<T>& other) {
	self.insert(other.begin(), other.end());
	return self;
}

std::vector<std::string> split(std::string& s, char delim);


namespace si {
namespace util {

template <typename T>
class Vector2D {
public:
	Vector2D() = default;
	
	Vector2D(T _x, T _y): x(_x), y(_y) {}
	
	Vector2D& operator+=(const Vector2D& other) {
		x += other.x;
		y += other.y;
		return *this;
	}
	
	Vector2D& operator-=(const Vector2D& other) {
		x -= other.x;
		y -= other.y;
		return *this;
	}
	
	
	Vector2D& operator*=(const Vector2D& other) {
		x *= other.y;
		y *= other.y;
		return *this;
	}
	
	Vector2D& operator/=(const Vector2D& other) {
		assert(other.x != 0 and other.y != 0);
		x /= other.x;
		y /= other.y;
		return *this;
	}
	
	template <typename ScalarT>
	Vector2D& operator*=(const ScalarT& s) {
		x *= s;
		y *= s;
		return *this;
	}
	
	template <typename ScalarT>
	Vector2D& operator/=(const ScalarT& s) {
		assert(s != 0);
		x /= s;
		y /= s;
		return *this;
	}
	
	
	Vector2D& normalize() {
		T len = length();
		if (len != 0 and len != 1) {
			x /= len;
			y /= len;
		}
		return *this;
	}
	
	T length() {
		return sqrt(x*x + y*y);
	}
	
	T x = 0;
	T y = 0;
};

template <typename T>
std::ostream& operator<<(std::ostream& out, const Vector2D<T>& v) {
	out << "(" << v.x << ", " << v.y << ")";
	return out;
}

template <typename T>
Vector2D<T> operator+(const Vector2D<T>& a, const Vector2D<T>& b) {
	return Vector2D<T>(a.x + b.x, a.y + b.y);
}

template <typename T>
Vector2D<T> operator-(const Vector2D<T>& a, const Vector2D<T>& b) {
	return Vector2D<T>(a.x - b.x, a.y - b.y);
}

template <typename T, typename ScalarT>
Vector2D<T> operator*(const Vector2D<T>& v, ScalarT s) {
	return Vector2D<T>(v.x*s, v.y*s);
}

template <typename T, typename ScalarT>
Vector2D<T> operator*(ScalarT s, const Vector2D<T>& v) {
	return Vector2D<T>(v.x*s, v.y*s);
}

template <typename T, typename ScalarT>
Vector2D<T> operator/(const Vector2D<T>& v, ScalarT s) {
	assert(s != 0);
	return Vector2D<T>(v.x/s, v.y/s);
}

template <typename T, typename ScalarT>
Vector2D<T> operator/(ScalarT s, const Vector2D<T>& v) {
	assert(s != 0);
	return Vector2D<T>(v.x/s, v.y/s);
}

template <typename T>
Vector2D<T> operator*(const Vector2D<T>& a, const Vector2D<T>& b) {
	return Vector2D<T>(a.x * b.x, a.y * b.y);
}

template <typename T>
Vector2D<T> operator/(const Vector2D<T>& a, const Vector2D<T>& b) {
	assert(b.x != 0 and b.y != 0);
	return Vector2D<T>(a.x / b.x, a.y / b.y);
}


using Vector2D_d = Vector2D<double>;

const Vector2D_d NORTH = Vector2D_d(0, 1);
const Vector2D_d EAST = Vector2D_d(1, 0);
const Vector2D_d SOUTH = Vector2D_d(0, -1);
const Vector2D_d WEST = Vector2D_d(-1, 0);
const Vector2D_d HOLD = Vector2D_d(0, 0);

}
}
