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
    bool        bJpg;
    
    QueImg( ofPixels _pixs, std::string _fileName, bool _bJpg )
    : pixs( _pixs )
    , fName( _fileName )
    , bJpg( _bJpg )
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
      if( lock() )
      {
        if( !ques.empty() )
        {
          if( ques.front().bJpg )
          {
            ofSaveImage( ques.front().pixs, ques.front().fName, SequenceExport::quality );
          }
          else
          {
            ofSaveImage( ques.front().pixs, ques.front().fName );
          }
          ques.pop_front();
          
          numExportedFrames++;
        }
        
        unlock();
      }
    }
  }
};


const unsigned int NUM_THREADS = 6;

// SequenceExport
//--------------------------------------------------------------------------------
class ofxSequenceExport
{
public:
  enum ExtType
  {
    EXT_JPG,
    EXT_PNG
  };
  
  void setup( std::shared_ptr< ofFbo > _fbo, std::string _dirPath = "", ExtType _ext = EXT_PNG, ofImageQualityType _quality = OF_IMAGE_QUALITY_BEST );
  
  void setDurationByTime( float _duration );
  void setDurationByFrame( int _num );
  
  void start();
  void stop();
  
  void stopQueue();
  
  void update( std::string _baseFileName = "" );
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
  
private:
  
  SequenceExport     expos[ NUM_THREADS ];
  
  ofxFastFboReader   reader;
  std::shared_ptr< ofFbo > fbo;
  ofPixels           pixels;
  
  std::string        dirPath;
  ExtType            ext;
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
  
  void addQue( std::string _outFilePath );
};
