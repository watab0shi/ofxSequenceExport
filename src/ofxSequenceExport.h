//
//  ofxSequenceExport.h
//  base1 : https://github.com/atduskgreg/ofxImageSequenceRecorder
//  base2 : https://gist.github.com/rettuce/bb8b17233137de776306
//
#pragma once

#include "ofMain.h"


// SequenceExport
//--------------------------------------------------------------------------------
class SequenceExport : public ofThread
{
  struct QueImg
  {
    string   fName;
    ofPixels pixs;
    
    QueImg( ofPixels _pixs, string _fileName ) : pixs( _pixs ), fName( _fileName ){}
  };
  
public:
  static ofImageQualityType quality;
  static void setQualty( ofImageQualityType _quality );
  
  deque< QueImg > ques;
  int             numExportedFrames = 0;
  
  bool isRunning(){ return isThreadRunning(); }
  
  void start()
  {
    numExportedFrames = 0;
    startThread( false );
  }
  
  void stop()
  {
    stopThread();
  }
  
  void threadedFunction()
  {
    while( isThreadRunning() )
    {
      if( !ques.empty() )
      {
        ofSaveImage( ques.front().pixs, ques.front().fName, SequenceExport::quality );
        ques.pop_front();
        
        if( lock() )
        {
          numExportedFrames++;
          unlock();
        }
      }
    }
  }
};


const unsigned int NUM_THREADS = 6;

// SequenceExport
//--------------------------------------------------------------------------------
class ofxSequenceExport
{
  SequenceExport     expos[ NUM_THREADS ];
  
  ofFbo*             fbo;
  ofPixels           pixels;
  
  string             outpath;
  string             format;
  
  int                numQueFrames;
  int                numExportedFrames;
  
  float              sTime;
  float              elapsed;
  float              last;
  
  bool               bAddQue;
  bool               bRunning;
  bool               bComplete;
  
  float              duration;
  bool               bUseDuration;
  
  void addQue();
  
public:
  void setup( ofFbo* _fbo, string _outpath, string _format, ofImageQualityType _quality = OF_IMAGE_QUALITY_HIGH );
  
  void setDuration( float _duration );
  
  void start();
  void stop();
  
  void stopQueue();
  
  void update();
  void drawProgressBar();
  
  int getNumExportedFrames()
  {
    numExportedFrames = 0;
    for( auto& e : expos ) numExportedFrames += e.numExportedFrames;
    
    return numExportedFrames;
  }
  
  int   getNumQueFrames(){ return numQueFrames; }
  float getElapsedTime(){  return elapsed; }
  bool  isRunning(){       return bRunning; }
  bool  isCompleted(){     return bComplete; }
};
