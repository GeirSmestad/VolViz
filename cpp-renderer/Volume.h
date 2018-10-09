#ifndef __VOLUME_H__
#define __VOLUME_H__

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>
#include <Vector3d.h>

#include <memory.h>

using std::vector;

/// Define how to groups of bytes should be interpreted
#define DATASET_LITTLE_ENDIAN

/// Macro for converting couples of bytes (char) in an integer
#ifdef DATASET_LITTLE_ENDIAN
#define BYTE2INT(b0, b1) \
    static_cast<int>(static_cast<unsigned char>(b1) << 8) + static_cast<unsigned char>(b0);
#else
#define BYTE2INT(b0, b1) \
    static_cast<int>(static_cast<unsigned char>(b0) << 8) + static_cast<unsigned char>(b1);
#endif


/**
 * Simple linear volume class which supports loading from DAT files.
 * Voxel values rescaled in [0,1] range.
 * There is no need for double precision floating point since the values in the
 *  datasets (for these assignemnts) are in the range [0, 4096].
 */
class Volume
{

    // ********************************************************************************************************
    // *** Basic methods **************************************************************************************
public:
    /// Default constructor.
    Volume() :
        m_width(0), m_height(0), m_depth(0), m_sliceSize(0), m_voxelNum(0), m_voxelData(NULL), m_gradients(NULL), m_gradientMagnitudes(NULL) {
    }

    /// Create a Volume loading data from the specified file
    Volume(const std::string &strFilename) :
        m_width(0), m_height(0), m_depth(0), m_sliceSize(0), m_voxelNum(0), m_voxelData(NULL), m_gradients(NULL), m_gradientMagnitudes(NULL) {
		loadVolumeDat(strFilename);
    }

    /// Copy constructor
    Volume(const Volume& other) :
        // TODO: This copy constructor does not consider gradients and gradient magnitude, so these should be implemented
        // if you ever want to use the copy constructor.
        m_width(other.m_width), m_height(other.m_height), m_depth(other.m_depth),
        m_sliceSize(other.m_sliceSize), m_voxelNum(other.m_voxelNum),
        m_voxelData(new float[other.m_voxelNum]) {
        memcpy(m_voxelData, other.m_voxelData, m_voxelNum * sizeof(float));
    }

    /// Assignment operator
    Volume& operator=(const Volume& other) {
        Volume tmp(other);
        this->swap(tmp);
    }

    /// Swap
    void swap(Volume& other) {
        std::swap(m_width, other.m_width);
        std::swap(m_height, other.m_height);
        std::swap(m_depth, other.m_depth);
        std::swap(m_sliceSize, other.m_sliceSize);
        std::swap(m_voxelNum, other.m_voxelNum);
        std::swap(m_voxelData, other.m_voxelData); // Just swap the pointers, not the whole data!
        std::swap(m_gradients, other.m_gradients);
        std::swap(m_gradientMagnitudes, other.m_gradientMagnitudes);
    }

    /// Destructor
    ~Volume(void) {
        delete [] m_voxelData;
        delete[] m_gradientMagnitudes;
    }

    // ********************************************************************************************************
    // *** Public methods *************************************************************************************
public:
    /// Return Volume's width.
    int getWidth() const { return m_width; }

    /// Return Volume's height.
    int getHeight() const { return m_height; }

    /// Return volume's depth.
    int getDepth() const { return m_depth; }

    /// Return the total number of voxels.
    int getVoxelNum() const { return m_voxelNum; }

    /// Examine the dimensions of the dataset and return the factor the dataset must be scaled by to make the longest
    /// dimension equal to 1.0
    float getScalingFactor() const {
        if (getWidth() >= getHeight() && getWidth() >= getDepth()) {
            // Width highest
            return (float)getWidth();
        } else if (getHeight() >= getWidth() && getHeight() >= getDepth()) {
            // Height highest
            return (float)getHeight();
        } else if (getDepth() >= getWidth() && getDepth() >= getHeight()) {
            // Depth highest
            return (float)getDepth();
        } else {
            // This should not happen if my math is correct. If it is not, crash.
            throw -1;
        }

    }

