/*
 * File name: AmplitudeData.cpp
 * Created on: 12-July-2017
 * Author: ravi
 */

#include <iostream>
#include "AmplitudeData.hpp"

//Default constructor
AmplitudeData::AmplitudeData(){
  // enter default values

}


/*
 * Populate all the Wave data
 */
void AmplitudeData::populate(PULSEreader *pReader, WAVESsampling *sampling, 
                              int maxCount, long long pulseIndex){
  waveData.push_back(pulseIndex);
  for(int j = 0; j < sampling->get_number_of_segments(); j++ ){
    sampling->set_active_segment(j);                  
    for(int k = 0; k < maxCount; k++){
      if(k >= sampling->get_number_of_samples()){
        waveData.push_back(0);
      } 
      else{
        waveData.push_back(sampling->get_sample(k));
      }                    
    }
  }              
}

/*
 * Calculate the first diferences
 */
void AmplitudeData::calculateFirstDifference(){
  int first, second, fDiff, count = 0;
  for(int i = 0; i< waveData.size(); i++){
    first = waveData[i+1];
    second = waveData[i+2];

    fDiff = second - first;

    firstDifference.push_back(fDiff);        
    count++;

    if(count == 59){      
        count = 0;
        i = i+2;
    }
  }
}

/*
 * Calculate the second diferences
 */
void AmplitudeData::calculateSecondDifference(){
  int first, second, sDiff, count =0;
  for(int i = 0; i< firstDifference.size(); i++){  
    first = firstDifference[i];
    second = firstDifference[i+1];
    sDiff = second - first;

    secondDifference.push_back(sDiff);
    count++;
    if (count == 58){
      count = 0;
      i = i+1;
    }
  
  }
}

/*
 * Check if values change from +ve to -ve
 */
void AmplitudeData::countPeaks(){
    for(int i = 0; i<firstDifference.size()-1; i++){
      if(firstDifference[i] > 0 && firstDifference[i+1] < 0){

      }
    }

}


/*
 * Displays all wave data
 */
void AmplitudeData::displayData(){
  std::cout << "Wave: \n" << std::endl;
  int count = 0;
  for(int i = 0; i<waveData.size(); i++){
    std::cout << waveData[i] << " ";
    count++;
    if(count == 61){
      count = 0;
      std::cout << std::endl ;
    }
  }
  std::cout << "First Diff\n";
  for(int i = 0, j = 1; i<firstDifference.size(); i++, j++){
    std::cout << firstDifference[i] << " ";
    if(j == 59){
      j = 0;
      std::cout << std::endl ;
    }
  }
  std::cout << "\nSecond Diff\n";
  for(int i = 0, j = 1; i<secondDifference.size(); i++, j++){
    std::cout << secondDifference[i] << " ";
    if(j == 58){
      j = 0;
      std::cout << std::endl ;
    }
  }
}