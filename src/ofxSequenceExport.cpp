
#include "ofxSequenceExport.h"


// SequenceExport [ static ]
//--------------------------------------------------------------------------------
ofImageQualityType SequenceExport::quality = OF_IMAGE_QUALITY_HIGH;

void SequenceExport::setQualty( ofImageQualityType _quality )
{
  quality = _quality;
}



// setup
//------------------------------------------------------------
void ofxSequenceExport::setup( ofFbo* _fbo, string _outpath, string _format, ofImageQualityType _quality )
{
  fbo               = _fbo;
  outpath           = _outpath;
  format            = _format;
  
  SequenceExport::setQualty( _quality );
  
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
}


// setDuration
//------------------------------------------------------------
void ofxSequenceExport::setDuration( float _duration )
{
  bUseDuration = true;
  duration     = _duration;
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
  
  cout << "[ ofxSequenceExport ] stoped adding quque!" << endl;
}


// update
//------------------------------------------------------------
void ofxSequenceExport::update()
{
  if( !isRunning() ) return;
  
  elapsed = ofGetElapsedTimef() - sTime;
  last    = ofGetLastFrameTime();
  
  if( bUseDuration && ( bAddQue && ( elapsed - last ) > duration ) ) stopQueue();
  
  if( bAddQue ) addQue();
  
  if( !bComplete && getNumExportedFrames() == numQueFrames )
  {
    bComplete = true;
    
    cout << "[ ofxSequenceExport ] export completed!" << endl;
  }
}
