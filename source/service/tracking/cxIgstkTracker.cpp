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

#include "cxIgstkTracker.h"

#include <QStringList>
#include "cxReporter.h"
#include "cxTypeConversions.h"
#include "cxEnumConverter.h"
#include "cxToolUsingIGSTK.h"
#include "cxIgstkTool.h"
#include <time.h>
#include "cxSettings.h"

namespace cx
{
QStringList IgstkTracker::getSupportedTrackingSystems()
{
	QStringList retval;
	retval << enum2string(tsPOLARIS);
	retval << enum2string(tsAURORA);
	return retval;
}

IgstkTracker::IgstkTracker(InternalStructure internalStructure) :
				mInternalStructure(internalStructure),
				mValid(false),
				mUid(""),
				mName(""),
				mTracker(NULL),
				mCommunication(CommunicationType::New()),
				mTrackerObserver(ObserverType::New()),
				mOpen(false),
				mInitialized(false),
				mTracking(false)
{
	mCommunication->SetPortNumber(igstk::SerialCommunication::PortNumber0);
	mCommunication->SetParity(igstk::SerialCommunication::NoParity);
	mCommunication->SetBaudRate(igstk::SerialCommunication::BaudRate115200);
	mCommunication->SetDataBits(igstk::SerialCommunication::DataBits8);
	mCommunication->SetStopBits(igstk::SerialCommunication::StopBits1);
	mCommunication->SetHardwareHandshake(igstk::SerialCommunication::HandshakeOff);

	bool logging = settings()->value("IGSTKDebugLogging", true).toBool();
	if (logging)
	{
		QString comLogging = mInternalStructure.mLoggingFolderName + "RecordedStreamByCustusX.txt";
		mCommunication->SetCaptureFileName(cstring_cast(comLogging));
		mCommunication->SetCapture(true);
	}

	switch (mInternalStructure.mType)
	{
	case tsNONE:
		mUid = mName = "None";
		reportError("Tracker is of type TRACKER_NONE, this means it's not valid.");
		mValid = false;
		return;
		break;
	case tsPOLARIS:
		mUid = mName = "Polaris";
		mTempPolarisTracker = PolarisTrackerType::New();
		mTempPolarisTracker->SetCommunication(mCommunication);
		report("Tracker is set to Polaris");
		mTracker = mTempPolarisTracker.GetPointer();
		mValid = true;
		break;
	case tsPOLARIS_CLASSIC:
		mUid = mName = "None";
		report("Polaris Classic is not supported");
		mValid = false;
		break;
		//There is no special handling of the tracking system if its spectra or vicra, polaris is polaris as we see it
	case tsAURORA:
		mUid = mName = "Aurora";
		mTempAuroraTracker = AuroraTrackerType::New();
		mTempAuroraTracker->SetCommunication(mCommunication);
		report("Tracker is set to Aurora");
		mTracker = mTempAuroraTracker.GetPointer();
		mValid = true;
		break;
	case tsMICRON:
		mUid = mName = "Micron";
		report("Tracker is set to Micron, not supported");
		//TODO: implement support for a micron tracker...
		mValid = false;
		break;
	default:
		break;
	}
	mTrackerObserver->SetCallbackFunction(this, &IgstkTracker::trackerTransformCallback);
	mTracker->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
	mCommunication->AddObserver(igstk::IGSTKEvent(), mTrackerObserver);
	this->addLogging();
}

IgstkTracker::~IgstkTracker()
{
}

TRACKING_SYSTEM IgstkTracker::getType() const
{
	return mInternalStructure.mType;
}

QString IgstkTracker::getName() const
{
	return mName;
}

QString IgstkTracker::getUid() const
{
	return mUid;
}

IgstkTracker::TrackerType* IgstkTracker::getPointer() const
{
	return mTracker;
}

void IgstkTracker::open()
{
//  igstk::SerialCommunication::ResultType result = igstk::SerialCommunication::FAILURE;
//  for(int i=0; i<5; ++i)
//  {
//    result = mCommunication->OpenCommunication();
//    if(result == igstk::SerialCommunication::SUCCESS)
//      break;
//    else
//      reportWarning("Could not open communication.");
//  }
	if (mCommunication->OpenCommunication() == false)
		reportWarning("Could not open communication.");
	mTracker->RequestOpen();
}

void IgstkTracker::close()
{
	mTracker->RequestClose();
	if (mCommunication->CloseCommunication() == false)
		reportWarning("Could not close communication.");
}

void IgstkTracker::attachTools(std::map<QString, IgstkToolPtr> tools)
{
	if (!this->isInitialized())
		return;

	for (std::map<QString, IgstkToolPtr>::iterator it = tools.begin(); it != tools.end(); ++it)
	{
		IgstkToolPtr tool = it->second;

		if (tool && tool->getPointer())
		{
			if (tool->getTrackerType() != mInternalStructure.mType)
				reportWarning(
								"Tracker is attaching a tool that is not of the correct type. Trackers type: "
												+ qstring_cast(mInternalStructure.mType) + ", tools tracker type: "
												+ qstring_cast(tool->getTrackerType()));

			tool->getPointer()->RequestAttachToTracker(mTracker);

			if (tool->isReference())
				mTracker->RequestSetReferenceTool(tool->getPointer());
//      if(tool->getType() == Tool::TOOL_REFERENCE)
//        mTracker->RequestSetReferenceTool(tool->getPointer());
		}
	}
}

void IgstkTracker::detachTools(std::map<QString, IgstkToolPtr> tools)
{
	if (!this->isInitialized())
		return;

	for (std::map<QString, IgstkToolPtr>::iterator it = tools.begin(); it != tools.end(); ++it)
	{
		IgstkToolPtr tool = it->second;

		if (tool && tool->getPointer())
		{
			tool->getPointer()->RequestDetachFromTracker();
		}
	}
}

void IgstkTracker::startTracking()
{
	mTracker->RequestStartTracking();
}

void IgstkTracker::stopTracking()
{
	mTracker->RequestStopTracking();
}

bool IgstkTracker::isValid() const
{
	return mValid;
}

bool IgstkTracker::isOpen() const
{
	return mOpen;
}

bool IgstkTracker::isInitialized() const
{
	return mInitialized;
}

bool IgstkTracker::isTracking() const
{
	return mTracking;
}

void IgstkTracker::trackerTransformCallback(const itk::EventObject &event)
{
	//successes
	if (igstk::TrackerOpenEvent().CheckEvent(&event))
	{
		this->internalOpen(true);
		this->internalInitialized(true);
	}
	else if (igstk::TrackerCloseEvent().CheckEvent(&event))
	{
		this->internalOpen(false);
		this->internalInitialized(false);
	}
	else if (igstk::TrackerInitializeEvent().CheckEvent(&event))
	{
		//Never happens???
		//this->internalInitialized(true);
		//report(mUid+" is initialized.");
		reportWarning("This never happens for some reason...  check code");
	}
	else if (igstk::TrackerStartTrackingEvent().CheckEvent(&event))
	{
		this->internalTracking(true);
	}
	else if (igstk::TrackerStopTrackingEvent().CheckEvent(&event))
	{
		this->internalTracking(false);
	}
	else if (igstk::TrackerUpdateStatusEvent().CheckEvent(&event))
	{
		//reportDebug(mUid+" is updated."); //SPAM!
	}
	else if (igstk::TrackerToolTransformUpdateEvent().CheckEvent(&event))
	{
		//reportDebug(mUid+" has updated a transform."); //SPAM
	}
	//communication success
	else if (igstk::CompletedEvent().CheckEvent(&event))
	{
		// this seems to appear after every transmit (several times/second)
		//report(mUid+" set up communication correctly."); //SPAM
	}
	//coordinate system success
	else if (igstk::CoordinateSystemSetTransformEvent().CheckEvent(&event))
	{
		//report();
	}
	//failures
	else if (igstk::InvalidRequestErrorEvent().CheckEvent(&event))
	{
		reportWarning(
						mUid
										+ " received an invalid request. This means that the internal igstk tracker did not accept the request. Do not know which request.");
		this->shutdown();
	}
	else if (igstk::TrackerOpenErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not open.");
		//this->shutdown();
	}
	else if (igstk::TrackerCloseErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not close.");
		//this->shutdown();
	}
	else if (igstk::TrackerInitializeErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not initialize.");
		//this->shutdown();
	}
	else if (igstk::TrackerStartTrackingErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not start tracking.");
		//this->shutdown();
	}
	else if (igstk::TrackerStopTrackingErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not stop tracking.");
		//this->shutdown();
	}
	else if (igstk::TrackerUpdateStatusErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not update.");
		//this->shutdown();
	}
	//communication failure
	else if (igstk::InputOutputErrorEvent().CheckEvent(&event))
	{
		//this happens when you pull out the cable while tracking
		reportError(mUid + " cannot communicate with input/output.");
		this->shutdown();
	}
	else if (igstk::InputOutputTimeoutEvent().CheckEvent(&event))
	{
		reportError(mUid + " input/output communication timed out.");
		//this->shutdown();
	}
	else if (igstk::OpenPortErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not open communication with tracker.");
		this->shutdown();
	}
	else if (igstk::ClosePortErrorEvent().CheckEvent(&event))
	{
		reportError(mUid + " could not close communication with tracker.");
		this->shutdown();
	}
	else
	{
		event.Print(std::cout);
	}
}

void IgstkTracker::addLogging()
{
	bool logging = settings()->value("IGSTKDebugLogging", true).toBool();
	if (logging)
	{
		std::ofstream* loggerFile = new std::ofstream();
		QString logFile = mInternalStructure.mLoggingFolderName + "Tracker_Logging.txt";
		loggerFile->open(cstring_cast(logFile));
		mTrackerLogger = igstk::Logger::New();
		mTrackerLogOutput = itk::StdStreamLogOutput::New();
		mTrackerLogOutput->SetStream(*loggerFile);
		mTrackerLogger->AddLogOutput(mTrackerLogOutput);
		mTrackerLogger->SetPriorityLevel(itk::Logger::DEBUG);

		mTracker->SetLogger(mTrackerLogger);
		mCommunication->SetLogger(mTrackerLogger);
	}
}

void IgstkTracker::internalOpen(bool value)
{
	if (mOpen == value)
		return;
	mOpen = value;

	report(mUid + " is " + (value ? "open" : "closed") + ".");
	emit open(mOpen);
}

void IgstkTracker::internalInitialized(bool value)
{
	if (mInitialized == value)
		return;
	mInitialized = value;

	report(mUid + " is " + (value ? "" : "un") + "initialized.");
	emit initialized(mInitialized);
}

void IgstkTracker::internalTracking(bool value)
{
	if (mTracking == value)
		return;
	mTracking = value;

	report(mUid + " is " + (value ? "" : "not ") + "tracking.");
	emit tracking(mTracking);
}

void IgstkTracker::internalError(bool value)
{
	reportWarning(mUid + " experienced a unrecoverable error, reconfiguration is required.");
	emit error();
}

void IgstkTracker::shutdown()
{
	mCommunication->CloseCommunication();

	//because the tracker now is closed we don't get the callback events so we need to reset the trackers internal
	//status manually
	this->internalTracking(false);
	this->internalInitialized(false);
	this->internalOpen(false);
	this->internalError(true);
}

} //namespace cx
