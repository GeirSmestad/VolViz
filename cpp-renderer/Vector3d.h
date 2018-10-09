#ifndef __VECTOR3D_H__
#define __VECTOR3D_H__

#include <cassert>
#include <cmath>
#include <ostream>

/// A 3D vector of double values.
class Vector3d
{
    // ********************************************************************************************************
    // *** Basic methods **************************************************************************************
public:
    /// Default constructor
    Vector3d()
    {
        m_vfElements[0] = 0.0;
        m_vfElements[1] = 0.0;
        m_vfElements[2] = 0.0;
    }

    /// Create a vector with the specified values
    Vector3d(const double fX, const double fY, const double fZ)
    {
        m_vfElements[0] = fX;
        m_vfElements[1] = fY;
        m_vfElements[2] = fZ;
    }

    /// Create a vector with the specified values
    Vector3d(const double vfValues[])
    {
        m_vfElements[0] = vfValues[0];
        m_vfElements[1] = vfValues[1];
        m_vfElements[2] = vfValues[2];
    }

    /// Copy constructor
    Vector3d(const Vector3d& other)
    {
        m_vfElements[0] = other.m_vfElements[0];
        m_vfElements[1] = other.m_vfElements[1];
        m_vfElements[2] = other.m_vfElements[2];
    }

    /// Assignment operator
    Vector3d& operator=(const Vector3d& other)
    {
        if(&other != this)
        {
            m_vfElements[0] = other.m_vfElements[0];
            m_vfElements[1] = other.m_vfElements[1];
            m_vfElements[2] = other.m_vfElements[2];
        }
        return *this;
    }

    /// Destructor
    ~Vector3d()
    {
    }


    // ********************************************************************************************************
    // *** Public methods *************************************************************************************
public:
    /// Set the values of the vector
    void Set(const double fX, const double fY, const double fZ)
    {
        m_vfElements[0] = fX;
        m_vfElements[1] = fY;
        m_vfElements[2] = fZ;
    }

    /// Set the first element of the vector
    void SetX(double fX)
    {
        m_vfElements[0] = fX;
    }

    /// Set the second element of the vector
    void SetY(double fY)
    {
        m_vfElements[1] = fY;
    }

    /// Set the third element of the vector
    void SetZ(double fZ)
    {
        m_vfElements[2] = fZ;
    }

    /// Return a const pointer to the elements of the vector
    const double * Get() const
    {
        return m_vfElements;
    }

    /// Return the i-th element of the vector
    double Get(unsigned int i) const
    {
        assert(i < 3);
        return m_vfElements[i];
    }

    /// Return the first element of the vector
    double GetX() const
    {
        return m_vfElements[0];
    }

    /// Return the second element of the vector
    double GetY() const
    {
        return m_vfElements[1];
    }

    /// Return the third element of the vector
    double GetZ() const
    {
        return m_vfElements[2];
    }

    /// Return the magnitude of the vector
    double GetMagnitude() const
    {
        return std::sqrt(GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ());
    }

    /// Return the square of the magnitude of the vector
    double GetSquaredMagnitude() const
    {
        return (GetX()*GetX() + GetY()*GetY() + GetZ()*GetZ());
    }

    /// Return the dot product of this vector with the specified one
    double Dot(const Vector3d &vecOther) const
    {
        return (GetX() * vecOther.GetX() + GetY()*vecOther.GetY() + GetZ()*vecOther.GetZ());
    }

    /// Return the cross product of this vector with the specified one
    Vector3d Cross(const Vector3d &vecOther) const
    {
        return Vector3d(GetY()*vecOther.GetZ() - GetZ()*vecOther.GetY(), GetZ()*vecOther.GetX() - GetX() * vecOther.GetZ(), GetX()*vecOther.GetY() - GetY()*vecOther.GetX());
    }

    /// Return a normalized copy of this vector
    Vector3d GetNormalized() const
    {
        Vector3d vecNew = *this;
        double fMagnitude = vecNew.GetMagnitude();

        if (fMagnitude > 0.0)
            vecNew /= fMagnitude;

        return vecNew;
    }

    /// Normalize this vector
    void normalize()
    {
        (*this) = GetNormalized();
    }

    /// Return the inverse of this vector
    Vector3d GetInverse() const
    {
        return Vector3d(1.0/GetX(),1.0/GetY(),1.0/GetZ());
    }

    /// Invert this vector
    void invert()
    {
        (*this) = GetInverse();
    }

