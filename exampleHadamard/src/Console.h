///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  Console
//
//  Created by Jayson Haebich, 2016, www.jaysonh.com
//
//  This class draws a text console to the screen outputting information about the quantum simulator
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __exampleHadamard__Console__
#define __exampleHadamard__Console__

#include "ofMain.h"
#include "Complex.h"

#define LINE_SPACING    10
#define MAX_LOG_ENTRIES 10
#define FONT_SIZE       8

class Console
{
public:
    
    Console();
    void init();
    void draw(ofVec2f pos, int state, int seed, Complex complexStateZero, Complex complexStateOne );
    void addMessage( std::string msg );
    
private:
    
    std::list <string> mLog;
    ofTrueTypeFont     mFont;
    int                mLastQuantumSeed;
};
#endif /* defined(__exampleHadamard__Console__) */
