#ifndef __QUATERNION_H__
#define __QUATERNION_H__

#include <math.h>
#include "Vector3d.h"

/// Unit quaternion class
class Quaternion
{
public:
	Quaternion()
	{
		Set(Vector3d(0.0f,0.0f,0.0f),0.0f);
    }

	Quaternion(const Vector3d &vecV, const float fS)
	{
		Set(vecV,fS);
    }

	Quaternion(const Vector3d &vecV)
	{
		Set(vecV,0.0f);
    }

	Quaternion(const float fAngle, const Vector3d &vecAxis)
	{
		float fD = fAngle * 0.5f;
		Set(vecAxis.GetNormalized() * sinf(fD),cosf(fD));
    }

	Quaternion(const Vector3d &vecFrom, const Vector3d &vecTo)
	{
		const float fAngle = acosf(vecFrom.GetNormalized().Dot(vecTo.GetNormalized()));
		const Vector3d vecAxis = vecFrom.GetNormalized().Cross(vecTo.GetNormalized());
		(*this) = Quaternion(fAngle,vecAxis);
    }

	~Quaternion()
	{
    }

	void Set(const Vector3d &vecV, const float fS)
	{
		m_vecVector = vecV;
		m_fScalar = fS;
		normalize();
    }

	void SetVector(const Vector3d &vecV)
	{
		Set(vecV,GetScalar());
    }

	void SetScalar(const float fS)
	{
		Set(GetVector(),fS);
    }

	const Vector3d & GetVector() const
	{
		return m_vecVector;
    }

	const float & GetScalar() const
	{
		return m_fScalar;
    }

	const float GetMagnitude() const
	{
		return sqrtf( GetScalar()*GetScalar() + GetVector().GetMagnitude() * GetVector().GetMagnitude());
    }

	const Quaternion GetNormalized() const
	{
		Quaternion quaNew = *this;
		float fMagnitude = quaNew.GetMagnitude();

		if (fMagnitude > 0.0f)
		{
			quaNew.m_vecVector /= fMagnitude;
			quaNew.m_fScalar /= fMagnitude;
		}

		return quaNew;
    }

	const Quaternion GetInverse() const
	{
		return Quaternion(GetVector()*-1.0f,GetScalar()).GetNormalized();
    }

	void normalize()
	{
		(*this) = GetNormalized();
    }

	void invert()
	{
		(*this) = GetInverse();
    }

	const bool operator==(const Quaternion & quaOther) const
	{
		return (GetVector() == quaOther.GetVector()) && (GetScalar() == quaOther.GetScalar());
    }

	const bool operator!=(const Quaternion & quaOther) const
	{
		return !(*this == quaOther);
    }

	const Quaternion & operator*=(const float & fOther)
	{
		Set(GetVector() * fOther,GetScalar() * fOther);
		return *this;
    }

	const Quaternion & operator/=(const float & fOther)
	{
		Set(GetVector() / fOther,GetScalar() / fOther);
		return *this;
    }

	const Quaternion & operator+=(const Quaternion & quaOther)
	{
		Set(GetVector() + quaOther.GetVector(),GetScalar() + quaOther.GetScalar());
		return *this;
    }

	const Quaternion & operator-=(const Quaternion & quaOther)
	{
		Set(GetVector() - quaOther.GetVector(),GetScalar() - quaOther.GetScalar());
		return *this;
    }

	const Quaternion & operator*=(const Quaternion& quaOther)
	{
		Set(
			GetVector().Cross(quaOther.GetVector()) + GetVector() * quaOther.GetScalar()  + quaOther.GetVector() * GetScalar(),
			GetScalar() * quaOther.GetScalar() - GetVector().Dot(quaOther.GetVector()));
		return *this;
    }

	const Quaternion operator*(const float fOther) const
	{
		Quaternion quaNew = *this;
		quaNew *= fOther;
		return quaNew;
    }

	const Quaternion operator/(const float fOther) const
	{
		Quaternion quaNew = *this;
		quaNew *= fOther;
		return quaNew;
    }

	const Quaternion operator+(const Quaternion & quaOther) const
	{
		Quaternion quaNew = *this;
		quaNew += quaOther;
		return quaNew;
    }

	const Quaternion operator-(const Quaternion & quaOther) const
	{
		Quaternion quaNew = *this;
		quaNew += quaOther;
		return quaNew;
    }

	const Quaternion operator*(const Quaternion & quaOther) const
	{
		Quaternion quaNew = *this;
		quaNew *= quaOther;
		return quaNew;
    }

	const Vector3d operator*(const Vector3d &vecOther) const
	{
		Quaternion quaRotated = (*this * Quaternion(vecOther) * GetInverse());
		return quaRotated.GetVector() * vecOther.GetMagnitude();
    }

private:
	Vector3d m_vecVector;
	float m_fScalar;
};

#endif /* __QUATERNION_H__ */
