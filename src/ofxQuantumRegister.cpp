/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ofxQuantumRegister.cpp
//
//  Created by Jayson Haebich, 2016 www.jaysonh.com
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "ofxQuantumRegister.h"

using namespace std;

////////////////////////////////////////////////////
// Default constructor                            //
////////////////////////////////////////////////////

ofxQuantumRegister::ofxQuantumRegister()
{
    // Empty register
    mRegSize = 0;
    mState   = NULL;
}

////////////////////////////////////////////////////
// Constructor that sets num bits                 //
////////////////////////////////////////////////////

ofxQuantumRegister::ofxQuantumRegister(unsigned long long int numBits, ofxQuantum *quantumSim)
{
    // Store reference to quantum simulator
    mQuantumSim = quantumSim;
    
    // Allocate the states and bits
    mRegSize    = numBits;
    mNumStates  = pow(2, mRegSize);
    mState      = new Complex[mNumStates];
    
    // Create our quantum bits
    mBits = new short*[(int)pow(2,mRegSize)];
    for(int i = 0; i < pow(2,mRegSize);i++)
    {
        mBits[i] = new short[mRegSize];
        
        int mask = 1;
        for(int j = 0; j < mRegSize; j++)
        {
            if ((mask & i) != 0)
                mBits[i][mRegSize-1-j] = 1;
            else
                mBits[i][mRegSize-1-j] = 0;
            mask<<=1;
        }
    }
    
    // Set first state
    mState[0] = Complex(1,0);
    
}

////////////////////////////////////////////////////
// Copy constructor                               //
////////////////////////////////////////////////////

ofxQuantumRegister::ofxQuantumRegister(const ofxQuantumRegister & old)
{
    // Set the size of the register
    mRegSize    = old.mRegSize;
    mNumStates  = pow(2, mRegSize);
    
    mState      = new Complex[mNumStates];
    
    // Copy states from old register
    for (unsigned int i = 0 ; i < mNumStates; i++)
    {
        mState[i] = old.mState[i];
    }
}

////////////////////////////////////////////////////
// Destructor                                     //
////////////////////////////////////////////////////

ofxQuantumRegister::~ofxQuantumRegister()
{
    // If states have been allocated then delete them
    if ( mState ) {
        delete [] mState;
    }
}

