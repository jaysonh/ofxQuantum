/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ofxQuantum.h
//
//  Created by Jayson Haebich, 2016, www.jaysonh.com
//
//  ofxQuantum is the quantum simulator object, it provides random numbers to the quantum register and gate functions
//  The class connects to a Quantum State Processing Unit (QSPU) which creates seeds for the random number generator
//  based on the quantum effects of decay of a radioactive isotope
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef OFXQUANTUM_H
#define OFXQUANTUM_H

#include "ofxQuantumRegister.h"
#include "QuantumSeedUnit.h"

#include "ofThread.h"

#define TIME_BETWEEN_SEED_UPDATES 5.0

class ofxQuantum : public ofThread
{
public:
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Public Functions
    //////////////////////////////////////////////////////////////////////////////////////////
    
    // Constructor
    ofxQuantum();
    
    // Destructor
    ~ofxQuantum();
    
    // Initialise the quantum simulator
    void  init();
    
    // Get a random number from the simulator
    float getRandom();
    
    // Thread running in background
    void threadedFunction();
    
    // Get the current seed being used by the Quantum Simulator
    long long getSeed();
    
private:
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Private Variables
    //////////////////////////////////////////////////////////////////////////////////////////
    
    long            mSeed;              // Current seed being used
    
    bool            mThreadRunning;     // Is the thread running
    QuantumSeedUnit mSeedUnit;          // Seed unit object that connects to external Quantum State Processing Unit (QSPU)
    float           mLastTimeChecked;   // Times since last checked for a new seed from QSPU
};


#endif 
