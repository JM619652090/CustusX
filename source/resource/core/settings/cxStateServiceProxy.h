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

#ifndef CXSTATESERVICEPROXY_H
#define CXSTATESERVICEPROXY_H

#include "cxStateService.h"
#include "cxServiceTrackerListener.h"

namespace cx
{

/**
 * \ingroup org_custusx_core_state
 */
class cxResource_EXPORT StateServiceProxy: public StateService
{
Q_OBJECT

public:
	static StateServicePtr create(ctkPluginContext *pluginContext);
	StateServiceProxy(ctkPluginContext *pluginContext);
	virtual ~StateServiceProxy() {}

	virtual QString getVersionName();
	virtual QActionGroup* getApplicationActions();
	virtual QString getApplicationStateName() const;
	virtual QStringList getAllApplicationStateNames() const;

	virtual QActionGroup* getWorkflowActions();
	virtual void setWorkFlowState(QString uid);

	virtual Desktop getActiveDesktop();
	virtual void saveDesktop(Desktop desktop);
	virtual void resetDesktop();

	virtual bool isNull();

private:
	void initServiceListener();
	void onServiceAdded(StateService* service);
	void onServiceRemoved(StateService *service);

	ctkPluginContext *mPluginContext;
	StateServicePtr mService;
	boost::shared_ptr<ServiceTrackerListener<StateService> > mServiceListener;
};

}

#endif // CXSTATESERVICEPROXY_H