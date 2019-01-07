
#include "ofxSequenceExport.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofMath.h"


// SequenceExport [ static ]
//--------------------------------------------------------------------------------
ofImageQualityType SequenceExport::quality = OF_IMAGE_QUALITY_HIGH;

void SequenceExport::setQuality( ofImageQualityType _quality )
{
  quality = _quality;
}



// setup
//------------------------------------------------------------
void ofxSequenceExport::setup( ofFbo* _fbo, std::string _outpath, std::string _format, ofImageQualityType _quality )
{
  fbo               = _fbo;
  outpath           = _outpath;
  format            = _format;
  
  SequenceExport::setQuality( _quality );
  
  numQueFrames      = 0;
  numExportedFrames = 0;
  
  sTime             = 0.;
  elapsed           = 0.;
  last              = 0.;
  
  bAddQue           = false;
  bRunning          = false;
  bComplete         = false;
  
  duration          = 0.;
  bUseDuration      = false;
  
  numFrames         = 0;
  bUseNumFrames     = false;
}


// setDuration
//------------------------------------------------------------
void ofxSequenceExport::setDuration( float _duration )
{
  bUseDuration  = true;
  bUseNumFrames = false;
  duration      = _duration;
}


// setNumFrames
//------------------------------------------------------------
void ofxSequenceExport::setNumFrames( int _num )
{
  bUseDuration  = false;
  bUseNumFrames = true;
  numFrames     = _num;
}


// start
//------------------------------------------------------------
void ofxSequenceExport::start()
{
  numQueFrames      = 0;
  numExportedFrames = 0;
  
  sTime             = ofGetElapsedTimef();
  
  bAddQue           = true;
  bRunning          = true;
  
  for( auto& e : expos ) e.start();
}

// stop
//------------------------------------------------------------
void ofxSequenceExport::stop()
{
  bAddQue  = false;
  bRunning = false;
  
  for( auto& e : expos ) e.stop();
}


// addQue
//------------------------------------------------------------
void ofxSequenceExport::addQue()
{
  fbo->readToPixels( pixels );
  
  assert( outpath.length() > 0 );
  assert( format.length()  > 0 );
  
  char fName[ 100 ];
  sprintf( fName, "%s%.6i.%s", outpath.c_str(), numQueFrames, format.c_str() );
  
  expos[ numQueFrames % NUM_THREADS ].ques.emplace_back( pixels, fName );
  
  ++numQueFrames;
}


// stopQueue
//------------------------------------------------------------
void ofxSequenceExport::stopQueue()
{
  bAddQue = false;
  sTime   = ofGetElapsedTimef();
  
  ofLog() << "[ ofxSequenceExport ] stoped adding quque!";
}


// update
//------------------------------------------------------------
void ofxSequenceExport::update()
{
  if( !isRunning() ) return;
  
  elapsed = ofGetElapsedTimef() - sTime;
  last    = ofGetLastFrameTime();
  
  if( ( bUseDuration  && ( bAddQue && ( elapsed - last ) > duration ) ) ||
      ( bUseNumFrames && ( bAddQue && getNumExportedFrames() == numFrames ) ) )
  {
    stopQueue();
  }
  
  if( bAddQue ) addQue();
  
  if( !bComplete && getNumExportedFrames() == numQueFrames )
  {
    bComplete = true;
    
    ofLog() << "[ ofxSequenceExport ] export completed!";
  }
}


// drawProgressBar
//------------------------------------------------------------
void ofxSequenceExport::drawProgressBar()
{
  int w = ofGetWidth();
  int h = ofGetHeight();
  
  ofSetColor( 160 );
  ofDrawRectangle( 0, h, w, -6 );
  ofSetColor( ( bComplete ) ? ofColor( 0, 200, 0 ) : ofColor( 200, 0, 0 ) );
  ofDrawRectangle( 0, h, ofMap( numExportedFrames, 0, numQueFrames, 0, w, true ), -6 );
}
