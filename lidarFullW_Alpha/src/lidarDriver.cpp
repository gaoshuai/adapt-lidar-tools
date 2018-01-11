/*
 * File name: lidarDriver.cpp
 * Created on: 17-May-2017
 * Author: ravi
 */
 
#include <iostream>
#include <iomanip>
#include <vector>
#include "CmdLine.hpp"
#include "FlightLineData.hpp"
#include "WaveGPSInformation.hpp"
#include "LidarVolume.hpp"
#include "PulseData.hpp"
#include "Peak.hpp"
//#include "GaussianFitter.hpp"


int main (int argc, char *argv[]) {

  // parse the command lines
  CmdLine cmdLineArgs;
  cmdLineArgs.parse(argc,argv);
  
  if(cmdLineArgs.printUsageMessage == true) {
    std::cout << cmdLineArgs.getUsageMessage() << std::endl;
    return 0;
  }

  // create a flight line data object
  std::string fileName = cmdLineArgs.getInputFileName();
  FlightLineData rawData;
  rawData.setFlightLineData(fileName);

  LidarVolume intermediateData;
  intermediateData.setBoundingBox(rawData.bb_x_min,rawData.bb_x_max,
                                  rawData.bb_y_min,rawData.bb_y_max,
                                  rawData.bb_z_min,rawData.bb_z_max);
  intermediateData.allocateMemory();

  PulseData pd;
  std::ostringstream stream;
  //GaussianFitter fitter;
  while(rawData.hasNextPulse()){
    rawData.getNextPulse(&pd);
    // struct peaks peakList = fitter.findPeaks(pd); 
    // for(int i=0;i<peaks.size;i++){
    //   intermediateData.addPeak(peaks.peak_list[i]);
    // }
    pd.displayPulseData(&stream);
    std::cout << stream.str() << std::endl;
  }
  // Lidar Volume is full and complete
  // Rasterize it
  
}
