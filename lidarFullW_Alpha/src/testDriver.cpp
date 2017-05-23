/*
 * File name: testDriver.cpp
 * Created on: 18-May-2017
 * Author: ravi
 */

#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "cmdLine.hpp"

int testCmdLine();
int testFileReader();

int main (){
  int failCount = 0;

  // Call all tests associated with cmdLine parsing
  failCount += testCmdLine(); 

  // Call all tests associated with fileReading
  failCount += testFileReader();

  std::cerr << "All Testing Complete: " << failCount << " tests failed\n" 
    << std::endl;
}

int testFileReader(){

  int passCount = 0;
  int failCount = 0;

  return failCount; 
}

/**
 * This function tests various command line arguments
 */
int testCmdLine(){

  int passCount = 0;
  int failCount = 0;

  int noOfArgs;
  char** someArgs;

  // set up some space to play around with
  // command line parsing
  someArgs = (char**)malloc(sizeof(char*)*10);
  for(int i=0;i<10;i++){
    someArgs[i] = (char*)malloc(sizeof(char)*256);
  } 
  // if getting space failed just return with a failure
  if(someArgs == NULL){
    std::cerr << "FAILURE: Malloc failed for testing" << 
      std::endl;
    return 1;
  }

  /**
   * This section contains all good tests
   */

  try{
    /* Test for no command line arguments
     * The program  name is considered an argument so we use noOfArgs = 1
     * If an exception is caught, then the test passes
     * Option: none
     * Argument: none
     * Expected result: Catch the exception and print usage
     */
    noOfArgs = 1;
    strncpy( someArgs[0],"test",4);
    parseCmdLineArgs(noOfArgs,someArgs);
    std::cerr << "\nFAIL: Test 1 - No command line arguments" << std::endl;
    failCount++;
  }catch(const std::exception& e){
    passCount++;
  }

  try{	
    /* Test for 2 command line arguments
     * option: f
     * argument: none
     * Expected result: Indicate that an argument is required and print usage
     */
    noOfArgs = 2;
    strncpy( someArgs[1],"-f",2);
    CmdLineArgs config = parseCmdLineArgs(noOfArgs,someArgs);
    if(strncmp(config.inputFile(),"test",4)){
      passCount++;
    }else{
        failCount++;
        std::cerr << "\nFAIL: Test 2 - Valid option '-f' without argument" 
                  << std::endl;
    }
  }catch(const std::exception& e){
    failCount++;
    std::cerr << "\nFAIL: Test 2 - Valid option '-f' without argument" 
              << std::endl;
    std::cerr << e.what();
  }

  try{	
    /* Test for 2 command line arguments
     * option: f
     * argument: test
     * Expected result: Print the first line of the 'test' file
     */
    noOfArgs = 3;
    strncpy( someArgs[1],"-f",2);
    strncpy( someArgs[2],"test",4);
    parseCmdLineArgs(noOfArgs,someArgs);
    CmdLineArgs config = parseCmdLineArgs(noOfArgs,someArgs);
    if(strncmp(config.inputFile(),"test",4)){
      passCount++;
      
    }else{
        failCount++;
        std::cerr << "\nFAIL: Test 3 - Valid option '-f' without argument" 
                  << std::endl;
    }
  }catch(const std::exception& e){
    failCount++;
    std::cerr << "\nFAIL: Test 3 - Valid option '-f' without argument" 
              << std::endl;
    std::cerr << e.what();
  }

  try{	
    /* Test for 2 command line arguments
     * option: f
     * argument: rest
     * Expected result: Argument 'rest' is not a valid file
     */
    noOfArgs = 2;
    strncpy( someArgs[1],"-f",2);
    strncpy( someArgs[2],"rest",4);
    parseCmdLineArgs(noOfArgs,someArgs);
    passCount++;
  }catch(const std::exception& e){
    failCount++;
    std::cout << "\nFAIL: Test 4 - Valid option '-f' without argument" 
              << std::endl;
    std::cerr << e.what();
  }

  try{
    /* Test for 2 command line arguments
     * option: h
     * argument: none
     * Expected result: Prints usage
     */
    noOfArgs = 2;
    strncpy( someArgs[1],"-g",2);
    parseCmdLineArgs(noOfArgs,someArgs);
    passCount++;
  }catch(const std::exception& e){
    failCount++;
    std::cout << "\nFAIL: Test 5 - Valid option '-f' without argument" 
              << std::endl;
    std::cerr << e.what();
  }

  try{
    /* Test for 2 command line arguments
     * option: g
     * argument: none
     * Expected result: Indicate that the option is invalid and print usage
     */
    noOfArgs = 2;
    strncpy( someArgs[1],"-g",2);
    parseCmdLineArgs(noOfArgs,someArgs);
    passCount++;
  }catch(const std::exception& e){
    failCount++;
    std::cout << "\nFAIL: Test 6 - Valid option '-f' without argument" 
              << std::endl;
    std::cerr << e.what();
  }


  std::cerr << "\nTesting Complete: " << failCount << " of " <<
  failCount+passCount << " tests failed" << std::endl;
	return failCount;
}


