#ifndef TRANSFER_FUNCTION_H
#define TRANSFER_FUNCTION_H

#include <QObject>
#include <cassert>
#include <cmath>
#include <fstream>
#include <iostream>
#include <Vector3d.h>
#include <vector>

using std::vector;

/// Maps intensity values between 0. and 1. to a color and a transparency value
class TransferFunction : public QObject {

    Q_OBJECT

    // *********************************************************************************************
    // *** Basic methods ***************************************************************************
public:
    /// Constructor
    TransferFunction() {
        Reset();
    } /* Constructor */


    // *********************************************************************************************
    // *** Signals *********************************************************************************
signals:
    /// Emitted when the transfer function change
    void tfChanged();


    // *********************************************************************************************
    // *** Public methods **************************************************************************
public:

    //The word 'sample' refers to an intensity value (between 0. and 1.) whose color and
    // alpha values has been explicitly specified

    /// Reset the transfer function to the initial state (3 samples, black-red-white, full opacity)
    void Reset() {
        // TODO: clear an reinitialize all the class members you need
        samples = vector<double>(0);

        // Sample value 0: Black, transparent
        samples.push_back(0.0);
        samples.push_back(0.0); // R
        samples.push_back(0.0); // G
        samples.push_back(0.0); // B
        samples.push_back(0.0); // Alpha

        // Sample value 1: White, opaque
        samples.push_back(1.0);
        samples.push_back(1.0);
        samples.push_back(1.0);
        samples.push_back(1.0);
        samples.push_back(1.0);

        DiscretizeSamples(100);
    }

    /// Debug method: Print contents of samples vector.
    void printContents() {

        std::cout << "Vector contents: ";

        for (int i = 0 ; i < samples.size() ; i++) {
            if (i != 0 && i % 5 == 0) {
                std::cout << " || ";
            }
            std::cout << " " << samples.at(i);
        }
        std::cout << std::endl;
    }

    /// Debug method: Print contents of discretized samples vector.
    void printDiscretizedContents() {
        std::cout << "Printing discretized vector contents of size " << discretizedSamples.size() << std::endl;

        for (int i = 0 ; i < discretizedSamples.size() ; i++) {
            if (i != 0 && i % 5 == 0) {
                std::cout << std::endl;
            }
            std::cout << " " << discretizedSamples.at(i);
        }
        std::cout << std::endl;
    }

    /// Add a sample to the current transfer function
    void AddSample(double sample, double r, double g, double b, double alpha = 1.) {
        AddSample(sample, Vector3d(r,g,b), alpha);
    }

    /// Add a sample to the current transfer function
    void AddSample(double sample, const Vector3d& color, double alpha = 1.) {
        assert(sample >= 0 && sample <= 1.);
        assert(color.GetX() >= 0. && color.GetX() <= 1.);
        assert(color.GetY() >= 0. && color.GetY() <= 1.);
        assert(color.GetZ() >= 0. && color.GetZ() <= 1.);
        assert(alpha >= 0. && alpha <= 1.);

        // Find the correct spot in the sorted sample vector and insert the sample
        // Assume that sample vector always begins with 0 and ends with 1

        for (int i = 0 ; i <= samples.size()-10 ; i+= 5) {
            double prevSample = samples.at(i);
            double nextSample = samples.at(i+5);

            if (sample >= prevSample && sample <= nextSample) {
                // Found the correct insertion point. (Condition guaranteed to be true at some point)
                // assuming 0 and 1 are already in samples vector and 'sample' in [0, 1]

                // std::cout << "Inserting sample at " << (i+5) << std::endl;

                vector<double>::iterator targetPosition;
                targetPosition = samples.begin()+i+5;

                // Insert new sample into samples vector
                vector<double> elementsToAdd(0);
                elementsToAdd.push_back(sample);
                elementsToAdd.push_back(color.GetX());
                elementsToAdd.push_back(color.GetY());
                elementsToAdd.push_back(color.GetZ());
                elementsToAdd.push_back(alpha);

                samples.insert(targetPosition, elementsToAdd.begin(), elementsToAdd.end());

                break;
            }
        }

        DiscretizeSamples(100);

         emit tfChanged(); // Calling this caused a crash, for some reason. Maybe there isn't a corresponding slot?
    }

