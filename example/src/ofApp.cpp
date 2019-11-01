//
// cd data/export
// ffmpeg -framerate 30       -i %06d.jpg     -vcodec libx264     -pix_fmt yuv420p          -r 30        -y out.mp4
// ffmpeg -framerate [in_fps] -i [in_imgName] -vcodec [vid_codec] -pix_fmt [vid_pix_fotmat] -r [vid_fps] -y [output_file]
//

#include "ofApp.h"


namespace
{
  const int   WIDTH    = 1920;
  const int   HEIGHT   = 1080;
  const float DURATION = 10.;
  const int   FRAME    = 300;
}

// setup
//------------------------------------------------------------
void ofApp::setup()
{
  numQue = 0;
  numExp = 0;
  frame  = 0;
  savePath = "export";
  
  // clean up directory
  ofDirectory dir( savePath );
  dir.listDir();
  std::vector< ofFile > fs = dir.getFiles();
  for( auto& f : fs ) f.remove();
  
  fbo = std::make_shared< ofFbo >();
  fbo->allocate( WIDTH, HEIGHT, GL_RGB );
  
  expo.setup( fbo, savePath );
  expo.useFastFboReader( true );
//  expo.setup( fbo, savePath, ofxSequenceExport::EXT_JPG, OF_IMAGE_QUALITY_BEST );
//  expo.setDurationByTime( DURATION );
  expo.setDurationByFrame( 300 );
  
  ofSetWindowShape( WIDTH, HEIGHT );
  ofSetFrameRate( 30 );
  ofBackground( 0 );
}


// update
//------------------------------------------------------------
void ofApp::update()
{
  numQue = expo.getNumQueFrames();
  numExp = expo.getNumExportedFrames();
  
  // draw in fbo
  fbo->begin();
  {
    ofClear( 0, 255 );
    ofTranslate( WIDTH * .5, HEIGHT * .5 );
    
    // draw arc
    ofSetColor( ofColor::fromHsb( ofGetFrameNum() % 256, 240, 255 ) );
    drawArc( expo.getElapsedTime(), HEIGHT * .35 );
    
    // drw label
    ofDrawBitmapStringHighlight( "fps       : " + ofToString( ofGetFrameRate(), 3 ), 0, -40 );
    ofDrawBitmapStringHighlight( "exp / que : " + ofToString( numExp ) + " / " + ofToString( numQue ), 0, -20 );
    ofDrawBitmapStringHighlight( "elapsed   : " + ofToString( expo.getElapsedTime(), 3, 9, '0' ), 0, 20 );
    ofDrawBitmapStringHighlight( "last      : " + ofToString( ofGetLastFrameTime() ), 0, 40 );
    ofDrawBitmapStringHighlight( "frame     : " + ofToString( frame ), 0, 60 );
  }
  fbo->end();
  
  // call expo.update( baseFileName ) after drawing fbo
  expo.update( ofToString( frame, 5, '0' ) );
  
  if( expo.isRunning() )
  {
    ++frame;
  }
}


// draw
//------------------------------------------------------------
void ofApp::draw()
{
  ofSetColor( 255 );
  fbo->draw( 0, 0 );
  
  // progress bar
  expo.drawProgressBar();
  
  ofDrawBitmapStringHighlight( "'s' key : start, stop", 20, 40 );
}


// drawArc
//------------------------------------------------------------
void ofApp::drawArc( float _t, float _r )
{
  int   n     = 1000;
  float angle = fmod( _t, 1 ) * TWO_PI / ( n - 1 );
  
  glBegin( GL_TRIANGLE_STRIP );
  for( int i = 0; i < n; ++i )
  {
    float   a  = i * angle - HALF_PI;
    ofVec2f p0 = ofVec2f( _r - 15, 0 ).rotateRad( a );
    ofVec2f p1 = ofVec2f( _r + 15, 0 ).rotateRad( a );
    
    glVertex2f( p0.x, p0.y );
    glVertex2f( p1.x, p1.y );
  }
  glEnd();
}


// keyPressed
//------------------------------------------------------------
void ofApp::keyPressed( int _key )
{
  if( _key == 's' )
  {
    numQue = 0;
    numExp = 0;
    frame  = 0;
    
    if( expo.isRunning() ) expo.stopQueue();
    else                   expo.start();
  }
}


// exit
//------------------------------------------------------------
void ofApp::exit()
{
  expo.stop();
}
