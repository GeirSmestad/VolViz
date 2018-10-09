#ifndef __MATRIX_H__
#define __MATRIX_H__

#include <cassert>
#include <cmath>
#include "Vector3d.h"
#include "Quaternion.h"

#include <memory.h>

const double PI = std::atan(1.) * 4.;

/** A 4x4 matrix of double values.
 * This matrix is *COLUMN MAJOR*, which means the 16 elements of the
 * matrix should be interpreted like this:
 *   M[0]  M[4]  M[8]  M[12]
 *   M[1]  M[5]  M[9]  M[13]
 *   M[2]  M[6]  M[10] M[14]
 *   M[3]  M[7]  M[11] M[15]
 */
class Matrix4d
{
    // ********************************************************************************************************
    // *** Static methods *************************************************************************************
public:
    /// Return a matrix representing a rotation of 'angle' degrees around the specified axis.
    static Matrix4d createRotation(double angle, const Vector3d &rotationAxis) {
        double x = rotationAxis.GetX();
        double y = rotationAxis.GetY();
        double z = rotationAxis.GetZ();
        double c = std::cos((angle * 2. * PI) / 360.);
        double s = std::sin((angle * 2. * PI) / 360.);

        return Matrix4d(
                x * x * (1 - c) + c, x * y * (1 - c) - z * s, x * z * (1 - c) + y * s, 0,
                y * x * (1 - c) + z * s, y * y * (1 - c) + c, y * z * (1 - c) - x * s, 0,
                z * x * (1 - c) - y * s, z * y * (1 - c) + x * s, z * z * (1 - c) + c, 0,
                0, 0, 0, 1);
    }

    /// Return an ortographic projection matrix according to the specified parameters
    static Matrix4d createOrthoPrj(double left, double right, double bottom, double top,
                                         double znear, double zfar)
    {
        double width = right - left;
        double height = top - bottom;
        double depth = zfar - znear;
        double sx = 2. / width;
        double sy = 2. / height;
        double sz = -2. / depth;
        double tx = -(left + right) / width;
        double ty = -(top + bottom) / height;
        double tz = -(zfar + znear) / depth;

        return Matrix4d (   sx, 0., 0., tx,
                            0., sy, 0., ty,
                            0., 0., sz, tz,
                            0., 0., 0., 1.);
    }

    /// Return a perspective projection matrix according to the specified parameters
    static Matrix4d createPerspectivePrj(double fov, double aspect, double znear, double zfar)
    {
        double top = znear * tanf(fov * PI / 360.0);
        double bottom = -top;
        double left = bottom * aspect;
        double right = top * aspect;

        double k = 2.0 * znear;
        double width = right - left;
        double height = top - bottom;
        double depth = zfar - znear;

        double xx = k / width;
        double yy = k / height;
        double xz = (right + left) / width;
        double yz = (top + bottom) / height;
        double zz = (-zfar - znear) / depth;
        double zw = (-k * zfar) / depth;

        return Matrix4d (   xx, 0., xz, 0.,
                            0., yy, yz, 0.,
                            0., 0., zz, zw,
                            0., 0.,-1., 0.);
    }

    // ********************************************************************************************************
    // *** Basic methods **************************************************************************************
public:
    /// Default constructor. Create an identity matrix
    Matrix4d()
	{
		m_vfElements.m_vfElements16[0]=m_vfElements.m_vfElements16[5]=m_vfElements.m_vfElements16[10]=m_vfElements.m_vfElements16[15]=1.0;
		m_vfElements.m_vfElements16[1]=m_vfElements.m_vfElements16[2]=m_vfElements.m_vfElements16[3]=m_vfElements.m_vfElements16[4]=m_vfElements.m_vfElements16[6]=m_vfElements.m_vfElements16[7]=m_vfElements.m_vfElements16[8]=m_vfElements.m_vfElements16[9]=m_vfElements.m_vfElements16[11]=m_vfElements.m_vfElements16[12]=m_vfElements.m_vfElements16[13]=m_vfElements.m_vfElements16[14]=0.0;
    }

    /// Create a matrix using the specified values. Values must be specified row-wise.
    Matrix4d(	const double & fElement0, const double & fElement4, const double & fElement8, const double & fElement12,
					const double & fElement1, const double & fElement5, const double & fElement9, const double & fElement13,
					const double & fElement2, const double & fElement6, const double & fElement10, const double & fElement14,
					const double & fElement3, const double & fElement7, const double & fElement11, const double & fElement15)
	{
		m_vfElements.m_vfElements16[0]=fElement0; m_vfElements.m_vfElements16[4]=fElement4;  m_vfElements.m_vfElements16[8]=fElement8;  m_vfElements.m_vfElements16[12]=fElement12;
		m_vfElements.m_vfElements16[1]=fElement1; m_vfElements.m_vfElements16[5]=fElement5;  m_vfElements.m_vfElements16[9]=fElement9;  m_vfElements.m_vfElements16[13]=fElement13;
		m_vfElements.m_vfElements16[2]=fElement2; m_vfElements.m_vfElements16[6]=fElement6; m_vfElements.m_vfElements16[10]=fElement10; m_vfElements.m_vfElements16[14]=fElement14;
		m_vfElements.m_vfElements16[3]=fElement3; m_vfElements.m_vfElements16[7]=fElement7; m_vfElements.m_vfElements16[11]=fElement11; m_vfElements.m_vfElements16[15]=fElement15;
    }