    /// Compute the color corresponding the the specified sample
    Vector3d GetColor(double sample) {
        assert(sample >= 0);
        assert(sample <= 1.);

        // Assume that the samples have been discretized. For now, just use the closest discretized sample.
        // For better resolution, we could instead interpolate between discretized samples.

        // We use this approach to get constant-time sample color lookup, which improves rendering time.

        int targetIndex = roundToNearest5(sample*discretizedSamples.size());

        // Handle edge case where index is rounded up to the edge of the vector
        if (targetIndex+4 >= discretizedSamples.size()) {
            targetIndex -= 5;
        }

        Vector3d result(discretizedSamples.at(targetIndex+1),
                        discretizedSamples.at(targetIndex+2),
                        discretizedSamples.at(targetIndex+3));

        return result;

    }

    /// Compute the alpha value corresponding the the specified sample
    double GetAlpha(double sample) const {
        assert(sample >= 0 && sample <= 1.);

        int targetIndex = roundToNearest5(sample*discretizedSamples.size());

        // Handle edge case where index is rounded up to the edge of the vector
        if (targetIndex+4 >= discretizedSamples.size()) {
            targetIndex -= 5;
        }

        return discretizedSamples.at(targetIndex+4);
    }

    /// Remove the target sample from the samples list, or do nothing if it doesn't exist.
    /// Warning: This does not implicitly re-discretize the sample list, you have to do this
    /// manually.
    void RemoveSample(double sample) {
        if (sample == 0 || sample == 1) {
            // Do not allow removal of the edge samples, they ensure TF consistency
            return;
        }

        for (int i = samples.size()-5 ; i >= 0 ; i-= 5) {
            double eachSample = samples.at(i);
            if (eachSample == sample) {
                // Found target sample, remove its five corresponding elements and return
                samples.erase(samples.begin()+i, samples.begin()+i+5);

                return;
            }
        }
    }

    /// Return the closest sample to target. If |target-closest| > threshold, return -1.
    /// The alpha parameter allows an optional constraint on the alpha value of the returned
    /// sample, also subject to threshold. Use alpha=-1 to disregard the alpha value in the search.
    double FindClosestSample(double target, double alpha, double threshold) {
        // Using linear search for simplicity, since this won't be called often.

        for (int i = 0 ; i <= samples.size()-10 ; i+= 5) {
            double prevSample = samples.at(i);
            double nextSample = samples.at(i+5);

            if (target >= prevSample && target <= nextSample) {
                // Target is closest or equal to one of these

                double diffUp = nextSample - target;
                double diffDown = target - prevSample;

                if (diffUp > threshold && diffDown > threshold) {
                    continue;
                }

                if (diffUp < diffDown) {
                    // nextSample is our candidate, return if alpha value matches
                    if (alpha == -1 || abs(alpha - samples.at(i+9)) < threshold ) {
                        return nextSample;
                    }

                    // prevSample is our candidate, return if alpha value matches
                } else if (alpha == -1 || abs(alpha - samples.at(i+4)) < threshold ) {
                    return prevSample;
                }
            }
        }
        return -1;
    }

    /// Update the r, g, b, a values of one of the edge samples. Ignore if sample is not 0 or 1.
    void UpdateEdgeSample(double sample, double red, double green, double blue, double alpha) {
        if (sample == 0) {
            samples[1] = red;
            samples[2] = green;
            samples[3] = blue;
            samples[4] = alpha;
        } else if (sample == 1) {
            int lastSampleStartIndex = samples.size() - 5;
            samples[lastSampleStartIndex + 1] = red;
            samples[lastSampleStartIndex + 2] = green;
            samples[lastSampleStartIndex + 3] = blue;
            samples[lastSampleStartIndex + 4] = alpha;
        }

        DiscretizeSamples(100);
    }

    /// Return reference to transfer function's samples
    const vector<double>* GetSamples() const {
        return &samples;
    }