    /** Return a reference to the value of the voxel at the specified position
     *  NOTE: you are not supposed to modify the original data! */
    float& getVoxel(int x, int y, int z) {
        return m_voxelData[m_sliceSize * z + m_width * y + x];
    }

    /// Return the value of the voxel at the specified position
    const float& getVoxel(int x, int y, int z) const {
        return m_voxelData[m_sliceSize * z + m_width * y + x];
    }

    /** Return a pointer to the voxel values
     *  NOTE: you are not supposed to modify the original data! */
    float *getData() { return m_voxelData; }

    /// Return a (const) pointer to the voxel values
    const float *getData() const { return m_voxelData; }


    /// Load a dataset from the specified file. Return true if the dataset has been loaded successfully.
    bool loadVolumeDat(const std::string & strFilename) {

        std::cout << "- Loading file \"" << strFilename << "\" ... " << std::endl;

        // Try to open the file
        std::fstream fileIn(strFilename.c_str(), std::ifstream::in | std::ifstream::binary);
        if(!fileIn.is_open()) {
            std::cerr << "+ Error opening the file." << std::endl;
            return false;
        }

        // Read the header
        std::cout << "- Reading file's header ..." << std::endl;
        char buffer[2];

        fileIn.read(buffer, 2);
        m_width = BYTE2INT(buffer[0], buffer[1]);
        fileIn.read(buffer, 2);
        m_height = BYTE2INT(buffer[0], buffer[1]);
        fileIn.read(buffer, 2);
        m_depth = BYTE2INT(buffer[0], buffer[1]);

        m_sliceSize = m_width * m_height;
        m_voxelNum = m_sliceSize * m_depth;

        std::cout << "- Dataset dimensions: " << m_width << "x" << m_height << "x" << m_depth << std::endl;

        // Allocate memory to store the dataset values
        if (m_voxelData) // If previous data is present, get rid of it
            delete m_voxelData;
        m_voxelData = new float[m_voxelNum];

        // Read the rest of the file
        std::cout << "- Reading voxel values ..." << std::endl;

        // Scan in voxel data from dataset
        for (int z = 0 ; z < m_depth; z++) {
            for (int y = 0 ; y < m_height; y++) {
                for (int x = 0 ; x < m_width; x++) {

                    if (fileIn.eof()) {
                        printf("Reached end of data file prematurely. Dataset may be corrupted. End reached at coordinates (%d, %d, %d).\n",
                               x, y, z);
                        return false;
                    }

                    fileIn.read(buffer, 2);
                    int thisVoxel = BYTE2INT(buffer[0], buffer[1]);
                    m_voxelData[m_sliceSize * z + m_width * y + x] = thisVoxel / 4095.0; // Add voxel data to array, scaling to [0,1].

                }
            }
        }

        std::cout << "Calculating gradients." << std::endl << std::endl;

        calculateGradients(0);

        calculateHistogram();

        std::cout << "Done parsing data file." << std::endl << std::endl;



        fileIn.close();

        return true;
    } /* loadVolumeDat() */


    /* Utility methods for interpolation etc. */

    /// Gets the closest voxel to the specified position
    const float& getVoxelClosest(float x, float y, float z) const {
        // TODO: Not sure if this is correct, we are just rounding to nearest in each dimension.
        int xVal = (int)floor(x + 0.5);
        int yVal = (int)floor(y + 0.5);
        int zVal = (int)floor(z + 0.5);

        // Handle out of bounds errors (usually just off-by-one)
        if (xVal >= m_width) {
            //std::cout << "Handled out of bounds error in X: " << xVal << "," << yVal << "," << zVal
            //          << " / " << x << "," << y << "," << z << std::endl;
            xVal = m_width-1;
        }
        if (yVal >= m_height) {
            //std::cout << "Handled out of bounds error in Y: " << xVal << "," << yVal << "," << zVal
            //             << " / " << x << "," << y << "," << z << std::endl;
            yVal = m_height-1;
        }
        if (zVal >= m_depth) {
            //std::cout << "Handled out of bounds error in Z: " << xVal << "," << yVal << "," << zVal
            //             << " / " << x << "," << y << "," << z << std::endl;
            zVal = m_depth-1;
        }

        //std::cout << "Outputting voxel at " << xVal << ", " << yVal << ", " << zVal << std::endl;

        return m_voxelData[m_sliceSize * zVal + m_width * yVal + xVal]; // Unsure if this is right. Is it?
    }