    /// Create a matrix using the specified vectors. each vector represent a row-
    Matrix4d(const Vector3d & vecOne, const Vector3d & vecTwo, const Vector3d & vecThree, const Vector3d & vecFour = Vector3d(0.0,0.0,0.0))
	{

        m_vfElements.m_vfElements16[0]=vecOne.GetX(); m_vfElements.m_vfElements16[4]=vecOne.GetY();  m_vfElements.m_vfElements16[8]=vecOne.GetZ();  m_vfElements.m_vfElements16[12]=0.0;
		m_vfElements.m_vfElements16[1]=vecTwo.GetX(); m_vfElements.m_vfElements16[5]=vecTwo.GetY();  m_vfElements.m_vfElements16[9]=vecTwo.GetZ();  m_vfElements.m_vfElements16[13]=0.0;
		m_vfElements.m_vfElements16[2]=vecThree.GetX(); m_vfElements.m_vfElements16[6]=vecThree.GetY(); m_vfElements.m_vfElements16[10]=vecThree.GetZ(); m_vfElements.m_vfElements16[14]=0.0;
        m_vfElements.m_vfElements16[3]=vecFour.GetX(); m_vfElements.m_vfElements16[7]=vecFour.GetY(); m_vfElements.m_vfElements16[11]=vecFour.GetZ(); m_vfElements.m_vfElements16[15]=1.0;
    }

    /// Create a matrix using the specified values. Values must be specified row-wise.
    Matrix4d(const double *pElements)
	{
		memcpy((void*)m_vfElements.m_vfElements16, (void *)pElements, 16*sizeof(double));
    }

    /// Create a matrix using the specified values.
    Matrix4d(const double vfElements[4][4])
	{
		memcpy((void*)m_vfElements.m_vfElements16, (void *)vfElements, 16*sizeof(double));
    }

    /// Create a rotation matrix according to the specified quaternion
    Matrix4d(const Quaternion & quaRotation)
	{
		m_vfElements.m_vfElements16[0]=m_vfElements.m_vfElements16[5]=m_vfElements.m_vfElements16[10]=m_vfElements.m_vfElements16[15]=1.0;
		m_vfElements.m_vfElements16[1]=m_vfElements.m_vfElements16[2]=m_vfElements.m_vfElements16[3]=m_vfElements.m_vfElements16[4]=m_vfElements.m_vfElements16[6]=m_vfElements.m_vfElements16[7]=m_vfElements.m_vfElements16[8]=m_vfElements.m_vfElements16[9]=m_vfElements.m_vfElements16[11]=m_vfElements.m_vfElements16[12]=m_vfElements.m_vfElements16[13]=m_vfElements.m_vfElements16[14]=0.0;
		rotate(quaRotation);
    }

    /// Copy constructor
    Matrix4d(const Matrix4d& matOther)
    {
        this->Set(matOther.m_vfElements.m_vfElements16);
    }

    /// Assignment operator
    Matrix4d& operator=(const Matrix4d & matOther)
    {
        this->Set(matOther.m_vfElements.m_vfElements16);
        return *this;
    }

    /// Destructor
    ~Matrix4d()
	{
    }


    // ********************************************************************************************************
    // *** Public methods *************************************************************************************
public:
    /// Set the elements of the matrix.
	void Set(const double *pElements)
	{
		memcpy((void*)m_vfElements.m_vfElements16, (void *)pElements, 16*sizeof(double));
    }

    /// Set the identity matrix as the current matrix.
    void identity()
	{
		m_vfElements.m_vfElements16[0]=m_vfElements.m_vfElements16[5]=m_vfElements.m_vfElements16[10]=m_vfElements.m_vfElements16[15]=1.0;
		m_vfElements.m_vfElements16[1]=m_vfElements.m_vfElements16[2]=m_vfElements.m_vfElements16[3]=m_vfElements.m_vfElements16[4]=m_vfElements.m_vfElements16[6]=m_vfElements.m_vfElements16[7]=m_vfElements.m_vfElements16[8]=m_vfElements.m_vfElements16[9]=m_vfElements.m_vfElements16[11]=m_vfElements.m_vfElements16[12]=m_vfElements.m_vfElements16[13]=m_vfElements.m_vfElements16[14]=0.0;
    }

    /// Return a (constant) pointer to the matrix elements.
	const double * Get() const
	{
		return (double *) m_vfElements.m_vfElements16;
    }

    /// Set the value of the matrix element at (row, col).
    void Set(const unsigned int row, const unsigned int col, const double & fValue)
	{
        assert(row < 4 && col < 4);
        m_vfElements.m_vfElements44[row][col] = fValue;
    }

    /// Return the matrix element at (row, col).
    const double & Get(const unsigned int row, const unsigned int col) const
	{
        assert(row < 4 && col < 4);
        return m_vfElements.m_vfElements44[row][col];
    }

    /// Return the matrix element at the specified index.
	const double & Get(const unsigned int i) const
	{
        assert(i < 16);
		return m_vfElements.m_vfElements16[i];
    }

    /// Set the values of a specific row of the matrix.
    void SetRow(const unsigned int row, const double vfValues[4])
	{
        assert(row < 4);
        m_vfElements.m_vfElements16[row + 0] = vfValues[0];
        m_vfElements.m_vfElements16[row + 4] = vfValues[1];
        m_vfElements.m_vfElements16[row + 8] = vfValues[2];
        m_vfElements.m_vfElements16[row +12] = vfValues[3];
    }

    /// Set the values of a specific column of the matrix.
    void SetColumn(const unsigned int col, const double vfValues[4])
	{
        assert(col < 4);
        m_vfElements.m_vfElements16[col*4+0] = vfValues[0];
        m_vfElements.m_vfElements16[col*4+1] = vfValues[1];
        m_vfElements.m_vfElements16[col*4+2] = vfValues[2];
        m_vfElements.m_vfElements16[col*4+3] = vfValues[3];
    }

