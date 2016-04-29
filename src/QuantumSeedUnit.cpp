/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  QuantumSeedUnit.cpp
//
//  Created by Jayson Haebich, 2016 www.jaysonh.com
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "QuantumSeedUnit.h"


////////////////////////////////////////////////////
// Default constructor                            //
////////////////////////////////////////////////////
QuantumSeedUnit::QuantumSeedUnit()
{
}

////////////////////////////////////////////////////
// Destructor                                     //
////////////////////////////////////////////////////
QuantumSeedUnit::~QuantumSeedUnit()
{
    // Closes serial connection to QSPU
    close();
}

////////////////////////////////////////////////////
// Close Connection                               //
////////////////////////////////////////////////////
void QuantumSeedUnit::close()
{
    printf("Closing serial connection\n");

    // End the background thread and join it together with the main thread so that the application can close cleanly
    if(mThread != NULL)
        mThread->join();
    
    delete mThread;
}

////////////////////////////////////////////////////
// Initalise the QSPU                             //
////////////////////////////////////////////////////
void QuantumSeedUnit::init()
{
    // Empty the list of devices
    mDeviceList.clear();

    // Find all the available serial devices
    DIR *dir;
    dir = opendir("/dev");
    
    string deviceName = "";
    
    mConnection = -1;
    
    // If serial connectios are not available
    if(dir == nullptr)
    {
        std::cout << "ofSerial buildDeviceList(): error listing devices in /dev";
    } 
    // Otherwise find devices
    else
    {
        int deviceCount = 0;

        // Pointer to store current entry
        struct dirent *entry;
        
        // Loop through each serial device
        while((entry = readdir(dir)) != nullptr)
        {
            deviceName = (char *)entry->d_name;
            
            // If it is a serial device
            if( deviceName.substr(0, 4) == "tty." ||
               deviceName.substr(0, 3) == "cu." )
            {
                std::cout << "serial device[" << deviceCount << "]: " << deviceName << std::endl;
                
                // Add to list of devices
                mDeviceList.push_back("/dev/" + deviceName);
                deviceCount++;
            }
            
        }

        // Close the serial devices directory
        closedir(dir);
    }
    
}

////////////////////////////////////////////////////
// Connect to serial device                       //
////////////////////////////////////////////////////
bool QuantumSeedUnit::connect(int indx, int baud)
{
    printf("Connecting to serial port: %i\n", indx);
    
    // If indx is valid
    if( indx <  mDeviceList.size() )
    {
        // Open connection
        mConnection = open( mDeviceList[indx].c_str() , O_RDWR | O_NOCTTY | O_NDELAY);
        
        if (mConnection == -1)
        {
            printf("Unable to open port.\n");
        }
        else
        {
            printf("Port opened.\n");
            
            // Set connection parameters
            struct termios options;
            
            cfsetispeed(&options, B9600);
            cfsetospeed(&options, B9600);
            
            options.c_cflag |= (CLOCAL | CREAD);
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            options.c_cflag &= ~CSIZE;
            options.c_iflag &= (tcflag_t) ~(INLCR | IGNCR | ICRNL | IGNBRK);
            options.c_oflag &= (tcflag_t) ~(OPOST);
            options.c_cflag |= CS8;
            tcsetattr(mConnection, TCSANOW, &options);
            

            // Start the update function running in the background so that it does not block the main loop
            mThread = new std::thread(&QuantumSeedUnit::updateAsThread, this);
        }
    }
    
    // Return true if we connected otherwise return false
    if(mConnection == -1)
        return false;
    else
        return true;
}

////////////////////////////////////////////////////
// Find the arduino device to connect to          //
////////////////////////////////////////////////////
bool QuantumSeedUnit::connectArduino()
{
    mConnected = false;
    
    // Go through all the devices and find the arduino board
    for(int i = 0; i < mDeviceList.size() ; i++)
    {
        // Connect to first arduino device
        if(mDeviceList[i].substr(0,18) == "/dev/cu.usbmodem14")
        {
            mConnected = connect(i, BAUD_RATE);
        }
    }
    
    // Display message on console
    if(mConnected)
    {
        printf("Connected to Quantum Random Seed Processing Unit\n");
    }else{
        printf("Couldn't connect to Quantum Random Seed Processing Unit\n");
    }
    
    return mConnected;
}

////////////////////////////////////////////////////
// Update function that runs in the background    //
////////////////////////////////////////////////////
void QuantumSeedUnit::updateAsThread()
{
    bool threadRunning = true;
    
    int bitIndx = 0;
    
    // This stores our 32 bit random seed (each unsigned char is 8 bits)
    unsigned char tmpByte[4];
    
    // Loop until the thread is closed
    while(threadRunning)
    {
        // Read in some data from the serial connection
        unsigned char tmp;
        int nRead = read(mConnection, &tmp, 1);
        
        // If there is some data to read
        if(nRead > 0  && tmp != '\n')
        {
            // Save the data in our array
            tmpByte[bitIndx] = tmp;
            bitIndx++;
            
            // Once our array is full we can construct our 32 bit seed
            if(bitIndx >= 4)
            {
                // Reset the bit indx for the next iteration
                bitIndx = 0;

                // Pack the 8 bit chars into our 32 bit int
                mSeed =   tmpByte[0] |
                        ( tmpByte[1] << 8)  |
                        ( tmpByte[2] << 16) |
                        ( tmpByte[3] << 24    );
                
            }
        }
        
        // Sleep for one second to allow other processes time to run
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        
    }
}

////////////////////////////////////////////////////
// Test if the QSPU is connected                  //
////////////////////////////////////////////////////
bool QuantumSeedUnit::isConnected()
{
    return mConnected;
}

long  QuantumSeedUnit::getSeed()
{
    return mSeed;
}