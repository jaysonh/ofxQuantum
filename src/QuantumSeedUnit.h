/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  QuantumSeedUnit.h
//
//
//  Created by Jayson Haebich, 2016, www.jaysonh.com
//
//  QuantumSeedUnit is a class which connects to the Quantum Seed Processing Unit (QSPU) which is a hardware device that 
//  generates seed numbers to be used in a random number generator, these seeds are created by detecting the decay of 
//  a radioactive particle
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef QUANTUM_SEED_UNIT_H
#define QUANTUM_SEED_UNIT_H

// Includes
#include <string>
#include <iostream>
#include <stdio.h>   /* Standard input/output definitions */
#include <string.h>  /* String function definitions */
#include <unistd.h>  /* UNIX standard function definitions */
#include <fcntl.h>   /* File control definitions */
#include <errno.h>   /* Error number definitions */
#include <termios.h> /* POSIX terminal control definitions */
#include <thread>         // std::thread
#include "Poco/Thread.h"
#include "Poco/Runnable.h"
#include <vector>
#include <dirent.h>

// serial error codes
#define SERIAL_NO_DATA 	-2
#define SERIAL_ERROR	-1

// Transmission rate for connection to QSPU
#define BAUD_RATE       9600

using namespace std;

class QuantumSeedUnit
{
    
public:
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Public methods
    //////////////////////////////////////////////////////////////////////////////////////////

    // Constructor
    QuantumSeedUnit();

    // Destructor
    ~QuantumSeedUnit();
    
    // Initialise the QSPU
    void init();

    // Connect to the QSPU
    bool connectArduino();
    
    // Update the QSPU in the background
    void updateAsThread();
    
    // Get the current seed
    long  getSeed();
    
    // Close the connection to the QSPU
    void  close();
    
    // Check if the QSPU is connected
    bool  isConnected();

private:
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // Private methods
    //////////////////////////////////////////////////////////////////////////////////////////

    // Connect to a serial device
    bool connect(int indx, int baudRate);

    //////////////////////////////////////////////////////////////////////////////////////////
    // Private Variables
    //////////////////////////////////////////////////////////////////////////////////////////
 
    std::thread * 			  mThread;			// Thread object
    std::vector <std::string> mDeviceList;	    // List of available serial devices
    unsigned long int         mSeed;			// Current seed
    int                       mConnection;		// Which serial port to connect to
    bool                      mConnected;		// Connection status
    
};

#endif