    /// Set the values of a specific row of the matrix. The 4th value of the row is not modified.
    void SetRow(const unsigned int row, const Vector3d & vecVector)
	{
        assert(row < 4);
        m_vfElements.m_vfElements16[row + 0] = vecVector.GetX();
        m_vfElements.m_vfElements16[row + 4] = vecVector.GetY();
        m_vfElements.m_vfElements16[row + 8] = vecVector.GetZ();
    }

    /// Set the values of a specific columns of the matrix. The 4th value of the column is not modified.
    void SetColumn(const unsigned int col, const Vector3d & vecVector)
	{
        assert(col < 4);
        m_vfElements.m_vfElements16[col*4+0] = vecVector.GetX();
        m_vfElements.m_vfElements16[col*4+1] = vecVector.GetY();
        m_vfElements.m_vfElements16[col*4+2] = vecVector.GetZ();
    }

    /// Access the matrix element at (row, col).
    double & operator()(const unsigned int row, const unsigned int col)
	{
        assert(row < 4 && col < 4);
        return m_vfElements.m_vfElements44[row][col];
    }

    /// Access the matrix element at (row, col).
    const double & operator()(const unsigned int row, const unsigned int col) const
	{
        assert(row < 4 && col < 4);
        return m_vfElements.m_vfElements44[row][col];
    }

    /// Set the translation part of the matrix.
	void SetTranslation(const Vector3d & vecTranslation)
	{
		m_vfElements.m_vfElements16[12] = vecTranslation.GetX();
		m_vfElements.m_vfElements16[13] = vecTranslation.GetY();
		m_vfElements.m_vfElements16[14] = vecTranslation.GetZ();
		m_vfElements.m_vfElements16[15] = 1.0;
    }

    /// Return the translation part of the matrix.
	const Vector3d GetTranslation() const
	{
		return Vector3d(m_vfElements.m_vfElements16[12],m_vfElements.m_vfElements16[13],m_vfElements.m_vfElements16[14]);
    }

    /// Return the rotation part of the matrix as a quaternion.
	const Quaternion GetRotation() const
	{
        const double fTrace = m_vfElements.m_vfElements16[0] + m_vfElements.m_vfElements16[5] + m_vfElements.m_vfElements16[10] + m_vfElements.m_vfElements16[15];

		if (fTrace > 0.0)
		{
            const double fS = 0.5f / std::sqrt(fTrace);
			const double fW = 0.25f / fS;
			const double fX = ( m_vfElements.m_vfElements16[6] - m_vfElements.m_vfElements16[9] ) * fS;
			const double fY = ( m_vfElements.m_vfElements16[8] - m_vfElements.m_vfElements16[2] ) * fS;
			const double fZ = ( m_vfElements.m_vfElements16[1] - m_vfElements.m_vfElements16[4] ) * fS;
			return Quaternion(Vector3d(fX,fY,fZ).GetNormalized(),fW);
		}
		else
		{
			const double fMaximum = (std::max)(m_vfElements.m_vfElements44[0][0],(std::max)(m_vfElements.m_vfElements44[1][1],m_vfElements.m_vfElements44[2][2]));

			if (fMaximum == m_vfElements.m_vfElements44[0][0])
			{
                const double fS  = std::sqrt( 1.0 + m_vfElements.m_vfElements16[0] - m_vfElements.m_vfElements16[5] - m_vfElements.m_vfElements16[10] ) * 2.0;
				const double fX = 0.5f / fS;
				const double fY = (m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[1] ) / fS;
				const double fZ = (m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[2] ) / fS;
				const double fW = (m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[6] ) / fS;
				return Quaternion(Vector3d(fX,fY,fZ).GetNormalized(),fW);
			}
			else if (fMaximum == m_vfElements.m_vfElements44[1][1])
			{
                const double fS  = std::sqrt( 1.0 + m_vfElements.m_vfElements16[5] - m_vfElements.m_vfElements16[0] - m_vfElements.m_vfElements16[10] ) * 2.0;
				const double fX = (m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[1] ) / fS;
				const double fY = 0.5f / fS;
				const double fZ = (m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[6] ) / fS;
				const double fW = (m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[2] ) / fS;
				return Quaternion(Vector3d(fX,fY,fZ).GetNormalized(),fW);
			}
			else
			{
				const double fS  = sqrtf( 1.0 + m_vfElements.m_vfElements16[10] - m_vfElements.m_vfElements16[0] - m_vfElements.m_vfElements16[5] ) * 2.0;
				const double fX = (m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[2] ) / fS;
				const double fY = (m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[6] ) / fS;
				const double fZ = 0.5f / fS;
				const double fW = (m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[1] ) / fS;
				return Quaternion(Vector3d(fX,fY,fZ).GetNormalized(),fW);
			}
		}
    }

    /// Return a matrix obtained postmultiplying this matrix by a translation matrix.
    const Matrix4d GetTranslated(const Vector3d & vecTranslation) const
	{
        return (*this) * Matrix4d(1.0,0.0,0.0,vecTranslation.GetX(), 0.0,1.0,0.0,vecTranslation.GetY(), 0.0,0.0,1.0,vecTranslation.GetZ(), 0.0, 0.0,0.0,1.0);
    }

    /// Postmultiply this matrix by a translation matrix.
	void translate(const Vector3d &vecTranslation)
	{
		*this = GetTranslated(vecTranslation);
    }

    /// Return a matrix obtained postmultiplying this matrix by a scaling matrix.
    const Matrix4d GetScaled(const Vector3d &vecScale) const
	{
        return (*this) * 	Matrix4d(	vecScale.GetX(),0.0,0.0,0.0, 0.0,vecScale.GetY(),0.0,0.0,	0.0,0.0,vecScale.GetZ(),0.0, 0.0,0.0,0.0,1.0);
    }