////////////////////////////////////////////////////
// Get the probability of a state                 //
////////////////////////////////////////////////////
Complex ofxQuantumRegister::getProb(unsigned long long int state) const
{
    // Throw an error if outside of number of states
    if (state >= mNumStates)
    {
        cout << "Invalid state index " << state << " requested!"
        << endl << flush;
        throw -1;
    }
    // Otherwise return state
    else
    {
        return(mState[state]);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Normalize the probability amplitude, this ensures that the sum of
// the sum of the squares of all the real and imaginary components is
// equal to one.
////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::norm()
{
    double b = 0;
    double f, g;
    
    // Calculate the total size of the register
    for (unsigned long long int i = 0; i < pow(2, mRegSize) ; i++)
    {
        b += pow(mState[i].getReal(), 2) + pow(mState[i].getImag(), 2);
    }
    
    b = pow(b, -.5);
    
    // Set the new states from the register
    for (unsigned long long int i = 0; i < pow(2, mRegSize) ; i++) {
        f = mState[i].getReal() * b;
        g = mState[i].getImag() * b;
        mState[i].set(f, g);
    }
}

////////////////////////////////////////////////////////////////////////
// Returns the size of the register.
////////////////////////////////////////////////////////////////////////

int ofxQuantumRegister::size() const
{
    return mRegSize;
}

////////////////////////////////////////////////////////////////////////
// Measure the bit, this forces it into one of the states
////////////////////////////////////////////////////////////////////////
int ofxQuantumRegister::measureBit(unsigned long long int bitIndx)
{
    // Get a randon number from the quantum simulator
    float quantumRandomNum = mQuantumSim->getRandom();
    
    // Chance of a zero state
    float zeroState = 0.0;
    
    // Chance of a one state
    float oneState  = 0.0;
    
    // now loop through all our bit states and add the probabilites
    for(int i = 0; i < pow(2,mRegSize);i++)
    {
        if(mBits[i][bitIndx] == 0)
        {
            zeroState += mState[i].length();
        }else if(mBits[i][bitIndx] == 1)
        {
            oneState  += mState[i].length();
        }
    }
    
    // Normalse the probabilities
    float length = zeroState + oneState;
    zeroState /= length;
    oneState  /= length;
  
    int result = -1;
    
    // Check our probabilities against the random number
    if(zeroState >= quantumRandomNum )
    {
        for(int i = 0; i < pow(2,mRegSize);i++)
        {
            if(mBits[i][bitIndx] != 0)
                mState[i] = Complex(0,0);
        }
        
        result = 0;
    }
    else
    {
        for(int i = 0; i < pow(2,mRegSize);i++)
        {
            if(mBits[i][bitIndx] != 1)
                mState[i] = Complex(0,0);
        }
        
        result = 1;
    }
    
    float total = 0.0;
    // Normalise remaining bits
    for(int i = 0; i < pow(2,mRegSize); i++)
    {
        std::complex<double> mycomplex (mState[i].getReal(), mState[i].getImag());

        total += std::norm(mycomplex);
    }
    total = sqrt(total);
    for(int i = 0; i < pow(2,mRegSize); i++)
    {
        mState[i] = Complex(mState[i].getReal() / total, mState[i].getImag() / total);
    }
    
    // Return the number we measured
    return result;
}

////////////////////////////////////////////////////////////////////////
// Measure a state, and return the decimal value measured.  Collapse
// the state so that the probability of measuring the measured value in
// the future is 1, and the probability of measuring any other state is
// 0.
////////////////////////////////////////////////////////////////////////

unsigned long long int ofxQuantumRegister::decimalMeasure()
{
    int done = 0;
    
    // Final decimal result of our measurement, -1 is an error, we did not measure anything
    int decVal = -1;
    
    // Variable to hold states
    double a, b;
    
    // Randon number from quantum simulator
    double rand1 = mQuantumSim->getRandom();
    a = b = 0;
    
    // Loop through the possible states and check if amplitude lies within the random number from
    // The quantum simulator
    for (unsigned long long int i = 0 ; i < pow(2, mRegSize)  ;i++) {
        if (!done ){
            b += pow(mState[i].getReal(), 2) + pow(mState[i].getImag(), 2);
            if (b > rand1 && rand1 > a) {
                //We have just measured the i state.
                for (unsigned long long int j = 0; j < pow(2, mRegSize) ; j++) {
                    mState[j].set(0,0);
                }
                mState[i].set(1,0);
                decVal = i;
                done = 1;
            }
            a += pow(mState[i].getReal(), 2) + pow(mState[i].getImag(), 2);
        }
    }
    return decVal;
}

////////////////////////////////////////////////////////////////////////
// For debugging, output information about the register.
////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::printInfo() 
{
    // Loop through and print out information about the bits
    for (unsigned long long int i = 0 ; i < pow(2, mRegSize) ; i++)
    {
        
        for(int j = 0; j < mRegSize;j++)
            cout << mBits[i][j];
        
        cout << " State " << i << " has probability amplitude "
        << mState[i].getReal() << " + i" << mState[i].getImag()
        << endl;
        
    }
}

////////////////////////////////////////////////////////////////////////
// Set the states to those given in the new_state array.
////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::setState(Complex *new_state) {
    
    // Set the state
    for (unsigned long long int i = 0 ; i < pow(2, mRegSize) ; i++)
    {
        mState[i].set(new_state[i].getReal(), new_state[i].getImag());
    }
}


////////////////////////////////////////////////////////////////////////
// Set the State to an equal superposition of the integers 0 -> number -1
////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::setAverage(unsigned long long int number)
{
    // If number is too big then print error message
    if (number >= pow(2, mRegSize))
    {
        cout << "Error, initializing past end of array in qureg::SetAverage.\n";
    }
    // Otherwise set the probability
    else
    {
        double prob = pow(number, -.5);
        for (unsigned long long int i = 0 ; i <= number ; i++) {
            mState[i].set(prob, 0);
        }
    }
}


/////////////////////////////////////////////////////////////////////////////
// Kron matrix expansion, used to create identiy matrix for gate operations
/////////////////////////////////////////////////////////////////////////////

cv::Mat ofxQuantumRegister::kron(cv::Mat& A, cv::Mat& B)
{
    CV_Assert(A.channels() == 1 && B.channels() == 1);
    
    cv::Mat1d Ad, Bd;
    A.convertTo(Ad, CV_64F);
    B.convertTo(Bd, CV_64F);
    
    // Expand matrix
    cv::Mat1d Kd(Ad.rows * Bd.rows, Ad.cols * Bd.cols, 0.0);
    for (int ra = 0; ra < Ad.rows; ++ra)
    {
        for (int ca = 0; ca < Ad.cols; ++ca)
        {
            Kd(cv::Range(ra*Bd.rows, (ra + 1)*Bd.rows), cv::Range(ca*Bd.cols, (ca + 1)*Bd.cols)) = Bd.mul(Ad(ra, ca));
        }
    }
    cv::Mat K;
    Kd.convertTo(K, A.type());
    
    return K;
}

////////////////////////////////////////////////////////////////////////////////////////////
// Apply pauli-X gate to register: https://en.wikipedia.org/wiki/Quantum_gate#Pauli-X_gate
////////////////////////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::applyGateX(unsigned long long int bit)
{
    if(bit < mRegSize)
    {
        int sz = pow(2,mRegSize);
        
        cv::Mat identity = (cv::Mat_<double>(2,2) << 1, 0, 0, 1);
        
        cv::Mat pauliX   = (cv::Mat_<double>(2,2) << 0, 1, 1, 0);
        
        cv::Mat result;
        
        if(bit == 0)
        {
            result = pauliX;
        }else
        {
            result = identity;
        }
        
        for(int i = 1; i < mRegSize; i++)
        {
            if( i == bit)
            {
                result = kron( result, pauliX );
            }else
            {
                result = kron( result, identity );
            }
        }
        
        applyToStates(&result);
        
        
    }else {
        printf("ERROR! bit indx out of range, max indx: %i\n", mRegSize);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////
// Apply pauli-Y gate to register: https://en.wikipedia.org/wiki/Quantum_gate#Pauli-Y_gate
////////////////////////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::applyGateY( unsigned long long int bit )
{
    if(bit < mRegSize)
    {
        int sz = pow(2,mRegSize);
        
        cv::Mat identityReal = (cv::Mat_<double>(2,2) << 1, 0, 0, 1);
        cv::Mat identityImag = (cv::Mat_<double>(2,2) << 1, 0, 0, 1);
        
        cv::Mat pauliYreal   = (cv::Mat_<double>(2,2) << 0, 0, 0, 0);
        cv::Mat pauliYimag   = (cv::Mat_<double>(2,2) << 0, -1, 1, 0);
        
        cv::Mat resultReal;
        cv::Mat resultImag;
        
        if(bit == 0)
        {
            resultReal = pauliYreal;
            resultImag = pauliYimag;
        }else
        {
            resultReal = identityReal;
            resultImag = identityImag;
        }
        
        for(int i = 1; i < mRegSize; i++)
        {
            if( i == bit)
            {
                resultReal = kron( resultReal, pauliYreal );
                resultImag = kron( resultImag, pauliYimag );
            }else
            {
                resultReal = kron( resultReal, identityReal );
                resultImag = kron( resultImag, identityImag );
            }
        }
        
        Complex *newStates = new Complex[ (int)pow(2,mRegSize) ];
        
        for(int i = 0; i < pow(2, mRegSize); i++)
        {
            newStates[i] = Complex(0,0);
            
            for(int j = 0; j < pow(2, mRegSize); j++)
            {
                Complex result = Complex(mState[j].getReal(), mState[j].getImag());
                
                result.mult(resultImag.at<double>(i,j));
                
                newStates[i] = newStates[i] + result;
                
            }
            
        }
        
        mState = newStates;
      
    }
    
    
}

////////////////////////////////////////////////////////////////////////////////////////////
// Apply pauli-Z gate to register: https://en.wikipedia.org/wiki/Quantum_gate#Pauli-Z_gate
////////////////////////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::applyGateZ( unsigned long long int bit )
{
    if(bit < mRegSize)
    {
        int sz = pow(2,mRegSize);
        
        cv::Mat identity = (cv::Mat_<double>(2,2) << 1, 0, 0, 1);
        
        cv::Mat pauliZ   = (cv::Mat_<double>(2,2) << 1, 0, 0, -1);
        
        cv::Mat result;
        
        if(bit == 0)
        {
            result = pauliZ;
        }else
        {
            result = identity;
        }
        
        for(int i = 1; i < mRegSize; i++)
        {
            if( i == bit)
            {
                result = kron( result, pauliZ );
            }else
            {
                result = kron( result, identity );
            }
        }
        
        applyToStates(&result);
        
        
    }else {
        printf("ERROR! bit indx out of range, max indx: %i\n", mRegSize);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////
// Apply cnot gate to register: https://en.wikipedia.org/wiki/Quantum_gate#Controlled_gates
////////////////////////////////////////////////////////////////////////////////////////////
void ofxQuantumRegister::applyGateCNOT(     unsigned long long int bit, int controlBitValue )
{
    if(controlBitValue == 1)
    {
        applyGateX(bit);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Apply toffoli gate to register: https://en.wikipedia.org/wiki/Toffoli_gate
/////////////////////////////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::applyGateToff( unsigned long long int bit, int controlBitVal1, int controlBitVal2 )
{
    if(controlBitVal1 == 1 && controlBitVal2 == 1)
    {
        applyGateX(bit);
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Apply hadamard gate to register: https://en.wikipedia.org/wiki/Quantum_gate#Hadamard_gate
/////////////////////////////////////////////////////////////////////////////////////////////////

void ofxQuantumRegister::applyGateHad(unsigned long long int bit)
{
    if(bit < mRegSize)
    {
        int sz = pow(2,mRegSize);
        float invSqrt = 1.0 / sqrt(2);
        
        cv::Mat identity = (cv::Mat_<double>(2,2) << 1, 0, 0, 1);
        
        cv::Mat hadamard = (cv::Mat_<double>(2,2) << 1.0, 1.0, 1.0 , -1.0 );
        
        
        cv::Mat result;
        
        if(bit == 0)
        {
            result = hadamard;
        }else
        {
            result = identity;
        }
        
        for(int i = 1; i < mRegSize; i++)
        {
            if( i == bit)
            {
                result = kron( result, hadamard );
            }else
            {
                result = kron( result, identity );
            }
        }
        
        applyToStates(&result);
        
        for(int i = 0; i < pow(2,mRegSize); i++)
        {
            mState[i] = Complex(mState[i].getReal() * invSqrt, mState[i].getImag() * invSqrt);
        }
        
        
    }else {
        printf("ERROR! bit indx out of range, max indx: %i\n", mRegSize);
    }
}

////////////////////////////
// Apply Matrix to states
///////////////////////////
void ofxQuantumRegister::applyToStates(cv::Mat *result)
{
    Complex *newStates= new Complex[ (int)pow(2,mRegSize) ];
    
    for(int i = 0; i < pow(2, mRegSize); i++)
    {
        float resultReal = 0.0;
        float resultImag = 0.0;
        
        for(int j = 0; j < pow(2, mRegSize); j++)
        {
            resultReal += mState[j].getReal() * result->at<double>(i,j);
            resultImag += mState[j].getImag() * result->at<double>(i,j);
        }
        
        newStates[i].set(resultReal, resultImag);
        
    }
    
    mState = newStates;
}


/////////////////////////////////////////////////
// Get number of states
/////////////////////////////////////////////////

unsigned long long int ofxQuantumRegister::getNumStates()
{
    return pow(2, mRegSize);
}

/////////////////////////////////////////////////
// Get specified state
/////////////////////////////////////////////////

Complex  ofxQuantumRegister::getState(int stateIndx)
{
    return mState[stateIndx];
    
}

