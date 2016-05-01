#pragma once

#include "ofMain.h"

#include "ofxQuantum.h"
#include "Console.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ofApp
//
//  Created by Jayson Haebich, 2016, www.jaysonh.com
//
//  Main openframeworks app
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////


class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
    
private:

        ofxQuantum           quantumSim;
        ofxQuantumRegister * quantumReg;
    
        ofShader             shader;
        int                  state;
    
        Console              console;
    
        int                  seed;
		
};