    /// Gets a voxel value for the specified coordinates, using trilinear interpolation
    const float& getVoxelTrilinear(float x, float y, float z) const {
        // Handle out of bounds errors (usually just off-by-one, occurrence indicates imprecise programming elsewhere)
        // Should really attack the program to ensure this type of error recovery isn't necessary.
        if (x >= m_width-1) {
            //std::cout << "Handled out of bounds error in X: " << x << "," << y << "," << z << std::endl;
            x = m_width-1;
        }
        if (y >= m_height-1) {
            //std::cout << "Handled out of bounds error in Y: " << x << "," << y << "," << z << std::endl;
            y = m_height-1;
        }
        if (z >= m_depth-1) {
            //std::cout << "Handled out of bounds error in Z: " << x << "," << y << "," << z << std::endl;
            z = m_depth-1;
        }
        if (x < 0) {
            x = 0;
        }
        if (y < 0) {
            y = 0;
        }
        if (z < 0) {
            z = 0;
        }

        // Note: Getting the coordinate axis directions right is essential.

        // Define corner values for interpolation.
        float p000 = getVoxel((int)floor(x), (int)floor(y), (int)floor(z));
        float p100 = getVoxel((int)ceil(x), (int)floor(y), (int)floor(z));
        float p101 = getVoxel((int)ceil(x), (int)floor(y), (int)ceil(z));
        float p001 = getVoxel((int)floor(x), (int)floor(y), (int)ceil(z));

        float p010 = getVoxel((int)floor(x), (int)ceil(y), (int)floor(z));
        float p110 = getVoxel((int)ceil(x), (int)ceil(y), (int)floor(z));
        float p111 = getVoxel((int)ceil(x), (int)ceil(y), (int)ceil(z));
        float p011 = getVoxel((int)floor(x), (int)ceil(y), (int)ceil(z));

        // Trilinearly interpolate.
        // See http://en.wikipedia.org/wiki/Trilinear_interpolation#Method

        float xd = x-(int)floor(x);
        float yd = y-(int)floor(y);
        float zd = z-(int)floor(z);

        float c00 = p000*(1-xd) + p100 * xd;
        float c10 = p010*(1-xd) + p110 * xd;
        float c01 = p001*(1-xd) + p101 * xd;
        float c11 = p011*(1-xd) + p111 * xd;

        float c0 = c00 * (1-yd) + c10 * yd;
        float c1 = c01 * (1-yd) + c11 * yd;

        float c = c0 * (1-zd) + c1 * zd;

        return c;
    }

    vector<float> GetHistogram() {
        return m_histogram;
    }

    /// Precomputes the gradients for the volume. Assumes that the volume has been loaded.
    /// calculationMethod = 0: Central differences approximation
    /// calculationMethod = 1: Next neighbor approximation (not implemented)
    void calculateGradients(int calculationMethod) {
        // Initialize gradient array, deleting old data if present
        if (m_gradients) {
            delete [] m_gradients;
        }


        if (m_gradientMagnitudes) {
            delete [] m_gradientMagnitudes;
        }

        m_gradients = new Vector3d[m_voxelNum];
        m_gradientMagnitudes = new double[m_voxelNum];

        // Calculate gradients and gradient magnitudes.

        for (int z = 0 ; z < m_depth ; z++) {
            for (int y = 0 ; y < m_height ; y++) {
                for (int x = 0 ; x < m_width ; x++) {
                    // std::cout << "Calculating voxel at " << x << "," << y << "," << z << std::endl;
                    if (x == 0 || y == 0 || z == 0 ||
                        x == m_width-1 || y == m_height-1 || z == m_depth-1) {
                        // Edge or corner voxel, gradient is not defined. Set to null vector.
                        m_gradients[m_sliceSize * z + m_width * y + x] = Vector3d(0,0,0);
                    } else if (calculationMethod == 0){
                        // Calculate gradient using central differences approximation

                        float xDifference = (getVoxel(x+1, y, z) - getVoxel(x-1, y, z)) * 0.5;
                        float yDifference = (getVoxel(x, y+1, z) - getVoxel(x, y-1, z)) * 0.5;
                        float zDifference = (getVoxel(x, y, z+1) - getVoxel(x, y, z-1)) * 0.5;

                        Vector3d eachVector(xDifference, yDifference, zDifference);

                        m_gradients[m_sliceSize * z + m_width * y + x] = eachVector;

                        // std::cout << "Set gradient vector to " << xDifference << ", " << yDifference << ", " << zDifference << std::endl;

                    } else if (calculationMethod == 1) {
                        // Calculate gradient using next neighbor approximation
                        // TODO: Not implemented
                    } else {
                        // Illegal argument, crash.
                        throw -1;
                    }

                    // Precompute gradient magnitude
                    m_gradientMagnitudes[m_sliceSize * z + m_width * y + x] = m_gradients[m_sliceSize * z + m_width * y + x].GetMagnitude();

                    // std::cout << "Gradient magnitude is " << m_gradientMagnitudes[m_sliceSize * z + m_width * y + x] << std::endl;
                }
            }
        }
    }

