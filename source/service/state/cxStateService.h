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

#ifndef CXSTATESERVICE_H_
#define CXSTATESERVICE_H_

#include "cxStateServiceExport.h"

#include <QDomNode>

#include "cxForwardDeclarations.h"
#include <QObject>

namespace cx
{
typedef boost::shared_ptr<class ApplicationStateMachine> ApplicationStateMachinePtr;
typedef boost::shared_ptr<class WorkflowStateMachine> WorkflowStateMachinePtr;
typedef boost::shared_ptr<class StateServiceBackend> StateServiceBackendPtr;

/**
 * \file
 * \addtogroup cx_service_state
 * @{
 */

/**
 * \brief Data class for CustusX desktop.
 *
 * Contains info about widget and toolbar info in the MainWindow,
 * and the Layout ID.
 *
 * \sa StateService
 *
 */
struct cxStateService_EXPORT Desktop
{
	Desktop();
	Desktop(QString layout, QByteArray mainwindowstate);

	QString mLayoutUid;
	QString mSecondaryLayoutUid;
	QByteArray mMainWindowState;
};

/**
 * \brief Global Application states for CustusX.
 *
 * The StateManager is the main class in \ref cx_service_state , and maintains
 * global state.
 *
 * Exception: \ref cx_resource_core_settings also contains some global state.
 *
 * \image html cxArchitecture_application.png "State Service main classes"
 *
 *
 *
 * \section cx_section_clinical_application_state Clinical Application State
 *
 * The active clinical application is known to the entire system.
 * The options are:
 *
 *  - Laboratory
 *  - Neurology
 *  - Laparascopy
 *  - Bronchoscopy
 *  - Endovascular
 *
 * The application can be used to customize the system to that clinical
 * area. Each one are represented as a state in StateManager. Currently the
 * tool configurations, the workflow states and the view orientations are
 * affected by this.
 *
 * \sa ApplicationStateMachine
 *
 *
 * \section cx_section_workflow Workflow
 *
 * The use of CustusX during a surgical procedure can be broken down into a series of steps:
 *
 *  - Preoperative data acquisition
 *  - Planning
 *  - Registration
 *  - Navigation + Intraoperative data acquisition.
 *  - Postoperative analysis
 *
 * This is the main workflow. Each step is named a Workflow State, and are
 * states in StateManager. They can be customized in the State Manager, and
 * they have a unique GUI setup.
 *
 * \image html workflow_steps_small.png "Workflow steps. The buttons are enlarged."
 *
 * \sa WorkflowStateMachine
 *
 * During each step, There are operations that usually are performed in
 * sequence. Examples:
 *
 *  - Acquire data -> reconstruct.
 *  - Import/acquire data -> show in view.
 *
 * These automations can be set in Preferences->Automation.
 *
 *
 *
 * \date 4. aug. 2010
 * \author Janne Beate Bakeng, SINTEF
 */
class cxStateService_EXPORT StateService: public QObject
{
Q_OBJECT

public:
	static StateServicePtr create(StateServiceBackendPtr backend);
	virtual ~StateService();

//	static StateService* createInstance(StateServiceBackendPtr backend);
//	static StateService* getInstance(); ///< returns the only instance of this class
//	static void destroyInstance(); ///< destroys the only instance of this class

	QString getVersionName();

	WorkflowStateMachinePtr getWorkflow();
	ApplicationStateMachinePtr getApplication();

	Desktop getActiveDesktop();
	void saveDesktop(Desktop desktop);
	void resetDesktop();

	QStringList getOpenIGTLinkServer();

private:
	StateService();

	void initialize(StateServiceBackendPtr backend); ///< init stuff that is dependent of the statemanager
	void fillDefaultSettings();
	template<class T>
	void fillDefault(QString name, T value);

	QStringList getDefaultGrabberServer();
	QStringList getGrabberServer(QString filename, QString postfix);
	QStringList checkGrabberServerExist(QString path, QString filename, QString args);
	QString getDefaultGrabberInitScript();

//	static StateService* mTheInstance; ///< the only instance of this class

	WorkflowStateMachinePtr mWorkflowStateMachine;
	ApplicationStateMachinePtr mApplicationStateMachine;
	StateServiceBackendPtr mBackend;

};

//StateService* stateService();

/**
 * @}
 */
}
#endif /* CXSTATESERVICE_H_ */