    /// Postmultiply this matrix by a scaling matrix.
	void scale(const Vector3d &vecScale)
	{
		*this = GetScaled(vecScale);
    }

    /// Return a matrix obtained postmultiplying this matrix by a rotation matrix.
    Matrix4d GetRotated(const Quaternion & quaRotation) const
	{
		const double fXX = quaRotation.GetVector().GetX() * quaRotation.GetVector().GetX();
		const double fXY = quaRotation.GetVector().GetX() * quaRotation.GetVector().GetY();
		const double fXZ = quaRotation.GetVector().GetX() * quaRotation.GetVector().GetZ();
		const double fXW = quaRotation.GetVector().GetX() * quaRotation.GetScalar();
		const double fYY = quaRotation.GetVector().GetY() * quaRotation.GetVector().GetY();
		const double fYZ = quaRotation.GetVector().GetY() * quaRotation.GetVector().GetZ();
		const double fYW = quaRotation.GetVector().GetY() * quaRotation.GetScalar();
		const double fZZ = quaRotation.GetVector().GetZ() * quaRotation.GetVector().GetZ();
		const double fZW = quaRotation.GetVector().GetZ() * quaRotation.GetScalar();

        return (*this) * Matrix4d(1.0 - 2.0 * (fYY + fZZ), 2.0 * (fXY - fZW), 2.0 * (fXZ + fYW), 0.0,  2.0 * (fXY + fZW), 1.0 - 2.0 * (fXX + fZZ), 2.0 * (fYZ - fXW), 0.0, 2.0 * (fXZ - fYW), 2.0 * (fYZ + fXW), 1.0 - 2.0 * (fXX + fYY), 0.0,	0.0, 0.0, 0.0, 1.0);
    }

    /// Postmultiply this matrix by a rotation matrix.
    void rotate(const Quaternion & quaRotation)
    {
        *this = GetRotated(quaRotation);
    }

    /// Return a matrix obtained postmultiplying this matrix by a rotation matrix.
    Matrix4d GetRotated(double angle, const Vector3d &rotationAxis) const
    {
        return (*this) * createRotation(angle, rotationAxis);
    }

    /// Postmultiply this matrix by a rotation matrix.
    void rotate(double angle, const Vector3d &rotationAxis)
    {
        *this = GetRotated(angle, rotationAxis);
    }

