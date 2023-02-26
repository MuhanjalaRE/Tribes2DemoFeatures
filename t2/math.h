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

        inline void mSinCos(const float angle, float& s, float& c)
        {
            s = sin(angle);
            c = cos(angle);
        }


        class Vector {
        public:
            float x_;
            float y_;
            float z_;

            operator float* () { return (&x_); }
            operator float* () const { return (float*)(&x_); }

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

            operator float* () { return (matrix_); }              ///< Allow people to get at m.
            operator float* () const { return (float*)(matrix_); }  ///< Allow people to get at m.

            inline Vector GetColumn(unsigned int column) {
                return Vector(matrix_[column], matrix_[column + 4], matrix_[column + 8]);
            }

            inline void SetColumn(unsigned int column, t2::math::Vector vector) {
                matrix_[column] = vector.x_;
                matrix_[column + 4] = vector.y_;
                matrix_[column + 8] = vector.z_;
            }

            inline void Identity(float* m)
            {
                *m++ = 1.0f;
                *m++ = 0.0f;
                *m++ = 0.0f;
                *m++ = 0.0f;

                *m++ = 0.0f;
                *m++ = 1.0f;
                *m++ = 0.0f;
                *m++ = 0.0f;

                *m++ = 0.0f;
                *m++ = 0.0f;
                *m++ = 1.0f;
                *m++ = 0.0f;

                *m++ = 0.0f;
                *m++ = 0.0f;
                *m++ = 0.0f;
                *m = 1.0f;
            }

            inline Matrix& Mul(float* a, float* b) {
                float* mresult = *this;

                mresult[0] = a[0] * b[0] + a[1] * b[4] + a[2] * b[8] + a[3] * b[12];
                mresult[1] = a[0] * b[1] + a[1] * b[5] + a[2] * b[9] + a[3] * b[13];
                mresult[2] = a[0] * b[2] + a[1] * b[6] + a[2] * b[10] + a[3] * b[14];
                mresult[3] = a[0] * b[3] + a[1] * b[7] + a[2] * b[11] + a[3] * b[15];

                mresult[4] = a[4] * b[0] + a[5] * b[4] + a[6] * b[8] + a[7] * b[12];
                mresult[5] = a[4] * b[1] + a[5] * b[5] + a[6] * b[9] + a[7] * b[13];
                mresult[6] = a[4] * b[2] + a[5] * b[6] + a[6] * b[10] + a[7] * b[14];
                mresult[7] = a[4] * b[3] + a[5] * b[7] + a[6] * b[11] + a[7] * b[15];

                mresult[8] = a[8] * b[0] + a[9] * b[4] + a[10] * b[8] + a[11] * b[12];
                mresult[9] = a[8] * b[1] + a[9] * b[5] + a[10] * b[9] + a[11] * b[13];
                mresult[10] = a[8] * b[2] + a[9] * b[6] + a[10] * b[10] + a[11] * b[14];
                mresult[11] = a[8] * b[3] + a[9] * b[7] + a[10] * b[11] + a[11] * b[15];

                mresult[12] = a[12] * b[0] + a[13] * b[4] + a[14] * b[8] + a[15] * b[12];
                mresult[13] = a[12] * b[1] + a[13] * b[5] + a[14] * b[9] + a[15] * b[13];
                mresult[14] = a[12] * b[2] + a[13] * b[6] + a[14] * b[10] + a[15] * b[14];
                mresult[15] = a[12] * b[3] + a[13] * b[7] + a[14] * b[11] + a[15] * b[15];

                return *this;
            }

            inline Matrix& Set(float* e) {

                //Matrix result;

                float* result = *this;

                enum {
                    AXIS_X = (1 << 0),
                    AXIS_Y = (1 << 1),
                    AXIS_Z = (1 << 2)
                };

                unsigned int axis = 0;
                if (e[0] != 0.0f) axis |= AXIS_X;
                if (e[1] != 0.0f) axis |= AXIS_Y;
                if (e[2] != 0.0f) axis |= AXIS_Z;

                switch (axis)
                {
                case 0:

                    Identity(result);
                    break;

                case AXIS_X:
                {
                    float cx, sx;
                    mSinCos(e[0], sx, cx);

                    result[0] = 1.0f;
                    result[1] = 0.0f;
                    result[2] = 0.0f;
                    result[3] = 0.0f;

                    result[4] = 0.0f;
                    result[5] = cx;
                    result[6] = sx;
                    result[7] = 0.0f;

                    result[8] = 0.0f;
                    result[9] = -sx;
                    result[10] = cx;
                    result[11] = 0.0f;

                    result[12] = 0.0f;
                    result[13] = 0.0f;
                    result[14] = 0.0f;
                    result[15] = 1.0f;
                    break;
                }

                case AXIS_Y:
                {
                    float cy, sy;
                    mSinCos(e[1], sy, cy);

                    result[0] = cy;
                    result[1] = 0.0f;
                    result[2] = -sy;
                    result[3] = 0.0f;

                    result[4] = 0.0f;
                    result[5] = 1.0f;
                    result[6] = 0.0f;
                    result[7] = 0.0f;

                    result[8] = sy;
                    result[9] = 0.0f;
                    result[10] = cy;
                    result[11] = 0.0f;

                    result[12] = 0.0f;
                    result[13] = 0.0f;
                    result[14] = 0.0f;
                    result[15] = 1.0f;
                    break;
                }

                case AXIS_Z:
                {
                    // the matrix looks like this:
                    //  r1 - (r4 * sin(x))     r2 + (r3 * sin(x))   -cos(x) * sin(y)
                    //  -cos(x) * sin(z)       cos(x) * cos(z)      sin(x)
                    //  r3 + (r2 * sin(x))     r4 - (r1 * sin(x))   cos(x) * cos(y)
                    //
                    // where:
                    //  r1 = cos(y) * cos(z)
                    //  r2 = cos(y) * sin(z)
                    //  r3 = sin(y) * cos(z)
                    //  r4 = sin(y) * sin(z)
                    float cz, sz;
                    mSinCos(e[2], sz, cz);
                    float r1 = cz;
                    float r2 = sz;
                    float r3 = 0.0f;
                    float r4 = 0.0f;

                    result[0] = cz;
                    result[1] = sz;
                    result[2] = 0.0f;
                    result[3] = 0.0f;

                    result[4] = -sz;
                    result[5] = cz;
                    result[6] = 0.0f;
                    result[7] = 0.0f;

                    result[8] = 0.0f;
                    result[9] = 0.0f;
                    result[10] = 1.0f;
                    result[11] = 0.0f;

                    result[12] = 0.0f;
                    result[13] = 0.0f;
                    result[14] = 0.0f;
                    result[15] = 1.0f;
                    break;
                }

                default:
                    // the matrix looks like this:
                    //  r1 - (r4 * sin(x))     r2 + (r3 * sin(x))   -cos(x) * sin(y)
                    //  -cos(x) * sin(z)       cos(x) * cos(z)      sin(x)
                    //  r3 + (r2 * sin(x))     r4 - (r1 * sin(x))   cos(x) * cos(y)
                    //
                    // where:
                    //  r1 = cos(y) * cos(z)
                    //  r2 = cos(y) * sin(z)
                    //  r3 = sin(y) * cos(z)
                    //  r4 = sin(y) * sin(z)
                    float cx, sx;
                    mSinCos(e[0], sx, cx);
                    float cy, sy;
                    mSinCos(e[1], sy, cy);
                    float cz, sz;
                    mSinCos(e[2], sz, cz);
                    float r1 = cy * cz;
                    float r2 = cy * sz;
                    float r3 = sy * cz;
                    float r4 = sy * sz;

                    result[0] = r1 - (r4 * sx);
                    result[1] = r2 + (r3 * sx);
                    result[2] = -cx * sy;
                    result[3] = 0.0f;

                    result[4] = -cx * sz;
                    result[5] = cx * cz;
                    result[6] = sx;
                    result[7] = 0.0f;

                    result[8] = r3 + (r2 * sx);
                    result[9] = r4 - (r1 * sx);
                    result[10] = cx * cy;
                    result[11] = 0.0f;

                    result[12] = 0.0f;
                    result[13] = 0.0f;
                    result[14] = 0.0f;
                    result[15] = 1.0f;
                    break;
                }

                return *this;
            }
        };
    }
}