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

#include "cxTrackingImplService.h"

#include <ctkPluginContext.h>
#include "cxData.h"
#include "cxReporter.h"
#include "cxLogicManager.h"
#include "cxDataManager.h"
#include "cxPatientData.h"
#include "cxPatientService.h"
#include "cxRegistrationTransform.h"
#include "cxToolManager.h"

namespace cx
{

TrackingImplService::TrackingImplService(ctkPluginContext *context) :
	mContext(context )
{
}

TrackingImplService::~TrackingImplService()
{
}

bool TrackingImplService::isNull()
{
	return false;
}

ToolPtr TrackingImplService::getTool(const QString& uid)
{
	return this->getOld()->getTool(uid);
}

ToolPtr TrackingImplService::getActiveTool()
{
	return this->getOld()->getDominantTool();
}

void TrackingImplService::setActiveTool(const QString& uid)
{
	this->getOld()->setDominantTool(uid);
}

ToolPtr TrackingImplService::getFirstProbe()
{
	return this->getOld()->findFirstProbe();
}

TrackingServiceOldPtr TrackingImplService::getOld() const
{
	return LogicManager::getInstance()->getTrackingService();
}

ToolPtr TrackingImplService::getReferenceTool() const
{
	return this->getOld()->getReferenceTool();
}

ToolPtr TrackingImplService::getManualTool()
{
	return this->getOld()->getManualTool();
}



} /* namespace cx */