    /// Return the inverse of this matrix
    Matrix4d GetInverse() const
	{
		const double fTmp1 = m_vfElements.m_vfElements44[2][2]*m_vfElements.m_vfElements44[3][3];
		const double fTmp2 = m_vfElements.m_vfElements44[0][1]*fTmp1;
		const double fTmp4 = m_vfElements.m_vfElements44[2][3]*m_vfElements.m_vfElements44[3][2];
		const double fTmp5 = m_vfElements.m_vfElements44[0][1]*fTmp4;
		const double fTmp6 = m_vfElements.m_vfElements44[0][2]*m_vfElements.m_vfElements44[3][3];
		const double fTmp7 = m_vfElements.m_vfElements44[2][1]*fTmp6;
		const double fTmp8 = m_vfElements.m_vfElements44[0][3]*m_vfElements.m_vfElements44[3][2];
		const double fTmp9 = m_vfElements.m_vfElements44[2][1]*fTmp8;
		const double fTmp11 = m_vfElements.m_vfElements44[0][2]*m_vfElements.m_vfElements44[2][3];
		const double fTmp12 = m_vfElements.m_vfElements44[3][1]*fTmp11;
		const double fTmp14 = m_vfElements.m_vfElements44[0][3]*m_vfElements.m_vfElements44[2][2];
		const double fTmp15 = m_vfElements.m_vfElements44[3][1]*fTmp14;
		const double fTmp17 = m_vfElements.m_vfElements44[1][1]*fTmp1;
		const double fTmp19 = m_vfElements.m_vfElements44[1][1]*fTmp4;
		const double fTmp22 = m_vfElements.m_vfElements44[1][2]*m_vfElements.m_vfElements44[3][3];
		const double fTmp23 = m_vfElements.m_vfElements44[2][1]*fTmp22;
		const double fTmp26 = m_vfElements.m_vfElements44[1][3]*m_vfElements.m_vfElements44[3][2];
		const double fTmp27 = m_vfElements.m_vfElements44[2][1]*fTmp26;
		const double fTmp29 = m_vfElements.m_vfElements44[1][2]*m_vfElements.m_vfElements44[2][3];
		const double fTmp30 = m_vfElements.m_vfElements44[3][1]*fTmp29;
		const double fTmp32 = m_vfElements.m_vfElements44[1][3]*m_vfElements.m_vfElements44[2][2];
		const double fTmp33 = m_vfElements.m_vfElements44[3][1]*fTmp32;
		const double fTmp48 = m_vfElements.m_vfElements44[0][1]*fTmp22;
		const double fTmp50 = m_vfElements.m_vfElements44[0][1]*fTmp26;
		const double fTmp53 = m_vfElements.m_vfElements44[1][1]*fTmp6;
		const double fTmp56 = m_vfElements.m_vfElements44[1][1]*fTmp8;
		const double fTmp58 = m_vfElements.m_vfElements44[0][2]*m_vfElements.m_vfElements44[1][3];
		const double fTmp59 = m_vfElements.m_vfElements44[3][1]*fTmp58;
		const double fTmp61 = m_vfElements.m_vfElements44[0][3]*m_vfElements.m_vfElements44[1][2];
		const double fTmp62 = m_vfElements.m_vfElements44[3][1]*fTmp61;
		const double fTmp66 = m_vfElements.m_vfElements44[0][1]*fTmp29;
		const double fTmp69 = m_vfElements.m_vfElements44[0][1]*fTmp32;
		const double fTmp71 = m_vfElements.m_vfElements44[1][1]*fTmp11;
		const double fTmp73 = m_vfElements.m_vfElements44[1][1]*fTmp14;
		const double fTmp76 = m_vfElements.m_vfElements44[2][1]*fTmp58;
		const double fTmp79 = m_vfElements.m_vfElements44[2][1]*fTmp61;
		const double fTmp84 = 1.0/(m_vfElements.m_vfElements44[0][0]*fTmp17-m_vfElements.m_vfElements44[0][0]*fTmp19-m_vfElements.m_vfElements44[0][0]*fTmp23+m_vfElements.m_vfElements44[0][0]*fTmp27+m_vfElements.m_vfElements44[0][0]*fTmp30-m_vfElements.m_vfElements44[0][0]*fTmp33-m_vfElements.m_vfElements44[1][0]*fTmp2+m_vfElements.m_vfElements44[1][0]*fTmp5+m_vfElements.m_vfElements44[1][0]*fTmp7-m_vfElements.m_vfElements44[1][0]*fTmp9-m_vfElements.m_vfElements44[1][0]*fTmp12+m_vfElements.m_vfElements44[1][0]*fTmp15+m_vfElements.m_vfElements44[2][0]*fTmp48-m_vfElements.m_vfElements44[2][0]*fTmp50-m_vfElements.m_vfElements44[2][0]*fTmp53+m_vfElements.m_vfElements44[2][0]*fTmp56+m_vfElements.m_vfElements44[2][0]*fTmp59-m_vfElements.m_vfElements44[2][0]*fTmp62-m_vfElements.m_vfElements44[3][0]*fTmp66+m_vfElements.m_vfElements44[3][0]*fTmp69+m_vfElements.m_vfElements44[3][0]*fTmp71-m_vfElements.m_vfElements44[3][0]*fTmp73-m_vfElements.m_vfElements44[3][0]*fTmp76+m_vfElements.m_vfElements44[3][0]*fTmp79);
		const double fTmp116 = m_vfElements.m_vfElements44[2][1]*m_vfElements.m_vfElements44[3][2];
		const double fTmp118 = m_vfElements.m_vfElements44[2][2]*m_vfElements.m_vfElements44[3][1];
		const double fTmp121 = m_vfElements.m_vfElements44[0][1]*m_vfElements.m_vfElements44[3][2];
		const double fTmp124 = m_vfElements.m_vfElements44[0][2]*m_vfElements.m_vfElements44[3][1];
		const double fTmp126 = m_vfElements.m_vfElements44[0][1]*m_vfElements.m_vfElements44[2][2];
		const double fTmp128 = m_vfElements.m_vfElements44[0][2]*m_vfElements.m_vfElements44[2][1];
		const double fTmp133 = m_vfElements.m_vfElements44[2][1]*m_vfElements.m_vfElements44[3][3];
		const double fTmp135 = m_vfElements.m_vfElements44[2][3]*m_vfElements.m_vfElements44[3][1];
		const double fTmp138 = m_vfElements.m_vfElements44[0][1]*m_vfElements.m_vfElements44[3][3];
		const double fTmp141 = m_vfElements.m_vfElements44[0][3]*m_vfElements.m_vfElements44[3][1];
		const double fTmp143 = m_vfElements.m_vfElements44[0][1]*m_vfElements.m_vfElements44[2][3];
		const double fTmp145 = m_vfElements.m_vfElements44[0][3]*m_vfElements.m_vfElements44[2][1];
		const double fTmp151 = m_vfElements.m_vfElements44[1][1]*m_vfElements.m_vfElements44[2][2];
		const double fTmp153 = m_vfElements.m_vfElements44[1][2]*m_vfElements.m_vfElements44[2][1];
		const double fTmp159 = m_vfElements.m_vfElements44[0][1]*m_vfElements.m_vfElements44[1][2];
		const double fTmp161 = m_vfElements.m_vfElements44[0][2]*m_vfElements.m_vfElements44[1][1];
		const double fTmp166 = m_vfElements.m_vfElements44[1][1]*m_vfElements.m_vfElements44[3][3];
		const double fTmp168 = m_vfElements.m_vfElements44[1][3]*m_vfElements.m_vfElements44[3][1];
		const double fTmp174 = m_vfElements.m_vfElements44[0][1]*m_vfElements.m_vfElements44[1][3];
		const double fTmp176 = m_vfElements.m_vfElements44[0][3]*m_vfElements.m_vfElements44[1][1];
		const double fTmp187 = m_vfElements.m_vfElements44[1][1]*m_vfElements.m_vfElements44[2][3];
		const double fTmp189 = m_vfElements.m_vfElements44[1][3]*m_vfElements.m_vfElements44[2][1];
		const double fTmp212 = m_vfElements.m_vfElements44[1][1]*m_vfElements.m_vfElements44[3][2];
		const double fTmp214 = m_vfElements.m_vfElements44[1][2]*m_vfElements.m_vfElements44[3][1];

        Matrix4d matNew;
		matNew.m_vfElements.m_vfElements44[0][1] = (-fTmp2+fTmp5+fTmp7-fTmp9-fTmp12+fTmp15)*fTmp84;
		matNew.m_vfElements.m_vfElements44[0][2] = -(-fTmp48+fTmp50+fTmp53-fTmp56-fTmp59+fTmp62)*fTmp84;
		matNew.m_vfElements.m_vfElements44[1][2] = -(m_vfElements.m_vfElements44[0][0]*fTmp22-m_vfElements.m_vfElements44[0][0]*fTmp26-m_vfElements.m_vfElements44[1][0]*fTmp6+m_vfElements.m_vfElements44[1][0]*fTmp8+m_vfElements.m_vfElements44[3][0]*fTmp58-m_vfElements.m_vfElements44[3][0]*fTmp61)*fTmp84;
		matNew.m_vfElements.m_vfElements44[1][3] = -(-m_vfElements.m_vfElements44[0][0]*fTmp29+m_vfElements.m_vfElements44[0][0]*fTmp32+m_vfElements.m_vfElements44[1][0]*fTmp11-m_vfElements.m_vfElements44[1][0]*fTmp14-m_vfElements.m_vfElements44[2][0]*fTmp58+m_vfElements.m_vfElements44[2][0]*fTmp61)*fTmp84;
		matNew.m_vfElements.m_vfElements44[3][1] = (m_vfElements.m_vfElements44[0][0]*fTmp116-m_vfElements.m_vfElements44[0][0]*fTmp118-m_vfElements.m_vfElements44[2][0]*fTmp121+m_vfElements.m_vfElements44[2][0]*fTmp124+m_vfElements.m_vfElements44[3][0]*fTmp126-m_vfElements.m_vfElements44[3][0]*fTmp128)*fTmp84;
		matNew.m_vfElements.m_vfElements44[2][1] = -(m_vfElements.m_vfElements44[0][0]*fTmp133-m_vfElements.m_vfElements44[0][0]*fTmp135-m_vfElements.m_vfElements44[2][0]*fTmp138+m_vfElements.m_vfElements44[2][0]*fTmp141+m_vfElements.m_vfElements44[3][0]*fTmp143-m_vfElements.m_vfElements44[3][0]*fTmp145)*fTmp84;
		matNew.m_vfElements.m_vfElements44[3][3] = (m_vfElements.m_vfElements44[0][0]*fTmp151-m_vfElements.m_vfElements44[0][0]*fTmp153-m_vfElements.m_vfElements44[1][0]*fTmp126+m_vfElements.m_vfElements44[1][0]*fTmp128+m_vfElements.m_vfElements44[2][0]*fTmp159-m_vfElements.m_vfElements44[2][0]*fTmp161)*fTmp84;
		matNew.m_vfElements.m_vfElements44[2][2] = (m_vfElements.m_vfElements44[0][0]*fTmp166-m_vfElements.m_vfElements44[0][0]*fTmp168-m_vfElements.m_vfElements44[1][0]*fTmp138+m_vfElements.m_vfElements44[1][0]*fTmp141+m_vfElements.m_vfElements44[3][0]*fTmp174-m_vfElements.m_vfElements44[3][0]*fTmp176)*fTmp84;
		matNew.m_vfElements.m_vfElements44[2][0] = (m_vfElements.m_vfElements44[1][0]*fTmp133-m_vfElements.m_vfElements44[1][0]*fTmp135-m_vfElements.m_vfElements44[2][0]*fTmp166+m_vfElements.m_vfElements44[2][0]*fTmp168+m_vfElements.m_vfElements44[3][0]*fTmp187-m_vfElements.m_vfElements44[3][0]*fTmp189)*fTmp84;
		matNew.m_vfElements.m_vfElements44[1][0] = -(m_vfElements.m_vfElements44[1][0]*fTmp1-m_vfElements.m_vfElements44[1][0]*fTmp4-m_vfElements.m_vfElements44[2][0]*fTmp22+m_vfElements.m_vfElements44[2][0]*fTmp26+m_vfElements.m_vfElements44[3][0]*fTmp29-m_vfElements.m_vfElements44[3][0]*fTmp32)*fTmp84;
		matNew.m_vfElements.m_vfElements44[0][3] = -(fTmp66-fTmp69-fTmp71+fTmp73+fTmp76-fTmp79)*fTmp84;
		matNew.m_vfElements.m_vfElements44[3][2] = -(m_vfElements.m_vfElements44[0][0]*fTmp212-m_vfElements.m_vfElements44[0][0]*fTmp214-m_vfElements.m_vfElements44[1][0]*fTmp121+m_vfElements.m_vfElements44[1][0]*fTmp124+m_vfElements.m_vfElements44[3][0]*fTmp159-m_vfElements.m_vfElements44[3][0]*fTmp161)*fTmp84;
		matNew.m_vfElements.m_vfElements44[0][0] = (fTmp17-fTmp19-fTmp23+fTmp27+fTmp30-fTmp33)*fTmp84;
		matNew.m_vfElements.m_vfElements44[2][3] = -(m_vfElements.m_vfElements44[0][0]*fTmp187-m_vfElements.m_vfElements44[0][0]*fTmp189-m_vfElements.m_vfElements44[1][0]*fTmp143+m_vfElements.m_vfElements44[1][0]*fTmp145+m_vfElements.m_vfElements44[2][0]*fTmp174-m_vfElements.m_vfElements44[2][0]*fTmp176)*fTmp84;
		matNew.m_vfElements.m_vfElements44[3][0] = -(m_vfElements.m_vfElements44[1][0]*fTmp116-m_vfElements.m_vfElements44[1][0]*fTmp118-m_vfElements.m_vfElements44[2][0]*fTmp212+m_vfElements.m_vfElements44[2][0]*fTmp214+m_vfElements.m_vfElements44[3][0]*fTmp151-m_vfElements.m_vfElements44[3][0]*fTmp153)*fTmp84;
		matNew.m_vfElements.m_vfElements44[1][1] = (m_vfElements.m_vfElements44[0][0]*fTmp1-m_vfElements.m_vfElements44[0][0]*fTmp4-m_vfElements.m_vfElements44[2][0]*fTmp6+m_vfElements.m_vfElements44[2][0]*fTmp8+m_vfElements.m_vfElements44[3][0]*fTmp11-m_vfElements.m_vfElements44[3][0]*fTmp14)*fTmp84;

		return matNew;
    }

