/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ofxQuantumRegister
//
//
//  Created by Jayson Haebich, 2016, www.jaysonh.com
//
//  ofxQuantumRegister is a class that describes a quantum register, which is a set of a quantum bits that can have
//  quantum gate functions applied to it. This code is based on work done by Matthew Hayward https://quantum-algorithms.herokuapp.com/
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef OFXQUANTUMREGISTER_H
#define OFXQUANTUMREGISTER_H

// Includes
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "Complex.h"
#include "ofxQuantum.h"
#include "ofxCv.h"


// Forward declarations
class ofxQuantum;
class ofxQuantumBit;

class ofxQuantumRegister
{
public:
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Public Functions
    //////////////////////////////////////////////////////////////////////////////////////////
    
    // Constructors.  Size is the number of bits in of our register
    // Quantum register must have a reference to an ofxQuantum object in order to measure the quantum state of a bit
    ofxQuantumRegister();
    ofxQuantumRegister(unsigned long long int size, ofxQuantum *quantumSim);
    ofxQuantumRegister(const ofxQuantumRegister &);
    ~ofxQuantumRegister();
    
    // Measures our quantum register, and returns the decimal and interpretation of the bit string measured.
    unsigned long long int decimalMeasure();
    
    // Prints out all the information about this quantum register
    // When verbose != 0 we return every value, when verbose = 0 we return only probability amplitudes which differ from 0.
    // WARNING, in the case of larger register sizes this can print an incredibly large amount of information!
    void printInfo(int verbose) const;
    
    // Set state of the qubits using the arrays of complex amplitudes.
    void setState(Complex *new_state);
    
    // Set the state to an equal superposition of all possible states between 0 and number inclusive.
    void setAverage(unsigned long long int number);
    
    // Normalize the state amplitudes.
    void norm();
    
    //Get the probability of a given state.
    Complex getProb(unsigned long long int state) const;
    
    //Return the size of the register.
    int size() const;
    
    // Measure the bit at a given indx
    int measureBit(unsigned long long int bitIndx);
    
    // Apply gates to register, see https://en.wikipedia.org/wiki/Quantum_gate for more info
    void applyGateX(    unsigned long long int bit );
    void applyGateY(    unsigned long long int bit );
    void applyGateZ(    unsigned long long int bit );
    void applyGateHad(  unsigned long long int bit );
    void applyGateCNOT( unsigned long long int bit, int controlBitVal );
    void applyGateToff( unsigned long long int bit, int controlBitVal1, int controlBitVal2 );
    void applyToStates( cv::Mat *result );
    
    // Get the number of states in the register
    unsigned long long int getNumStates();
    
    // Get the complex number represenation of a state
    Complex   getState(int stateIndx);
    
private:
    
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Private Functions
    //////////////////////////////////////////////////////////////////////////////////////////
    
    // Expands matrices
    cv::Mat kron( cv::Mat & A, cv::Mat & B);
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Private Variables
    //////////////////////////////////////////////////////////////////////////////////////////
    
    Complex    *           mState;       // Complex number states in our register
    ofxQuantum *           mQuantumSim;  // Reference to quantum simulator
    unsigned long long int mRegSize;     // Size of the register
    unsigned long long int mNumStates;   // Number of states in this register, equals 2 ^ mRegSize
    short       **         mBits;        // Binary representation of each state
    
};

#endif
