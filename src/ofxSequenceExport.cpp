
#include "ofxSequenceExport.h"
#include "ofAppRunner.h"
#include "ofGraphics.h"
#include "ofMath.h"


// SequenceExport [ static ]
//--------------------------------------------------------------------------------
ofImageQualityType SequenceExport::quality = OF_IMAGE_QUALITY_BEST;

void SequenceExport::setQuality( ofImageQualityType _quality )
{
  quality = _quality;
}


// setup
//------------------------------------------------------------
void ofxSequenceExport::setup( std::shared_ptr< ofFbo > _fbo, std::string _dirPath, ofxSequenceExport::ExtType _ext, ofImageQualityType _quality )
{
  fbo               = _fbo;
  
  ofDirectory dir( _dirPath );
  dirPath = dir.getAbsolutePath();
  ofLog() << dirPath;
  
  ext = _ext;
  if( ext == EXT_JPG )
  {
    SequenceExport::setQuality( _quality );
    format = "jpg";
  }
  else
  {
    format = "png";
  }
  
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
  bUseFastFboReader = false;
}


// useFastFboReader
//------------------------------------------------------------
void ofxSequenceExport::useFastFboReader( bool _b )
{
  bUseFastFboReader = _b;
}


// setDuration
//------------------------------------------------------------
void ofxSequenceExport::setDurationByTime( float _duration )
{
  bUseDuration  = true;
  bUseNumFrames = false;
  duration      = _duration;
}


// setNumFrames
//------------------------------------------------------------
void ofxSequenceExport::setDurationByFrame( int _num )
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
  
  if( bUseFastFboReader )
  {
    reader.setAsync( false );
  }
  
  for( auto& e : expos )
  {
    e.ques.clear();
    e.start();
  }
}

// stop
//------------------------------------------------------------
void ofxSequenceExport::stop()
{
  bAddQue  = false;
  bRunning = false;
  
  for( auto& e : expos )
  {
    e.stop();
    e.ques.clear();
  }
}


// addQue
//------------------------------------------------------------
void ofxSequenceExport::addQue( std::string _outFilePath )
{
  if( bUseFastFboReader )
  {
    reader.readToPixels( *fbo, pixels );
  }
  else
  {
    fbo->readToPixels( pixels );
  }
  
  if( expos[ numQueFrames % NUM_THREADS ].lock() )
  {
    expos[ numQueFrames % NUM_THREADS ].ques.emplace_back( pixels, _outFilePath, ( ext == EXT_JPG ) );
    expos[ numQueFrames % NUM_THREADS ].unlock();
  }
  
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
void ofxSequenceExport::update( std::string _baseFileName )
{
  if( !isRunning() ) return;
  
  elapsed = ofGetElapsedTimef() - sTime;
  last    = ofGetLastFrameTime();
  
  if( ( bUseDuration  && ( bAddQue && ( elapsed - last ) > duration ) ) ||
      ( bUseNumFrames && ( bAddQue && numQueFrames >= numFrames ) ) )
  {
    stopQueue();
  }
  
  if( bAddQue )
  {
    std::string baseName = ( _baseFileName == "" ) ? ofToString( numQueFrames, 6, '0' ) : _baseFileName;
    std::string fullPath = dirPath + "/" + baseName + "." + format;
    
    addQue( fullPath );
  }
  
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
