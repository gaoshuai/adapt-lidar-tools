/*
* File name: csv_CmdLine_unittests.cpp
* Created on: 16-July-2019
* Author: ravi
*/

#include "csv_CmdLine.hpp"
#include "gtest/gtest.h"
#include <getopt.h>
#include <iostream>
#include <fstream>


class csv_CmdLineTest : public testing::Test {
    protected:

        virtual void SetUp(){
            numberOfArgs = 10;
            maxLengthOfArg = 256;
            commonArgSpace = allocateTestArgs(numberOfArgs,maxLengthOfArg);
            //Set required arguments
            strncpy(commonArgSpace[0],"test",5);
            strncpy(commonArgSpace[1],"-f",3);
            strncpy(commonArgSpace[2],"do_not_use.pls",15);
            strncpy(commonArgSpace[3],"-p",3);
            strncpy(commonArgSpace[4],"1",2);
            //Stifle command line error messages
            cmd.quiet = true;
            //Create test files
            std::ofstream pls ("do_not_use.pls");
            pls.close();
            std::ofstream wvs ("do_not_use.wvs");
            wvs.close();
        }

        static char** allocateTestArgs(int N,int M){
            char** someArgs = (char**)malloc(sizeof(char*)*N);
            for(int i=0;i<10;i++){
                someArgs[i] = (char*)malloc(sizeof(char)*M);
            }
            return someArgs;
        }

        void TearDown()  {
            for (int i=0 ;i<10;i++){
                free(commonArgSpace[i]);
            }
            free(commonArgSpace);
            //Delete test files
            std::remove("do_not_use.pls");
            std::remove("do_not_use.wvs");
        }

        char** commonArgSpace;
        int numberOfArgs;
        int maxLengthOfArg;
        csv_CmdLine cmd;
        csv_CmdLine cmd2;
        csv_CmdLine cmd3;

};