    /// Return the opposite of this vector
    Vector3d GetOpposite() const
    {
        return -(*this); // call operator-
    }

    /// Change this vector in its opposite
    void oppose()
    {
        (*this) = GetOpposite();
    }

    /// Return true if this vector is equal to the specified one
    bool operator==(const Vector3d & vecOther) const
    {
        return (GetX() == vecOther.GetX()) && (GetY() == vecOther.GetY()) && (GetZ() == vecOther.GetZ());
    }

    /// Return true if this vector is different from the specified one
    bool operator!=(const Vector3d & vecOther) const
    {
        return !(*this == vecOther);
    }

    /// Access the i-th element of the vector
    double & operator[](unsigned int i)
    {
        assert(i<3);
        return m_vfElements[i];
    }

    /// Access the i-th element of the vector (const version)
    const double& operator[](unsigned int i) const
    {
        assert(i<3);
        return m_vfElements[i];
    }

    /// In-place vector sum
    const Vector3d & operator+=(const Vector3d & vecOther)
    {
        SetX(GetX()+vecOther.GetX());
        SetY(GetY()+vecOther.GetY());
        SetZ(GetZ()+vecOther.GetZ());
        return *this;
    }

    /// In-place vector difference
    const Vector3d & operator-=(const Vector3d & vecOther)
    {
        SetX(GetX()-vecOther.GetX());
        SetY(GetY()-vecOther.GetY());
        SetZ(GetZ()-vecOther.GetZ());
        return *this;
    }

    /// In-place element by element multiplication
    const Vector3d & operator*=(const Vector3d & vecOther)
    {
        SetX(GetX()*vecOther.GetX());
        SetY(GetY()*vecOther.GetY());
        SetZ(GetZ()*vecOther.GetZ());
        return *this;
    }

    /// In-place scalar multiplication
    const Vector3d & operator*=(const double & fOther)
    {
        SetX(GetX()*fOther);
        SetY(GetY()*fOther);
        SetZ(GetZ()*fOther);
        return *this;
    }

    /// In-place element by element division
    const Vector3d & operator/=(const Vector3d & vecOther)
    {
        assert(vecOther.GetX() != 0.);
        SetX(GetX()/vecOther.GetX());
        assert(vecOther.GetY() != 0.);
        SetY(GetY()/vecOther.GetY());
        assert(vecOther.GetZ() != 0.);
        SetZ(GetZ()/vecOther.GetZ());
        return *this;
    }

     /// In-place scalar division
    const Vector3d & operator/=(const double & fOther)
    {
        assert(fOther != 0.);
        SetX(GetX()/fOther);
        SetY(GetY()/fOther);
        SetZ(GetZ()/fOther);
        return *this;
    }

    /// Vector sum
    Vector3d operator+(const Vector3d & vecOther) const
    {
        Vector3d vecNew = *this;
        vecNew += vecOther;
        return vecNew;
    }

    /// Vector difference
    Vector3d operator-(const Vector3d & vecOther) const
    {
        Vector3d vecNew = *this;
        vecNew -= vecOther;
        return vecNew;
    }

    /// Element by element multiplication
    Vector3d operator*(const Vector3d & vecOther) const
    {
        Vector3d vecNew = *this;
        vecNew *= vecOther;
        return vecNew;
    }

    /// Scalar multiplication
    Vector3d operator*(const double & fOther) const
    {
        Vector3d vecNew = *this;
        vecNew *= fOther;
        return vecNew;
    }

    /// Element by element division
    Vector3d operator/(const Vector3d & vecOther) const
    {
        Vector3d vecNew = *this;
        vecNew /= vecOther;
        return vecNew;
    }

    /// Scalar division
    Vector3d operator/(const double & fOther) const
    {
        Vector3d vecNew = *this;
        vecNew /= fOther;
        return vecNew;
    }

    ///
    Vector3d operator-() const
    {
        return Vector3d(-GetX(),-GetY(),-GetZ());
    }

    // ********************************************************************************************************
    // *** Class members **************************************************************************************
private:
    double m_vfElements[3];

}; /* Vector */

//// No idea why sometimes this method cause errors in linking
//std::ostream& operator<<(std::ostream& out, const Vector3d& v) {
//    out << "(" << v.GetX() << ", " << v.GetY() << ", " << v.GetZ() << ")";
//    return out;
//}

#endif /* __VECTOR3D_H__ */