    /// Invert this matrix
	void invert()
	{
		(*this) = GetInverse();
	}

    /// Return the transposed of this matrix
    const Matrix4d GetTransposed() const
	{
        return Matrix4d(
			m_vfElements.m_vfElements16[0],m_vfElements.m_vfElements16[1],m_vfElements.m_vfElements16[2],m_vfElements.m_vfElements16[3],
			m_vfElements.m_vfElements16[4],m_vfElements.m_vfElements16[5],m_vfElements.m_vfElements16[6],m_vfElements.m_vfElements16[7],
			m_vfElements.m_vfElements16[8],m_vfElements.m_vfElements16[9],m_vfElements.m_vfElements16[10],m_vfElements.m_vfElements16[11],
			m_vfElements.m_vfElements16[12],m_vfElements.m_vfElements16[13],m_vfElements.m_vfElements16[14],m_vfElements.m_vfElements16[15]);
    }

    /// Transpose this matrix
	void transpose()
	{
		(*this) = GetTransposed();
    }

    /// Return the affine transformation (discard translation) of the specified vector according to this matrix.
    const Vector3d AffineMul(const Vector3d & vecOther) const
	{
		return Vector3d(
			(m_vfElements.m_vfElements16[0]*vecOther.GetX() + m_vfElements.m_vfElements16[4]*vecOther.GetY() + m_vfElements.m_vfElements16[8]*vecOther.GetZ()),
			(m_vfElements.m_vfElements16[1]*vecOther.GetX() + m_vfElements.m_vfElements16[5]*vecOther.GetY() + m_vfElements.m_vfElements16[9]*vecOther.GetZ()),
			(m_vfElements.m_vfElements16[2]*vecOther.GetX() + m_vfElements.m_vfElements16[6]*vecOther.GetY() + m_vfElements.m_vfElements16[10]*vecOther.GetZ()));
    }