    /// Get the gradient at a certain point in the dataset
    const Vector3d& getGradient(int x, int y, int z) const {
        return m_gradients[m_sliceSize * z + m_width * y + x];
    }

    /// Get the gradient at a certain point in the dataset (floating point argument)
    const Vector3d& getGradient(float x, float y, float z) const {
        if (x >= m_width-1) {
            //std::cout << "Handled out of bounds error in X: " << x << "," << y << "," << z << std::endl;
            x = m_width-1;
        }
        if (y >= m_height-1) {
            //std::cout << "Handled out of bounds error in Y: " << x << "," << y << "," << z << std::endl;
            y = m_height-1;
        }
        if (z >= m_depth-1) {
            //std::cout << "Handled out of bounds error in Z: " << x << "," << y << "," << z << std::endl;
            z = m_depth-1;
        }
        if (x < 0) {
            x = 0;
        }
        if (y < 0) {
            y = 0;
        }
        if (z < 0) {
            z = 0;
        }

        return m_gradients[m_sliceSize * (int)z + m_width * (int)y + (int)x];
    }

    /// Get the gradient at a certain point in the dataset, using trilinear interpolation.
    const Vector3d& getGradientTrilinear(float x, float y, float z) const {
        if (x >= m_width-1) {
            //std::cout << "Handled out of bounds error in X: " << x << "," << y << "," << z << std::endl;
            x = m_width-1;
        }
        if (y >= m_height-1) {
            //std::cout << "Handled out of bounds error in Y: " << x << "," << y << "," << z << std::endl;
            y = m_height-1;
        }
        if (z >= m_depth-1) {
            //std::cout << "Handled out of bounds error in Z: " << x << "," << y << "," << z << std::endl;
            z = m_depth-1;
        }
        if (x < 0) {
            x = 0;
        }
        if (y < 0) {
            y = 0;
        }
        if (z < 0) {
            z = 0;
        }

        Vector3d result;

        Vector3d v000 = m_gradients[m_sliceSize * (int)floor(z) + m_width * (int)floor(y) + (int)floor(x)];
        Vector3d v100 = m_gradients[m_sliceSize * (int)floor(z) + m_width * (int)floor(y) + (int)ceil(x)];
        Vector3d v101 = m_gradients[m_sliceSize * (int)ceil(z) + m_width * (int)floor(y) + (int)ceil(x)];
        Vector3d v001 = m_gradients[m_sliceSize * (int)ceil(z) + m_width * (int)floor(y) + (int)floor(x)];

        Vector3d v010 = m_gradients[m_sliceSize * (int)floor(z) + m_width * (int)ceil(y) + (int)floor(x)];
        Vector3d v110 = m_gradients[m_sliceSize * (int)floor(z) + m_width * (int)ceil(y) + (int)ceil(x)];
        Vector3d v111 = m_gradients[m_sliceSize * (int)ceil(z) + m_width * (int)ceil(y) + (int)ceil(x)];
        Vector3d v011 = m_gradients[m_sliceSize * (int)ceil(z) + m_width * (int)ceil(y) + (int)floor(x)];

        // Trilinearly interpolate.
        // See http://en.wikipedia.org/wiki/Trilinear_interpolation#Method

        float xd = x-(int)floor(x);
        float yd = y-(int)floor(y);
        float zd = z-(int)floor(z);

        // Interpolate trilinear for each dimension. Should probably do this with a helper function, but can't
        // be bothered.
        float p000 = v000.GetX();
        float p100 = v100.GetX();
        float p101 = v101.GetX();
        float p001 = v001.GetX();

        float p010 = v010.GetX();
        float p110 = v110.GetX();
        float p111 = v111.GetX();
        float p011 = v011.GetX();

        float c00 = p000*(1-xd) + p100 * xd;
        float c10 = p010*(1-xd) + p110 * xd;
        float c01 = p001*(1-xd) + p101 * xd;
        float c11 = p011*(1-xd) + p111 * xd;

        float c0 = c00 * (1-yd) + c10 * yd;
        float c1 = c01 * (1-yd) + c11 * yd;

        float c = c0 * (1-zd) + c1 * zd;

        result.SetX(c);

        p000 = v000.GetY();
        p100 = v100.GetY();
        p101 = v101.GetY();
        p001 = v001.GetY();

        p010 = v010.GetY();
        p110 = v110.GetY();
        p111 = v111.GetY();
        p011 = v011.GetY();

        c00 = p000*(1-xd) + p100 * xd;
        c10 = p010*(1-xd) + p110 * xd;
        c01 = p001*(1-xd) + p101 * xd;
        c11 = p011*(1-xd) + p111 * xd;

        c0 = c00 * (1-yd) + c10 * yd;
        c1 = c01 * (1-yd) + c11 * yd;

        c = c0 * (1-zd) + c1 * zd;

        result.SetY(c);

        p000 = v000.GetZ();
        p100 = v100.GetZ();
        p101 = v101.GetZ();
        p001 = v001.GetZ();

        p010 = v010.GetZ();
        p110 = v110.GetZ();
        p111 = v111.GetZ();
        p011 = v011.GetZ();

        c00 = p000*(1-xd) + p100 * xd;
        c10 = p010*(1-xd) + p110 * xd;
        c01 = p001*(1-xd) + p101 * xd;
        c11 = p011*(1-xd) + p111 * xd;

        c0 = c00 * (1-yd) + c10 * yd;
        c1 = c01 * (1-yd) + c11 * yd;

        c = c0 * (1-zd) + c1 * zd;

        result.SetZ(c);

        return result;
    }


