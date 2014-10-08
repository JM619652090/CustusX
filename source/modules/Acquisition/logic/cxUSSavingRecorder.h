/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#ifndef CXUSSAVINGRECORDER_H
#define CXUSSAVINGRECORDER_H

#include "cxPluginAcquisitionExport.h"

#include <vector>
#include <QFutureWatcher>
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"

namespace cx
{
struct USReconstructInputData;
}
namespace cx
{
typedef boost::shared_ptr<class UsReconstructionFileMaker> UsReconstructionFileMakerPtr;
typedef boost::shared_ptr<class SavingVideoRecorder> SavingVideoRecorderPtr;
typedef boost::shared_ptr<class RecordSession> RecordSessionPtr;

/**
 * \file
 * \addtogroup cx_module_acquisition
 * @{
 */


/**
 * \brief Record and save ultrasound data.
 * \ingroup cx_module_acquisition
 *
 * Use the start/stop pair to record video from the input streams
 * during that period. A cancel instead of stop will clear the recording.
 *
 * After stopping, use
 *    - getDataForStream() to get unsaved reconstruct data.
 *    - startSaveData() to launch save threads, emitting signals for each completed save.
 *
 * Use clearRecording() to free memory and temporary files (this can be a lot of disk space).
 *
 * Intended to be a unit-testable part of the USAcquisition class.
 *
 *  \date April 17, 2013
 *  \author christiana
 */
class cxPluginAcquisition_EXPORT USSavingRecorder : public QObject
{
	Q_OBJECT
public:
	USSavingRecorder();
	virtual ~USSavingRecorder();
	/**
	  * Start recording
	  */
	void startRecord(RecordSessionPtr session, ToolPtr tool, std::vector<VideoSourcePtr> video);
	void stopRecord();
	void cancelRecord();

	void setWriteColor(bool on);
	void set_rMpr(Transform3D rMpr);
	/**
	  * Retrieve an in-memory data set for the given stream uid.
	  */
	USReconstructInputData getDataForStream(QString streamUid);
	/**
	  * Start saving all data acquired after a start/stop record.
	  * A separate saveDataCompleted() signal is emitted
	  * for each completed saved stream.
	  */
	void startSaveData(QString baseFolder, bool compressImages);
	size_t getNumberOfSavingThreads() const;
	void clearRecording();

signals:
	void saveDataCompleted(QString mhdFilename); ///< emitted when data has been saved to file

private slots:
	void fileMakerWriteFinished();
private:
//	std::map<double, Transform3D> getToolHistory(ToolPtr tool, RecordSessionPtr session);
	void saveStreamSession(USReconstructInputData reconstructData, QString saveFolder, QString streamSessionName, bool compress);
	USReconstructInputData getDataForStream(unsigned videoRecorderIndex);

	RecordSessionPtr mSession;
	std::list<QFutureWatcher<QString>*> mSaveThreads;
	// video and tool used at start of recording:
	std::vector<SavingVideoRecorderPtr> mVideoRecorder;
	ToolPtr mRecordingTool;
	bool mDoWriteColor;
	Transform3D m_rMpr;
};
typedef boost::shared_ptr<USSavingRecorder> USSavingRecorderPtr;

/**
* @}
*/
}

#endif // CXUSSAVINGRECORDER_H
