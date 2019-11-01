#pragma once

#include "ofMain.h"
#include "ofxSequenceExport.h"


// ofApp
//--------------------------------------------------------------------------------
class ofApp : public ofBaseApp
{
public:
  void setup();
  void update();
  void draw();
  void exit();
  
  void keyPressed( int _key );
  
  ofxSequenceExport expo;
  std::shared_ptr< ofFbo > fbo;
  int               frame;
  int               numQue;
  int               numExp;
  string            savePath;
  
  void drawArc( float _t, float _r );
};
