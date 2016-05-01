//
//  Console.cpp
//  exampleHadamard
//
//  Created by jayson on 23/04/2016.
//
//

#include "Console.h"

Console::Console()
{
    
}

void Console::init()
{
    // Load font
    mFont.loadFont(ofToDataPath("fonts/Courier.ttf"), FONT_SIZE, true);
}

void Console::draw( ofVec2f pos, int state, int seed, Complex complexStateZero, Complex complexStateOne )
{
    // Draw console to the screen
    ofPushMatrix();
    ofTranslate(pos.x, pos.y);
    
    mFont.drawString("***************************************",0,0);
    for(int i = 1; i < 23; i++)
    {
        mFont.drawString("*", 0,   LINE_SPACING * i);
        mFont.drawString("*", 267, LINE_SPACING * i);
    }
    
    mFont.drawString("Summary", 55, 2 * LINE_SPACING);
    mFont.drawString("=============================", 55, 3 * LINE_SPACING);
    
    mFont.drawString("Measured:      " + ofToString(state), 55,LINE_SPACING*4);
    mFont.drawString("QuantumSeed:   " + ofToString(seed),  55,LINE_SPACING*5);
    
    mFont.drawString("Zero State:    " + ofToString(abs(complexStateZero.getReal())) + " + " + ofToString(complexStateZero.getImag()) + "i",  55,LINE_SPACING*6);
    
    mFont.drawString("One  State:    " + ofToString(abs(complexStateOne.getReal())) + " + " + ofToString(complexStateOne.getImag()) + "i",  55,LINE_SPACING*7);
    
    mFont.drawString("***************************************", 0,LINE_SPACING*23);
    
    mFont.drawString("console log", 55, 9 * LINE_SPACING);
    mFont.drawString("=============================", 55, 10 * LINE_SPACING);
    
    int lineNum = 0;
    for (std::list<string>::const_iterator iterator = mLog.begin(); iterator != mLog.end(); ++iterator) {
        string line = *iterator;
        
        mFont.drawString(" > " + line, 55, (11+lineNum) * LINE_SPACING);
        lineNum++;
    }
    
    ofPopMatrix();
    
    addMessage("Measured state: " + ofToString(state));
    
    // If the quantum seed has changed then write this to the console
    if(seed != mLastQuantumSeed)
    {
        mLastQuantumSeed = seed;
        addMessage("Updated seed: " + ofToString(seed));
    }
}


void Console::addMessage(std::string msg)
{
    mLog.push_front(msg);
    
    while(mLog.size() > MAX_LOG_ENTRIES)
    {
        mLog.pop_back();
    }
}