    /// Get the gradient magnitude at a certain point in the dataset
    const double& getGradientMagnitude(float x, float y, float z) const {
        // Handle out of bounds errors (usually just off-by-one, occurrence indicates imprecise programming elsewhere)
        if (x >= m_width-1) {
            //std::cout << "Handled out of bounds error in X: " << x << "," << y << "," << z << std::endl;
            x = m_width-1;
        }
        if (y >= m_height-1) {
            //std::cout << "Handled out of bounds error in Y: " << x << "," << y << "," << z << std::endl;
            y = m_height-1;
        }
        if (z >= m_depth-1) {
            //std::cout << "Handled out of bounds error in Z: " << x << "," << y << "," << z << std::endl;
            z = m_depth-1;
        }
        if (x < 0) {
            x = 0;
        }
        if (y < 0) {
            y = 0;
        }
        if (z < 0) {
            z = 0;
        }

        return m_gradientMagnitudes[m_sliceSize * (int)z + m_width * (int)y + (int)x];
    }

    /// Get the gradient magnitude at a certain point in the dataset, using trilinear interpolation.
    const double& getGradientMagnitudeTrilinear(float x, float y, float z) const {
        // Handle out of bounds errors (usually just off-by-one, occurrence indicates imprecise programming elsewhere)
        if (x >= m_width-1) {
            //std::cout << "Handled out of bounds error in X: " << x << "," << y << "," << z << std::endl;
            x = m_width-1;
        }
        if (y >= m_height-1) {
            //std::cout << "Handled out of bounds error in Y: " << x << "," << y << "," << z << std::endl;
            y = m_height-1;
        }
        if (z >= m_depth-1) {
            //std::cout << "Handled out of bounds error in Z: " << x << "," << y << "," << z << std::endl;
            z = m_depth-1;
        }
        if (x < 0) {
            x = 0;
        }
        if (y < 0) {
            y = 0;
        }
        if (z < 0) {
            z = 0;
        }

        // Define corner values for interpolation.
        float p000 = m_gradientMagnitudes[m_sliceSize * (int)floor(z) + m_width * (int)floor(y) + (int)floor(x)];
        float p100 = m_gradientMagnitudes[m_sliceSize * (int)floor(z) + m_width * (int)floor(y) + (int)ceil(x)];
        float p101 = m_gradientMagnitudes[m_sliceSize * (int)ceil(z) + m_width * (int)floor(y) + (int)ceil(x)];
        float p001 = m_gradientMagnitudes[m_sliceSize * (int)ceil(z) + m_width * (int)floor(y) + (int)floor(x)];

        float p010 = m_gradientMagnitudes[m_sliceSize * (int)floor(z) + m_width * (int)ceil(y) + (int)floor(x)];
        float p110 = m_gradientMagnitudes[m_sliceSize * (int)floor(z) + m_width * (int)ceil(y) + (int)ceil(x)];
        float p111 = m_gradientMagnitudes[m_sliceSize * (int)ceil(z) + m_width * (int)ceil(y) + (int)ceil(x)];
        float p011 = m_gradientMagnitudes[m_sliceSize * (int)ceil(z) + m_width * (int)ceil(y) + (int)floor(x)];

        // Trilinearly interpolate.
        // See http://en.wikipedia.org/wiki/Trilinear_interpolation#Method

        float xd = x-(int)floor(x);
        float yd = y-(int)floor(y);
        float zd = z-(int)floor(z);

        float c00 = p000*(1-xd) + p100 * xd;
        float c10 = p010*(1-xd) + p110 * xd;
        float c01 = p001*(1-xd) + p101 * xd;
        float c11 = p011*(1-xd) + p111 * xd;

        float c0 = c00 * (1-yd) + c10 * yd;
        float c1 = c01 * (1-yd) + c11 * yd;

        float c = c0 * (1-zd) + c1 * zd;

        return c;
    }

