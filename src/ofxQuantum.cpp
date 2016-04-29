/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ofxQuantum.cpp
//
//  Created by Jayson Haebich, 2016 www.jaysonh.com
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ofxQuantum.h"


////////////////////////////////////////////////////
// Default constructor                            //
////////////////////////////////////////////////////
ofxQuantum::ofxQuantum()
{
    
}

////////////////////////////////////////////////////
// Default destructor                             //
////////////////////////////////////////////////////
ofxQuantum::~ofxQuantum()
{
    mThreadRunning = false;
}

/////////////////////////////////////////////////////
// Initialise simulator                            //
/////////////////////////////////////////////////////
void ofxQuantum::init()
{
    
    printf("Connecting to Quantum Seed Processing Unit\n");
    
    // Connect to quantum seed processing unit (QSPU) via serial
    mSeedUnit.init();
    mSeedUnit.connectArduino();
    
    // Set the random seed
    mLastTimeChecked = ofGetElapsedTimef();
    mSeed            = ofGetSystemTime();
    
    // Seed random number generator with pseudo random seed
    srand( mSeed );
    
    // Start the background thread which updates the QSPU
    startThread(false, false);
}

/////////////////////////////////////////////////////
// Get a random float 0.0-1.0                      //
/////////////////////////////////////////////////////
float ofxQuantum::getRandom()
{
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

///////////////////////////////////////////////////////////////////////////////
// Function that runs in background and checks for new random seed from QSPU //
///////////////////////////////////////////////////////////////////////////////
void ofxQuantum::threadedFunction()
{
    // start
    mThreadRunning = true;
    
    while(mThreadRunning)
    {
        // If the QSPU is connected then check for a new seed
        if(mSeedUnit.isConnected())
        {
            if(ofGetElapsedTimef() - mLastTimeChecked > TIME_BETWEEN_SEED_UPDATES)
            {
                long seed = mSeedUnit.getSeed();
                
                if(seed != mSeed)
                {
                    srand(seed);
                    mSeed=seed;
                }
                
                mLastTimeChecked = ofGetElapsedTimef();
            }
        }
        
        // This thread doesn't need much cpu time so sleep for 200ms
        sleep(200);
    }
}

/////////////////////////////////////////////////////
// Get the current seed being used                 //
/////////////////////////////////////////////////////

long long ofxQuantum::getSeed()
{
    return mSeed;
}
