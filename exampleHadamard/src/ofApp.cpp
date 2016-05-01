#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    console.init();
    
    // Initialise quantum simulator
    quantumSim.init();
    
    // Create our register with one qubit and link it to the quantum simulator
    quantumReg =new ofxQuantumRegister( 1, &quantumSim );
    
    // Apply the hadamard gate which puts the qubit in a superposition states
    // This means that it has an equal chance of being a 1 or a 0 when it is measured
    quantumReg->applyGateHad(0);
    
    // Load shader that draws our visualisation
    shader.load("shaders/hadamard");
    
    // Set our window title
    ofSetWindowTitle("ofxQuantum");
}

//--------------------------------------------------------------
void ofApp::update(){
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    
    // If the mouse is not being pressed then measure the state
    // When the mouse is pressed then the state is held
    if( !(ofGetMousePressed()))
    {
        // Measure the qubit and save the measured state
        state = quantumReg->measureBit(0);
        
        // Reapply the hadamard gate so that the qubit is back in the superposition state
        quantumReg->applyGateHad(0);
    }
    
    // Draw quantum bit visualisation in a shader
    shader.begin();
    
    // Set shader uniforms
    shader.setUniform3f("iResolution", ofGetWidth(),ofGetHeight(), 0.0);
    shader.setUniform1f("iGlobalTime", ofGetElapsedTimef());
    shader.setUniform2f("iMouse",      ofVec2f(ofGetMouseX(),ofGetMouseY()));
    shader.setUniform1f("iState",      (float)state );
    
    ofRect(0,0, ofGetWidth(), ofGetHeight());
    shader.end();
    
    // Draw console to the screen
    console.draw(ofVec2f(10,30), state, seed, quantumReg->getState(0), quantumReg->getState(1));
}