    // ********************************************************************************************************
    // *** Class members **************************************************************************************
private:
	int m_width;
	int m_height;
	int m_depth;
	int m_sliceSize;
    int m_voxelNum;
	float *m_voxelData;

    Vector3d *m_gradients; // Array of gradient vectors
    double *m_gradientMagnitudes; // Array of the magnitude of gradient vectors

    vector<float> m_histogram;

    /// Calculates the histogram for this volume: An array of voxel value occurence by voxel value.
    /// Assumes voxel data has been loaded when called.
    void calculateHistogram() {
        m_histogram = vector<float>(0);


        // Initialize histogram vector to 0
        for (int i = 0 ; i < 200 ; i++) {
            m_histogram.push_back(0);
        }

        // Calculate histogram
        for (int i = 0 ; i < m_voxelNum ; i++) {
            float sampleValue = m_voxelData[i]*200;

            // sampleValue is [0, 100], we want to index the array in the range [0, 199].
            int index = floor(sampleValue + 0.5);

            if (index > 199) {
                index = 99;
            }

            // Ignore zero values
            //if (index != 0) {
                m_histogram[index]++;
            //}


        }


        for (int i = 0 ; i < m_histogram.size() ; i++) {
            // Logarithmically scale histogram - there are often very sharp peaks in the histogram, and we want to dampen these.
            // Alternatively scale the histogram using a root, this might make most sense since dataset is 3D.
            float base = 1.1;

            // m_histogram[i] = log((float)m_histogram[i])/ log(base);
            // m_histogram[i] = sqrt((float)m_histogram[i]);
            m_histogram[i] = pow((float)m_histogram[i],(float)(1/3.0));


        }

    }
};

#endif /* __VOLUME_H__ */