    /// Assuming a consistent samples vector, discretize this vector across the [0,1] range
    /// using linear interpolations and n samples in total.
    void DiscretizeSamples(int n) {
        discretizedSamples.clear();

        int currentSampleIndex = 0;
        int nextSampleIndex = 5;

        double currentSample = samples.at(currentSampleIndex);
        double nextSample = samples.at(nextSampleIndex);

        double currentR = samples.at(currentSampleIndex+1);
        double nextR = samples.at(nextSampleIndex+1);

        double currentG = samples.at(currentSampleIndex+2);
        double nextG = samples.at(nextSampleIndex+2);

        double currentB = samples.at(currentSampleIndex+3);
        double nextB = samples.at(nextSampleIndex+3);

        double currentAlpha = samples.at(currentSampleIndex+4);
        double nextAlpha = samples.at(nextSampleIndex+4);

        for (int i = 0 ; i < n-1 ; i++) {
            // currentPosition is never greater than 1, there will be no overflow
            float currentPosition = ((float)i)/n;

            if (currentPosition >= nextSample) {
                // std::cout << "Debug: Incremented sample to " << std::endl;
                currentSampleIndex += 5;
                nextSampleIndex += 5;

                currentSample = samples.at(currentSampleIndex);
                nextSample = samples.at(nextSampleIndex);

                // Update the values of the samples we are currently looking at
                currentR = samples.at(currentSampleIndex+1);
                nextR = samples.at(nextSampleIndex+1);

                currentG = samples.at(currentSampleIndex+2);
                nextG = samples.at(nextSampleIndex+2);

                currentB = samples.at(currentSampleIndex+3);
                nextB = samples.at(nextSampleIndex+3);

                currentAlpha = samples.at(currentSampleIndex+4);
                nextAlpha = samples.at(nextSampleIndex+4);
            }

            // Perform linear interpolation of values
            double interpolatedAlpha = currentAlpha + (currentPosition-currentSample)*(nextAlpha-currentAlpha)/(nextSample-currentSample);
            double interpolatedR = currentR + (currentPosition-currentSample)*(nextR-currentR)/(nextSample-currentSample);
            double interpolatedG = currentG + (currentPosition-currentSample)*(nextG-currentG)/(nextSample-currentSample);
            double interpolatedB = currentB + (currentPosition-currentSample)*(nextB-currentB)/(nextSample-currentSample);

            // Ensure that interpolated values are not out of bounds
            if (interpolatedAlpha > 1) {
                interpolatedAlpha = 1;
            }
            if (interpolatedR > 1) {
                interpolatedR = 1;
            }
            if (interpolatedG > 1) {
                interpolatedG = 1;
            }
            if (interpolatedB > 1) {
                interpolatedB = 1;
            }

            if (interpolatedAlpha < 0) {
                interpolatedAlpha = 0;
            }
            if (interpolatedR < 0) {
                interpolatedR = 0;
            }
            if (interpolatedG < 0) {
                interpolatedG = 0;
            }
            if (interpolatedB < 0) {
                interpolatedB = 0;
            }

            // Add interpolated values to vector
            discretizedSamples.push_back(currentPosition);
            discretizedSamples.push_back(interpolatedR);
            discretizedSamples.push_back(interpolatedG);
            discretizedSamples.push_back(interpolatedB);
            discretizedSamples.push_back(interpolatedAlpha);
        }

        // Add final (edge) values to vector
        int lastSampleIndex = samples.size() - 5;
        discretizedSamples.push_back(samples.at(lastSampleIndex));
        discretizedSamples.push_back(samples.at(lastSampleIndex+1));
        discretizedSamples.push_back(samples.at(lastSampleIndex+2));
        discretizedSamples.push_back(samples.at(lastSampleIndex+3));
        discretizedSamples.push_back(samples.at(lastSampleIndex+4));
    }

    // Input and output from disk
    /// Store a transfer function on the specified file
    bool save(std::string filename) const {
        std::ofstream tfOut(filename.c_str());
        if(!tfOut.good()) {
            std::cerr << "Unable to save the transfer function to the specified file." << std::endl;
            return false;
        }

        // We could make this very simple and simply write the samples vector to disk.
        // It could then be loaded back into this class trivially.

        // TODO: save the transfer function to a file according to your own defined format

        tfOut.close();
        return true;
    }


    /// Load a trasnfer function from the specified file
    bool open(std::string filename) {
        std::ifstream tfIn(filename.c_str());
        while(tfIn.good()) {
            // TODO: Read the content of the file accordng to the same format used to save the transfer function
        }
        tfIn.close();

        // TODO: make sure that color and alpha for 0. and 1. are defined

        // emit tfChanged(); // TODO: this causes crash
        return true;
    }

    // *********************************************************************************************
    // *** Class members ***************************************************************************
private:
    // Samples are 5-tuples representing sample position, alpha and r, g, b.

    /* For simplicity we can use a 1-dimensional vector with the  0st, 5th, 10th and so on
       element (modulo 5) denoting the sample value and the next 4 values denoting the stored
       variables. The vector should be sorted. */

       vector<double> samples;

       // We want to discretize the sample space in order to allow sample lookup in O(1) time.
       vector<double> discretizedSamples;




       /// Round argument to nearest int that is a multiple of 5
       static int roundToNearest5(double number) {
           int nearestInt = number < 0.0 ? ceil(number - 0.5) : floor(number + 0.5);

           int remainder = nearestInt % 5;

           if (remainder != 0) {
               if (remainder < 3) {
                   return nearestInt - remainder;
               } else {
                   return nearestInt + (5-remainder);
               }
           }

           return nearestInt;

       }

};

#endif // TRANSFER_FUNCTION_H
