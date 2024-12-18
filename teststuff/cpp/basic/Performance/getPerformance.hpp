#pragma once
#ifndef H_GET_PERFORMANCE
#define H_GET_PERFORMANCE

#include <iostream>
#include <time.h>
#include <chrono>
#include <ctime>
#include <math.h>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <HC_useful/useful.hpp>

using namespace std;

// / @brief Class to get performance (fps, delays for each checkpoint, totaldelay)
// /
// / 
class getPerf {
    /*
    t0
    -process
    t1; delay = t1-t([1]-1=0)
    
    */
    private:
    /// @brief max number of chars for each checkpoint name
    int strLenMax = 25; //NOTE: if changed, change update_totalInfo printf


    /// @brief whether to save the data into csv file
    bool CSV_save = false;

    /// @brief filename of csv file with path included
    string CSV_filename;

    /// @brief csv file object
    ofstream csvFile;

    int CSV_lineCount = 0;

    bool CSV_init = true;

    public:
    /// @brief filter for new delay values: old_val = new_val*delayFilter + old_val*(1-delayFilter)
    float delayFilter = 1;
    bool printNames = true;
    /// @brief vector to hold names of each checkpoint
    vector<string> names;

    /// @brief vector to hold clock times
    vector<decltype(chrono::steady_clock::now())> times;
    /// @brief vector to hold delays in unit:milliseconds
    vector<float> delays_ms;

    /// @brief total/sum delay in milliseconds of all the checkpoints
    float total_delay;
    /// @brief total FPS from start-end checkpoint
    float FPS;

    /// @brief [0] final print string (don't remember why i ahve two elements and at this point im too afraid to check)
    string rawPrintStrings[2];

    getPerf(string nameInitStr="") {
        names.push_back(nameInitStr+"_t0");
        times.push_back(chrono::steady_clock::now());
        delays_ms.push_back(0);
    }

    /**
     * @brief setup csv related
     * @param filename name of csvfile to create: NOTE: DO NOT ADD `.csv` EXTENSION
     * @param overwrite whether to overwrite `{filename}.csv` if that filename'd file already exists
    */
    void csv_setup(
        string filename,
        bool overwrite=false
    );
    /**
     * @brief
     * Creates a time measuring point from previous call. NOTE: call class member AFTER what to measure.
     * if `name` exist then function will not create new element but only update clock_t vector
     * @param name string name of checkpoint to either create or update
    */
    void add_checkpoint(string name);
    /**
     * @brief update total_delay and FPS member variables
     * @param reset_t0 whether to 
     * @param printResult whether to print total_delay and FPS
     * @param printAll whether to print every checkpoint
    */
    string update_totalInfo(
        bool reset_t0,
        bool printResult = true,
        bool printAll = true,
        char resultEndSymb0 = '\n',
        char resultEndSymb1 = '\n'
    );
    /**
     * @brief Function to get clock_t type variable from given checkpoint name
     * @param name the name of checkpoint to get time from
     * @return clock_t type variable
    */
    auto getTime(string name);
    /**
     * @brief Function to get last read delay from given checkpoint name
     * @param name name of checkpoint to get delay from
     * @return float type variable of delay in unit:Milliseconds
    */
    float getDelay(string name);
    string cutStr(string& var, int maxLen=10);
    /**
     * @brief get vector index of name
     * @param name string of the `{name}` to find index of
     * @return index of `{name}`
    */
    int getIdx(string name);
    void close();
};


#endif