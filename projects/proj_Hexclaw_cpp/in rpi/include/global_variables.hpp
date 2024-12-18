
#pragma once
#ifndef HPP_HEXCLAW_GLOBAL
#define HPP_HEXCLAW_GLOBAL

// #include <includes.hpp>

#include <constants.hpp>
#include <string>
#include <iostream>

#include <PiPCA9685/PCA9685.h>
#include <wirelessCOM.hpp>
#include <IR_camTrack.hpp>
#include <terminalMenu.hpp>
#include <opencv/recordVideo/recordFrames.hpp>
#include <two_cam_coordinate/two_cam_coordinate.hpp>


/// @brief absolute path of the current directory for this program
extern std::string absPath;

/// @brief pca9685-board libraries obbject
extern PiPCA9685::PCA9685 pca;

extern bool hardExit;

// IK related: ik calc variable declaration

/// @brief container array for the current/old servo motors angles
extern float current_q[6];

/// @brief new servo rotations angles
extern float new_q[6];

/**
 * `{yaw, pitch, roll}` variables:
 * unit: degrees
 * */
extern float orient[3];

/// @brief `{x, y, z}` coordinate array container
extern float PP[3];

/// @brief new axis value scalars
extern float axisScal[3];

/// @brief new axis value offset
extern float axisOffset[3];

/// @brief new axis filter
extern float axisFilter[3];

/// @brief raw tilt values from accelerometer readings
extern float pitch;
extern float roll;

/// @brief filtered tilt values from accelerometer readings
extern float Pitch;
extern float Roll;

/// @brief Wireless nodemcu udp COM header class object
extern nodemcu_orient orientObj;
//nodemcu_orient orientObj(orient, "192.168.1.177");


/// @brief prefered image size to resize/convert/use for all images
extern int prefSize[2];

/// @brief whether to use automatic brightness adjustment with opencv cam tracking
extern bool useAutoBrightne;
/// @brief whether to read and measure performance (delays/fps) for `IR_camTracking` class
extern bool takeCVTrackPerf;

/// @brief whether to display `IR_camTracking`'s images
extern bool displayToWindow;

extern bool displayTFT;


// int l_HSV[3] = {0, 0, 255};
// int u_HSV[3] = {179, 9, 255};
extern int HW_HSV[2][3];

extern const char* window_name;
// IR camtracking header class initialization
extern IR_camTracking camObj[2];


// Two_cam_triangle header class initialisation
extern float camPosition[2][2];
extern float camAng_offs[2];
extern float inpPos[2];
extern float solvedPos[2];
extern camTriangle camTri;


extern bool pigpioInitia;
extern int pin_ledRelay;

#if takePerf
//main thread performance measurement
extern getPerf perfObj[3];
#endif

extern opencv_recorder recObj;



/**
 * simple function for locking a mutex reference and printing `toPrint` to cout buffer
 * NOTE: this function will release after locking but before calling the function *make sure* the mutex object is unlocked,
 * because otherwise it'll give undefined behavious.
 * @param coutMutex `std::mutex` object reference.
 * @param toPrint the string object to print to cout.
 * @param blockingLock whether to use the blocking member function
 * `std::mutex::lock()` or the nonblocking `std::mutex::try_lock()`.
 * @param flushOut whether to flush after printing to cout
*/
void lock_cout(
	std::mutex &coutMutex,
	string toPrint,
	bool blockingLock = true,
	bool flushOut = false,
	bool startClearScr = false
);
#if useThreads
#if takePerf
//sub thread(s) performance measurements

/**
 * sub-thread perfObj mutexes
*/
extern std::mutex mtx_perfObj_threads[2];
// getPerf perfObj[2] {
// 	{"sub-thread[0]"},
// 	{"sub-thread[1]"}
// };
#endif

/** array to hold initialisation boolean values (true if it's been init, false otherwise)
 * `[0]` = cam0-thread
 * `[1]` = cam1-thread
 * `[2]` = main-thread
*/
extern bool threadsInit[3];

/// variable/container for cv::Mat display objects/img's that's used in the main thread
extern cv::Mat flippedImg_main[2];
/// @brief variable/container for intermediary cv::Mat images; temporarily holds new values from sub threads
extern cv::Mat flippedImg_interm[2];

/** main camera tracked object center position holder/container array;
 * `[0][0, 1]` = cam/thread 0;
 * `[1][0, 1]` = cam/thread 1;
*/
extern float camObjPos_main[2][2];
/**
 * intermediary container/holder array for tracked-object-center-positions received from sub-threads
 * `[0][0, 1]` = sub-thread[0]
 * `[1][0, 1]` = sub-thread[1]
*/
extern float camObjPos_interm[2][2];

/**
 * main-thread container/holder array for error exit codes from threads
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
extern int returCodes_main[2];
/**
 * sub-thread container/holder array for error exit codes
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
extern int returCodes_interm[2];

/**
 * vector of cam processing delays for each webcam
 * in an array.
*/
extern std::vector<float> camProcess_delays[2];

/**
 * vector of cam processing delay names for each webcam
 * in an array.
*/
extern std::vector<std::string> camProcess_delayNames[2];

/**
 * container array for number of contours detected, to be used in main thread
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
extern size_t numContours_main[2];
/**
 * container array for number of contours detected, to be used in sub threads
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
extern size_t numContours_interm[2];

/**
 * @brief transfer results from sub thread[`t-idx`] to `[..]_interm[t_idx]` variables
 * @param camPtr `IR_camTracking` object pointer
 * @param t_idx `int` value of which thread to use function on
*/
void transferCamVars(IR_camTracking* camPtr, int t_idx);
/**
 * @brief update main-thread `[..]_main[t-idx]` variables with sub-thread `[..]_interm[t_idx]` variables
 * @param t_idx `int` value of which thread to use function on
*/
void updateCamVars(int t_idx);
/**
 * general variable mutexes
 * `[0]` = sub-thread[0]
 * `[1]` = sub-thread[1]
*/
extern std::mutex mtx[2];

/**
 * Syncing purpose mutexes:
 * 
 * Purpose is for a sub-thread to lock its own mutex, then
 * continuously `try_lock()` and `unlock()` the other mutex to check if that sub-thread
 * has reached same point in process.
 * 
 * When both sub-threads have "hard"-locked their mutexes the
 * `try_lock()` will return false, from which each sub-thread can unlock their own mutex and run
 * the rest of the function
 * '[0]' = sub-thread[0]
 * '[1]' = sub-thread[1]
*/
extern std::mutex mtx_sync[2];

/**
 * mutex for std::iostream::cout
*/
extern std::mutex mtx_cout;

// /**mutex for saving frames onto `opencv_recorder` class object*/
// std::mutex mtx_vidRec;

extern bool exit_thread[2];

/**
 * @brief Multithread function for processing and storing image/webcam processing results
 * @param camRef `IR_camTracking` object pointer
 * @param t_idx `int` value for thread indexing/naming
*/
void thread_task(IR_camTracking* camPtr, int t_idx);
#endif


extern termMenu startMenu;
extern termMenu modeMenu;


extern termMenu opt6_startMenu;

/**
 *  _ _ _ _ _ _ _
 *  _ 0 _ 0 _ 0 _
 *  _ _ _ _ _ _ _
 *  _ 0 _ 0 _ 0 _
 *  _ _ _ _ _ _ _
 *  _ x _ _ _ e _
 *  _ _ _ _ _ _ _
 * 
*/
extern termMenu opt6_control_panel;

void exitFrom_lvl2(bool* driverBool);


#endif
