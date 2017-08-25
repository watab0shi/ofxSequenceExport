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
  
  const int         WIDTH    = 1280;
  const int         HEIGHT   = 800;
  const float       DURATION = 10.;
  
  ofxSequenceExport expo;
  ofFbo             fbo;
  int               numQue;
  int               numExp;
  string            savePath;
  
  void drawArc( float _t, float _r );
};
