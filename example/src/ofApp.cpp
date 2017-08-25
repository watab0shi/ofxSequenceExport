//
// cd data/export
// ffmpeg -framerate 30       -i %06d.png     -vcodec libx264     -pix_fmt yuv420p          -r 30        -y out.mov
// ffmpeg -framerate [in_fps] -i [in_imgName] -vcodec [vid_codec] -pix_fmt [vid_pix_fotmat] -r [vid_fps] -y [output_file]
//

#include "ofApp.h"


// setup
//------------------------------------------------------------
void ofApp::setup()
{
  numQue = 0;
  numExp = 0;
  savePath = "export/";
  
  // clean up directory
  ofDirectory dir( savePath );
  dir.listDir();
  vector< ofFile > fs = dir.getFiles();
  for( auto& f : fs ) f.remove();
  
  fbo.allocate( WIDTH, HEIGHT, GL_RGB );
  
  expo.setup( &fbo, savePath, "jpg", OF_IMAGE_QUALITY_HIGH );
  expo.setDuration( DURATION );
  
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
  fbo.begin();
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
  }
  fbo.end();
  
  // call expo.update() after drawing fbo
  expo.update();
}


// draw
//------------------------------------------------------------
void ofApp::draw()
{
  ofSetColor( 255 );
  fbo.draw( 0, 0 );
  
  // progress bar
  ofSetColor( 160 );
  ofDrawRectangle( 0, HEIGHT, WIDTH, -6 );
  ofSetColor( ( expo.isCompleted() ) ? ofColor( 0, 200, 0 ) : ofColor( 200, 0, 0 ) );
  ofDrawRectangle( 0, HEIGHT, ofMap( numExp, 0, numQue, 0, WIDTH, true ), -6 );
  
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
