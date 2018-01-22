// File name: LidarVolume.cpp
// Created on: 08-November-2017
// Author: ravi
 
#include "LidarVolume.hpp"
#include <math.h>

//Default constructor
LidarVolume::LidarVolume(){
  //initialize to zero
  bb_x_min_pad;
  bb_y_min_pad;
  bb_z_min_pad;
  bb_x_max_pad;
  bb_y_max_pad;
  bb_z_max_pad;
  bb_i_min = 0;
  bb_j_min = 0;
  bb_k_min = 0;
  bb_i_max = 0;
  bb_j_max = 0;
  bb_k_max = 0;

  i_extent = 0;
  j_extent = 0;
  k_extent = 0;
  
  currentPeak = 0;
  numOfPeaks = 5;
  //peakData = NULL;
  peaks = 0;
  
}

void LidarVolume::setBoundingBox(double ld_xMin, double ld_xMax,
                                       double ld_yMin, double ld_yMax,
                                       double ld_zMin, double ld_zMax){  

  bb_x_min_pad = ld_xMin-10;
  bb_y_min_pad = ld_yMin-10;
  bb_z_min_pad = ld_zMin-10;
  bb_x_max_pad = ld_xMax+10;
  bb_y_max_pad = ld_yMax+10;
  bb_z_max_pad = ld_zMax+10;

  bb_i_min = 0;
  bb_j_min = 0;
  bb_k_min = 0;
  bb_i_max = (int)(ceil(bb_x_max_pad - bb_x_min_pad));
  bb_j_max = (int)(ceil(bb_y_max_pad - bb_y_min_pad));
  bb_k_max = (int)(ceil(bb_z_max_pad - bb_z_min_pad));

  i_extent = bb_i_max - bb_i_min +1;
  j_extent = bb_j_max - bb_j_min +1;
  k_extent = bb_k_max - bb_k_min +1;
}

void LidarVolume::insertPeak(peak *pulsePeak){
  
  peaks[currentPeak].time = pulsePeak->time;
  peaks[currentPeak].amp = pulsePeak->amp;
  peaks[currentPeak].width = pulsePeak->width;
  
  currentPeak++;
}

//allocate memory with a 3d array of the data
void LidarVolume::allocateMemory(){
  // we are going to allocate a 3D array of space that will hold peak 
  // information (we don't know how many per voxel)
  // vector<int>* element;
  // (vector<int>**) malloc (#ofiElem * #ofjElem * #ofk * sizeof(element));
}

void LidarVolume::deallocateMemory(){

}

int LidarVolume::position(int i, int j, int k){
  //return k + (j* len(k)) + (i*len(k)*len(j));
  return 0;
}