//Tests no inputted arguments
TEST_F(csv_CmdLineTest, noArguments){
    optind = 0;
    numberOfArgs = 1;
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

/****************************************************************************
 *
 * File Name Tests
 *
 ****************************************************************************/
// Tests file was correctly set
TEST_F(csv_CmdLineTest, validFileName){
    //This also tests file trimming for files in the same path
    optind = 0;
    numberOfArgs = 5;
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
    ASSERT_EQ("do_not_use.pls", cmd.getInputFileName(true));
    ASSERT_EQ("do_not_use.wvs", cmd.getInputFileName(false));
}

//Tests file was incorrectly set
TEST_F(csv_CmdLineTest, invalidFileName){
    //Delete pls file
    std::remove("do_not_use.pls");

    //Invalid pls
    optind = 0;
    numberOfArgs = 5;
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    //Delete wvs file and create pls file
    std::remove("do_not_use.wvs");
    std::ofstream pls ("do_not_use.pls");
    pls.close();

    //Invalid wvs
    optind = 0;
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    //No file
    optind = 0;
    numberOfArgs = 4;
    strncpy(commonArgSpace[2],"-e",3);
    strncpy(commonArgSpace[3],"1",2);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

//Tests file was correctly trimmed with various paths
TEST_F(csv_CmdLineTest, fileTrimmingTestPath){
    numberOfArgs = 5;

    //Same path, child path, sister path
    std::vector<std::string> paths = {"./", "etc/"};
    for (auto it = paths.begin(); it != paths.end(); ++it){
        //Create test files
        std::ofstream pls2 (*it + std::string("do_not_use.pls"));
        pls2.close();
        std::ofstream wvs2 (*it + std::string("do_not_use.wvs"));
        wvs2.close();

        optind = 0;
        strncpy (commonArgSpace[2],
            (*it + std::string("do_not_use.pls")).c_str(),15 + it->length());
        ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
        ASSERT_FALSE(cmd.printUsageMessage);
        ASSERT_EQ("do_not_use", cmd.getTrimmedFileName(true));
        ASSERT_EQ("do_not_use", cmd.getTrimmedFileName(false));
  
        //Delete test files
        std::remove((*it + std::string("do_not_use.pls")).c_str());
        std::remove((*it + std::string("do_not_use.wvs")).c_str());
    }
}

//Tests file was correctly trimmed with odd file names
TEST_F(csv_CmdLineTest, fileTrimmingTestName){
    //Create test files
    std::ofstream pls2 ("do.not.use.pls");
    pls2.close();
    std::ofstream wvs2 ("do.not.use.wvs");
    wvs2.close();

    optind = 0;
    numberOfArgs = 5;
    strncpy (commonArgSpace[2],"do.not.use.pls",15);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
    ASSERT_EQ("do.not.use", cmd.getTrimmedFileName(true));
    ASSERT_EQ("do.not.use", cmd.getTrimmedFileName(false));

    //Delete test files
    std::remove("do.not.use.pls");
    std::remove("do.not.use.wvs");
}

//Tests the setInputFileName function against valid cases
TEST_F(csv_CmdLineTest, setInputFileNameTest_valid) {
    //Test file names
    char f0[15] = "do.not.use.pls";
    char f1[15] = "do.not.use.wvs";
    char f2[15] = "do.not.use.txt";

    //Create test files
    std::ofstream pls (f0);
    pls.close();
    std::ofstream wvs (f1);
    wvs.close();
    std::ofstream txt (f2);
    txt.close();

    //Run tests
    EXPECT_NO_THROW(cmd.setInputFileName(f0));
    EXPECT_NO_THROW(cmd.setInputFileName(f1));
        EXPECT_NO_THROW(cmd.setInputFileName(f2));

    std::remove("do.not.use.pls");
    std::remove("do.not.use.wvs");
    std::remove("do.not.use.txt");
}

/****************************************************************************
 *
 * Product Option Tests
 *
 ****************************************************************************/
//Tests valid product variable char
TEST_F(csv_CmdLineTest, validProductOption){
    //Single argument options
    numberOfArgs = 5;
    std::vector<std::string> vars = {"-p"};
    for (auto it = vars.begin(); it != vars.end(); ++it){
        optind = 0;
        strncpy(commonArgSpace[3],(*it).c_str(),3);
        ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
        ASSERT_FALSE(cmd.printUsageMessage);
    }
}

//Tests invalid product variable char
TEST_F(csv_CmdLineTest, invalidProductOption){
    //Invalid character
    optind = 0;
    numberOfArgs = 5;
    strncpy(commonArgSpace[3],"-g",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

//Tests for an invalid product number
TEST_F(csv_CmdLineTest, invalidProductNumber){
    //No product number
    optind = 0;
    numberOfArgs = 4;
    strncpy(commonArgSpace[4],"",0);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    //Invalid product number
    optind = 0;
    numberOfArgs = 5;
    strncpy(commonArgSpace[4],"19",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    optind = 0;
    strncpy(commonArgSpace[4],"0",2);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
  
    optind = 0;
    strncpy(commonArgSpace[4],"-1",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    optind = 0;
    strncpy(commonArgSpace[4],"3000000000000",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

/****************************************************************************
 *
 * Non-Product Option Tests
 *
 ****************************************************************************/
//Tests for a valid non-product option
TEST_F(csv_CmdLineTest, validNonProductOption){
    //Shouldn't trigger usage message
    optind = 0;
    numberOfArgs = 6;
    strncpy(commonArgSpace[5],"-d",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
    ASSERT_FALSE(cmd.useGaussianFitting);

    //Should trigger usage message
    optind = 0;
    strncpy(commonArgSpace[5],"-h",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

//Tests for an invalid non-product option
TEST_F(csv_CmdLineTest, invalidNonProductOption){
    optind = 0;
    numberOfArgs = 6;
    strncpy(commonArgSpace[5],"-p",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

/****************************************************************************
 *
 * Long Option Tests
 *
 ****************************************************************************/
//Tests valid long file option
TEST_F(csv_CmdLineTest, validLongFileOption){
    //--file test
    optind = 0;
    numberOfArgs = 5;
    strncpy(commonArgSpace[1],"--file",7);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
}

//Tests valid long product options
TEST_F(csv_CmdLineTest, validLongProductOption){
    //long product options
    //single argument
    numberOfArgs = 5;
    std::vector<std::string> opts = {"--peaks"};
    for (auto it = opts.begin(); it != opts.end(); ++it){
        optind = 0;
        strncpy(commonArgSpace[3],(*it).c_str(),it->length() + 1);
        ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
        ASSERT_FALSE(cmd.printUsageMessage);
    }
}

//Tests valid lon non-product options
TEST_F(csv_CmdLineTest, validLongNonProductOption){
    //Shoudln't trigger usage message
    optind = 0;
    numberOfArgs = 6;
    strncpy(commonArgSpace[5],"--firstdiff",12);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
    ASSERT_FALSE(cmd.useGaussianFitting);

    //Should trigger usage message
    optind = 0;
    strncpy(commonArgSpace[5],"--help",7);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

//Tests missing arguments for long options
TEST_F(csv_CmdLineTest, validLongOptionMissingArgument){
    //single argument product option
    //invalid product number
    optind = 0;
    strncpy(commonArgSpace[4],"100",4);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    //missing product number
    optind = 0;
    numberOfArgs = 4;
    strncpy(commonArgSpace[3],"--peaks",12);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);

    //missing file
    optind = 0;
    strncpy(commonArgSpace[1],"--file",8);
    strncpy(commonArgSpace[2],"-p",3);
    strncpy(commonArgSpace[3],"1",2);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

//Tests invalid long option
TEST_F(csv_CmdLineTest, invalidLongOption){
    optind = 0;
    numberOfArgs = 6;
    strncpy(commonArgSpace[5],"--oops",7);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_TRUE(cmd.printUsageMessage);
}

/****************************************************************************
 *
 * Output filename tests
 *
 ****************************************************************************/
//Tests correct naming of method
TEST_F(csv_CmdLineTest, outputFileNameMethod){
    optind = 0;
    numberOfArgs = 5;
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
    ASSERT_EQ("do_not_use_Amplitude_gaussian.csv",
            cmd.get_output_filename(1));

    optind = 0;
    numberOfArgs = 6;
    strncpy(commonArgSpace[5],"-d",3);
    ASSERT_NO_THROW(cmd.parse_args(numberOfArgs,commonArgSpace));
    ASSERT_FALSE(cmd.printUsageMessage);
    ASSERT_EQ("do_not_use_Amplitude_firstDiff.csv",
            cmd.get_output_filename(1));
}

/* Call RUN_ALL_TESTS() in main().

   We do this by linking in src/gtest_main.cc file, which consists of
   a main() function which calls RUN_ALL_TESTS() for us.

   This runs all the tests defined, prints the result, and
   returns 0 if successful, or 1 otherwise.

*/
