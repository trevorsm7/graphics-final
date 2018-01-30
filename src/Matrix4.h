#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include <iostream>
#include "Vector3.h"
#include "Vector4.h"

class Matrix4
{
    friend std::ostream& operator<<(std::ostream&, const Matrix4&);

protected:
    float m_mat[4][4];

public:
    Matrix4() {} // do not initialize matrix

    Matrix4& zeros()
    {
        memset(m_mat, 0, sizeof(m_mat));
        return *this;
    }

    // Const functions (result returned in a copy)
    Matrix4 operator*(const Matrix4& m) const;
    Vector4 operator*(const Vector4& v) const;
    Vector3 multiply(const Vector3& v, float w) const;
    Vector4 getRow(int) const;
    Vector4 getCol(int) const;

    // Doesn't modify matrix itself, but allows caller to do so
    float* getPointer() {return &m_mat[0][0];}

    // In-place functions (result stored in this)
    Matrix4& operator*=(const Matrix4& m) {return (*this = *this * m);}
    Matrix4& transpose();

    // Reset matrix to be of specified type (and return reference to self)
    Matrix4& identity();  // create identity matrix
    Matrix4& rotateX(float); // rotation about x axis
    Matrix4& rotateY(float); // rotation about y axis
    Matrix4& rotateZ(float); // rotation about z axis
    Matrix4& rotate(float angle, const Vector3& axis);
    Matrix4& scale(float, float, float);
    Matrix4& scale(float s) {return scale(s, s, s);}
    Matrix4& translate(float, float, float);
    Matrix4& translate(const Vector3& t) {return translate(t.getX(), t.getY(), t.getZ());}
    Matrix4& camera(const Vector3& center, const Vector3& lookAt, const Vector3& up);
    Matrix4& frustum(float l, float r, float b, float t, float n, float f);
    Matrix4& projection(float fovY, float aspect, float near, float far);
    Matrix4& ortho(float l, float r, float b, float t, float n, float f);
    Matrix4& viewport(int width, int height);
    Matrix4& bezier();
};

std::ostream& operator<<(std::ostream&, const Matrix4&);

#endif