    /// Return true if two matrices are identical.
    const bool operator==(const Matrix4d & matOther) const
	{
		for (unsigned int i=0;i < 16; i++)
		{
			if (m_vfElements.m_vfElements16[i] != matOther.m_vfElements.m_vfElements16[i])
				return false;
		}

		return true;
    }

    /// Return true if two matrices have at least a different element
    const bool operator!=(const Matrix4d & matOther) const
	{
		return !(*this == matOther);
    }

    /// Multiply this matrix by the specified one
    void mul(const Matrix4d & matOther) {
        *this = *this * matOther;
	}

    /// Multiply this matrix by the specified one
    void postmul(const Matrix4d & matOther) {
        *this =   *this * matOther;
    }

    /// Multiply the specified matrix by this one
    void premul(const Matrix4d & matOther) {
		*this = matOther * *this  ;
	}

    /// Return the product between this and the specified matrix
    const Matrix4d operator*(const Matrix4d & matOther) const
	{
        return Matrix4d(
			m_vfElements.m_vfElements16[0]*matOther.m_vfElements.m_vfElements16[0] + m_vfElements.m_vfElements16[4]*matOther.m_vfElements.m_vfElements16[1] + m_vfElements.m_vfElements16[8]*matOther.m_vfElements.m_vfElements16[2] + m_vfElements.m_vfElements16[12]*matOther.m_vfElements.m_vfElements16[3],
			m_vfElements.m_vfElements16[0]*matOther.m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[4]*matOther.m_vfElements.m_vfElements16[5] + m_vfElements.m_vfElements16[8]*matOther.m_vfElements.m_vfElements16[6] + m_vfElements.m_vfElements16[12]*matOther.m_vfElements.m_vfElements16[7],
			m_vfElements.m_vfElements16[0]*matOther.m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[4]*matOther.m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[8]*matOther.m_vfElements.m_vfElements16[10] + m_vfElements.m_vfElements16[12]*matOther.m_vfElements.m_vfElements16[11],
			m_vfElements.m_vfElements16[0]*matOther.m_vfElements.m_vfElements16[12] + m_vfElements.m_vfElements16[4]*matOther.m_vfElements.m_vfElements16[13] + m_vfElements.m_vfElements16[8]*matOther.m_vfElements.m_vfElements16[14] + m_vfElements.m_vfElements16[12]*matOther.m_vfElements.m_vfElements16[15],

			m_vfElements.m_vfElements16[1]*matOther.m_vfElements.m_vfElements16[0] + m_vfElements.m_vfElements16[5]*matOther.m_vfElements.m_vfElements16[1] + m_vfElements.m_vfElements16[9]*matOther.m_vfElements.m_vfElements16[2] + m_vfElements.m_vfElements16[13]*matOther.m_vfElements.m_vfElements16[3],
			m_vfElements.m_vfElements16[1]*matOther.m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[5]*matOther.m_vfElements.m_vfElements16[5] + m_vfElements.m_vfElements16[9]*matOther.m_vfElements.m_vfElements16[6] + m_vfElements.m_vfElements16[13]*matOther.m_vfElements.m_vfElements16[7],
			m_vfElements.m_vfElements16[1]*matOther.m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[5]*matOther.m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[9]*matOther.m_vfElements.m_vfElements16[10] + m_vfElements.m_vfElements16[13]*matOther.m_vfElements.m_vfElements16[11],
			m_vfElements.m_vfElements16[1]*matOther.m_vfElements.m_vfElements16[12] + m_vfElements.m_vfElements16[5]*matOther.m_vfElements.m_vfElements16[13] + m_vfElements.m_vfElements16[9]*matOther.m_vfElements.m_vfElements16[14] + m_vfElements.m_vfElements16[13]*matOther.m_vfElements.m_vfElements16[15],

			m_vfElements.m_vfElements16[2]*matOther.m_vfElements.m_vfElements16[0] + m_vfElements.m_vfElements16[6]*matOther.m_vfElements.m_vfElements16[1] + m_vfElements.m_vfElements16[10]*matOther.m_vfElements.m_vfElements16[2] + m_vfElements.m_vfElements16[14]*matOther.m_vfElements.m_vfElements16[3],
			m_vfElements.m_vfElements16[2]*matOther.m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[6]*matOther.m_vfElements.m_vfElements16[5] + m_vfElements.m_vfElements16[10]*matOther.m_vfElements.m_vfElements16[6] + m_vfElements.m_vfElements16[14]*matOther.m_vfElements.m_vfElements16[7],
			m_vfElements.m_vfElements16[2]*matOther.m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[6]*matOther.m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[10]*matOther.m_vfElements.m_vfElements16[10] + m_vfElements.m_vfElements16[14]*matOther.m_vfElements.m_vfElements16[11],
			m_vfElements.m_vfElements16[2]*matOther.m_vfElements.m_vfElements16[12] + m_vfElements.m_vfElements16[6]*matOther.m_vfElements.m_vfElements16[13] + m_vfElements.m_vfElements16[10]*matOther.m_vfElements.m_vfElements16[14] + m_vfElements.m_vfElements16[14]*matOther.m_vfElements.m_vfElements16[15],

			m_vfElements.m_vfElements16[3]*matOther.m_vfElements.m_vfElements16[0] + m_vfElements.m_vfElements16[7]*matOther.m_vfElements.m_vfElements16[1] + m_vfElements.m_vfElements16[11]*matOther.m_vfElements.m_vfElements16[2] + m_vfElements.m_vfElements16[15]*matOther.m_vfElements.m_vfElements16[3],
			m_vfElements.m_vfElements16[3]*matOther.m_vfElements.m_vfElements16[4] + m_vfElements.m_vfElements16[7]*matOther.m_vfElements.m_vfElements16[5] + m_vfElements.m_vfElements16[11]*matOther.m_vfElements.m_vfElements16[6] + m_vfElements.m_vfElements16[15]*matOther.m_vfElements.m_vfElements16[7],
			m_vfElements.m_vfElements16[3]*matOther.m_vfElements.m_vfElements16[8] + m_vfElements.m_vfElements16[7]*matOther.m_vfElements.m_vfElements16[9] + m_vfElements.m_vfElements16[11]*matOther.m_vfElements.m_vfElements16[10] + m_vfElements.m_vfElements16[15]*matOther.m_vfElements.m_vfElements16[11],
			m_vfElements.m_vfElements16[3]*matOther.m_vfElements.m_vfElements16[12] + m_vfElements.m_vfElements16[7]*matOther.m_vfElements.m_vfElements16[13] + m_vfElements.m_vfElements16[11]*matOther.m_vfElements.m_vfElements16[14] + m_vfElements.m_vfElements16[15]*matOther.m_vfElements.m_vfElements16[15] );
    }

