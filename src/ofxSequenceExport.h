//
//  ofxSequenceExport.h
//  base1 : https://github.com/atduskgreg/ofxImageSequenceRecorder
//  base2 : https://gist.github.com/rettuce/bb8b17233137de776306
//
#pragma once

#include "ofThread.h"
#include "ofImage.h"
#include "ofFbo.h"
#include "ofxFastFboReader.h"


// SequenceExport
//--------------------------------------------------------------------------------
class SequenceExport : public ofThread
{
  struct QueImg
  {
    std::string fName;
    ofPixels    pixs;
    
    QueImg( ofPixels _pixs, std::string _fileName )
    : pixs( _pixs )
    , fName( _fileName )
    {
    }
  };
  
public:
  static ofImageQualityType quality;
  static void setQuality( ofImageQualityType _quality );
  
  std::deque< QueImg > ques;
  int                  numExportedFrames = 0;
  
  bool isRunning(){ return isThreadRunning(); }
  
  void start()
  {
    numExportedFrames = 0;
    startThread();
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
//        ofSaveImage( ques.front().pixs, ques.front().fName, SequenceExport::quality );
        ofSaveImage( ques.front().pixs, ques.front().fName );
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
  
  ofxFastFboReader   reader;
  ofFbo*             fbo;
  ofPixels           pixels;
  
  std::string        outpath;
  std::string        format;
  
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
  int                numFrames;
  bool               bUseNumFrames;
  
  void addQue();
  void addQue( std::string _outFilePath );
  
public:
  void setup( ofFbo* _fbo, std::string _outpath, std::string _ext, ofImageQualityType _quality = OF_IMAGE_QUALITY_HIGH );
  
  void setDuration( float _duration );
  void setNumFrames( int _num );
  
  void start();
  void stop();
  
  void stopQueue();
  
  void update();
  void update( std::string _outFilePath );
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
