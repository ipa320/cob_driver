/*
 * Copyright 2017 Fraunhofer Institute for Manufacturing Engineering and Automation (IPA)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0

 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef _ElmoRecorder_H
#define _ElmoRecorder_H

#include <string>
#include <cob_canopen_motor/SDOSegmented.h>

class CanDriveHarmonica;

/**
 * The in Elmo drives integrated Recorder allows the user to record drive information at a high frequency.
 *
 * The Recorder firstly has to be configured properly. It can be started immidiately after configuring or later, triggered by a signal (e.g. a begin-motion-command).
 * When the recording is finished, you can read out the data via CANopen using a segmented SDO transfer. Appropriate functions for this CANopen specific process are provided by the CanDriveHarmonica class.
 * This class brings all the functions to use the Elmo Recorder in a comfortable way.
 */
class ElmoRecorder {
	public:
		/**
		* @param pParentHarmonicaDrive This pointer is used to give ElmoRecorder the ability to take use of CANopen functions of CanDriveHarmonica
		*/
		ElmoRecorder(CanDriveHarmonica * pParentHarmonicaDrive);

		~ElmoRecorder();

		/**
		* Processes the collected Elmo Recorder data and saves them into a logfile.
		*/
		int processData(segData& SDOData);

		/**
		* Configures the Elmo Recorder to log internal data at a high frequency
		* (This can be used for identification of the drive chain)
		* You can read the data out after recording process has finished, using readoutRecorderTry().
		* @param iRecordingGap iRecordingGap = N indicates that a new sample should be taken once per N time quanta (= 4 * 90 usec)
		* @param driveID Enter the drive ID of the current motor, to later identify the logfiles
		* @param startImmediately 1: start the recording process immediately, 0: start recording at next BG (begin motion) command
		* @see readoutRecorderTry()
		*/
		int configureElmoRecorder(int iRecordingGap, int driveID, int startImmediately = 1);

		/**
		* @param initNow Enter true to set the initialization state to true, enter false to only request the state.
		* @return Return the initialization state of the recorder.
		*/
		bool isInitialized(bool initNow);

		/**
		* Start point of the actual read-out process. The following flow looks as follows: Request SR -> readoutRecorderTryStatus() If recorder ready -> readoutRecorder() start SDO read-out -> when finished -> processData() process data ->  logToFile() save to file.
		* This function takes use of the SR (status register) that tells the recorder state. In this function, the SR is requested.
		*/
		int readoutRecorderTry(int iObjSubIndex);

		/**
		* This function is called, when a new SR message is received. If the Recorder is finished and ready to upload data, readoutRecorder() is called to actually start the read-out process.
		* @param iStatusReg Status register received from the binary interpreter response.
		*/
		int readoutRecorderTryStatus(int iStatusReg, segData& SDOData);

		/**
		* @param sLogFileprefix Path (to an existing directory!) and file-prefix for the created logfile. It is extended with _MotorNumber_RecordedSource.log
		*/
		int setLogFilename(std::string sLogFileprefix);

	private:
		/**
		* Stores the targeted object from the time of requesting the read-out to the actual begin after "Recorder has finished" confirmation by SR
		*/
		int m_iCurrentObject;

		float m_fRecordingStepSec;

		std::string m_sLogFilename;

		/**
		* A flag that tells, whether we are waiting for read-out until the confirmation by SR, that the recorder is ready for read-out
		*/
		int m_iReadoutRecorderTry;

		CanDriveHarmonica* m_pHarmonicaDrive;

		/**
		* Hold the drive ID to later identify the logfiles (in the same directory)
		*/
		int m_iDriveID;

		/**
		* Elmo Recorder is intialized from the time of the first configuration on.
		*/
		bool m_bIsInitialized;

		/**
		* @param iObjSubIndex Requests the SDO Upload of the recorder object 0x2030 of the selected iObjSubIndex-source
		*/
		int readoutRecorder(int iObjSubIndex);

		/**
		* After processing the collected Recorder data log it to a file.
		* @param vtValues[] A 2 x N vector with a time stamp in the first column and the according data point value in teh second
		* @param filename Path and file-prefix to an existing directory! It is extended with _MotorNumber_RecordedSource.log
		*/
		int logToFile(std::string filename, std::vector<float> vtValues[]);

		/**
		* Convert the 32bit binary representation of a float to an actual 32bit float value
		*/
		float convertBinaryToFloat(unsigned int binaryRepresentation);

		/**
		* Convert the 16bit binary representation of a float to an actual 16bit (half)float value
		*/
		float convertBinaryToHalfFloat(unsigned int iBinaryRepresentation);
};

#endif
