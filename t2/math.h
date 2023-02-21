#pragma once

#include <cmath>

#define M_PI 3.14159265358979323846
#define M_2PI (2*M_PI)
#define M_PI_F ((float)(M_PI))
#define PI M_PI
#define DEG2RAD(x) ((float)(x) * (float)(M_PI_F / 180.f))
#define RAD2DEG(x) ((float)(x) * (float)(180.f / M_PI_F))
#define INV_PI (0.31830988618f)
#define HALF_PI (1.57079632679f)

namespace t2 {
    namespace math {
        class Vector {
        public:
            float x_;
            float y_;
            float z_;

            inline Vector() : x_(0), y_(0), z_(0) {}

            inline Vector(float x_, float y, float z) : x_(x_), y_(y), z_(z) {}

            inline Vector operator+(const Vector& other) const { return Vector(x_ + other.x_, y_ + other.y_, z_ + other.z_); }

            inline Vector operator-(const Vector& other) const { return Vector(x_ - other.x_, y_ - other.y_, z_ - other.z_); }

            inline Vector operator*(float scalar) const { return Vector(x_ * scalar, y_ * scalar, z_ * scalar); }

            inline Vector operator*(const Vector& other) const { return Vector(x_ * other.x_, y_ * other.y_, z_ * other.z_); }

            inline Vector operator/(float scalar) const { return Vector(x_ / scalar, y_ / scalar, z_ / scalar); }

            inline Vector operator/(const Vector& other) const { return Vector(x_ / other.x_, y_ / other.y_, z_ / other.z_); }

            inline Vector& operator=(const Vector& other) {
                x_ = other.x_;
                y_ = other.y_;
                z_ = other.z_;
                return *this;
            }

            inline Vector& operator+=(const Vector& other) {
                x_ += other.x_;
                y_ += other.y_;
                z_ += other.z_;
                return *this;
            }

            inline Vector& operator-=(const Vector& other) {
                x_ -= other.x_;
                y_ -= other.y_;
                z_ -= other.z_;
                return *this;
            }

            inline Vector& operator*=(const float other) {
                x_ *= other;
                y_ *= other;
                z_ *= other;
                return *this;
            }

            inline float Dot(const Vector& b) const { return (x_ * b.x_) + (y_ * b.y_) + (z_ * b.z_); }

            inline float MagnitudeSqr() const { return Dot(*this); }

            inline float Magnitude() const { return std::sqrtf(MagnitudeSqr()); }

            inline Vector Unit() const {
                const float fMagnitude = Magnitude();
                return Vector(x_ / fMagnitude, y_ / fMagnitude, z_ / fMagnitude);
            }

            friend bool operator==(const Vector& first, const Vector& second) { return first.x_ == second.x_ && first.y_ == second.y_ && first.z_ == second.z_; }

            friend bool operator!=(const Vector& first, const Vector& second) { return !(first == second); }

        };

        typedef Vector Rotation;

        class Matrix
        {
        public:
            float matrix_[16];
            inline Vector GetColumn(unsigned int column) {
                return Vector(matrix_[column], matrix_[column + 4], matrix_[column + 8]);
            }

            inline void SetColumn(unsigned int column, t2::math::Vector vector) {
                matrix_[column] = vector.x_;
                matrix_[column + 4] = vector.y_;
                matrix_[column + 8] = vector.z_;
            }
        };
    }
}