    /// Multiply this matrix by the specified one
    const Matrix4d & operator*=(const Matrix4d & matOther)
	{
		*this = (*this) * matOther;
        return *this;
    }

    /// Return the vector obtained multiplying this matrix by the specified vector (homogeneous coordinates)
	const Vector3d operator* (const Vector3d &vecOther) const
	{
		const double fW = m_vfElements.m_vfElements16[3]*vecOther.GetX() + m_vfElements.m_vfElements16[7]*vecOther.GetY() + m_vfElements.m_vfElements16[11]*vecOther.GetZ() + m_vfElements.m_vfElements16[15];
		return Vector3d(
			(m_vfElements.m_vfElements16[0]*vecOther.GetX() + m_vfElements.m_vfElements16[4]*vecOther.GetY() + m_vfElements.m_vfElements16[8]*vecOther.GetZ() + m_vfElements.m_vfElements16[12]) / fW,
			(m_vfElements.m_vfElements16[1]*vecOther.GetX() + m_vfElements.m_vfElements16[5]*vecOther.GetY() + m_vfElements.m_vfElements16[9]*vecOther.GetZ() + m_vfElements.m_vfElements16[13]) / fW,
			(m_vfElements.m_vfElements16[2]*vecOther.GetX() + m_vfElements.m_vfElements16[6]*vecOther.GetY() + m_vfElements.m_vfElements16[10]*vecOther.GetZ() + m_vfElements.m_vfElements16[14]) / fW );
    }

    /// Write out this matrix on the specified stream
    void print(std::ostream& out) const {
        out << m_vfElements.m_vfElements16[0] << " " << m_vfElements.m_vfElements16[4] << " " << m_vfElements.m_vfElements16[8] << " " << m_vfElements.m_vfElements16[12] << "\n" <<
               m_vfElements.m_vfElements16[1] << " " << m_vfElements.m_vfElements16[5] << " " << m_vfElements.m_vfElements16[9] << " " << m_vfElements.m_vfElements16[13] << "\n" <<
               m_vfElements.m_vfElements16[2] << " " << m_vfElements.m_vfElements16[6] << " " << m_vfElements.m_vfElements16[10] << " " << m_vfElements.m_vfElements16[14] << "\n" <<
               m_vfElements.m_vfElements16[3] << " " << m_vfElements.m_vfElements16[7] << " " << m_vfElements.m_vfElements16[11] << " " << m_vfElements.m_vfElements16[15] << std::endl;
    }

    // ***************************************************************************************************
    // *** Class members *********************************************************************************
private:
	union
	{
		double m_vfElements16[16];
		double m_vfElements44[4][4];
	} m_vfElements;
}; /* Matrix4d */

#endif
