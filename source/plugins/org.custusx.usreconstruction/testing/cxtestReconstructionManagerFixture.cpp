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

#include "cxtestReconstructionManagerFixture.h"

#include "catch.hpp"
#include <QApplication>
#include "cxDoubleProperty.h"
#include "cxTimedAlgorithm.h"

#include "cxBoolProperty.h"

#include "cxDataLocations.h"
#include "cxReconstructPreprocessor.h"
#include "cxReconstructionExecuter.h"
#include "cxLogicManager.h"
#include "cxLogger.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxUsReconstructionServiceProxy.h"
#include "cxPatientModelServiceProxy.h"
#include "cxSessionStorageService.h"


namespace cxtest
{

ReconstructionManagerTestFixture::ReconstructionManagerTestFixture() :
	mViewService(cx::ViewService::getNullObject())
{
	mVerbose = false;
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	QString folder = cx::DataLocations::getTestDataPath() + "/temp/test.cx3";
	cx::logicManager()->getSessionStorageService()->load(folder);

	ctkPluginContext *pluginContext = cx::logicManager()->getPluginContext();
	mPatientModelService = cx::PatientModelServiceProxy::create(pluginContext);
}

ReconstructionManagerTestFixture::~ReconstructionManagerTestFixture()
{
	cx::LogicManager::shutdown();
}

void ReconstructionManagerTestFixture::setPNN_InterpolationSteps(int value)
{
	cx::UsReconstructionServicePtr manager = this->getManager();
	QDomElement algo = manager->getSettings().getElement("algorithms", "pnn");
	cx::ReconstructionMethodService* algorithm = manager->createAlgorithm();
	REQUIRE(algorithm);

	std::vector<cx::PropertyPtr> adaptors = algorithm->getSettings(algo);
	cx::PropertyPtr adapter = cx::Property::findProperty(adaptors, "interpolationSteps");
	if(adapter)
		adapter->setValueFromVariant(value);
	else
		cx::reportError("Could not find adapter interpolationSteps");
}

void ReconstructionManagerTestFixture::reconstruct()
{
	mOutput.clear();
	cx::UsReconstructionServicePtr reconstructer = this->getManager();
	bool createBModeWhenAngio = reconstructer->getParam("Dual Angio")->getValueAsVariant().toBool();

	cx::ReconstructionExecuterPtr executer(new cx::ReconstructionExecuter(mPatientModelService, mViewService));

	executer->startNonThreadedReconstruction(reconstructer->createAlgorithm(),
			reconstructer->createCoreParameters(),
			reconstructer->getSelectedFileData(),
			createBModeWhenAngio);

	mOutput = executer->getResult();
}

void ReconstructionManagerTestFixture::threadedReconstruct()
{
	mOutput.clear();
	cx::UsReconstructionServicePtr reconstructer = this->getManager();

	cx::ReconstructionExecuterPtr executer(new cx::ReconstructionExecuter(mPatientModelService, mViewService));

	bool createBModeWhenAngio = reconstructer->getParam("Dual Angio")->getValueAsVariant().toBool();
	executer->startReconstruction(reconstructer->createAlgorithm(),
			reconstructer->createCoreParameters(),
			reconstructer->getSelectedFileData(),
			createBModeWhenAngio);
	cx::TimedAlgorithmPtr thread = executer->getThread();

	QObject::connect(thread.get(), SIGNAL(finished()), qApp, SLOT(quit()));
	qApp->exec();

	REQUIRE(thread->isFinished());
	REQUIRE(!thread->isRunning());

	mOutput = executer->getResult();
}

std::vector<cx::ImagePtr> ReconstructionManagerTestFixture::getOutput()
{
	return mOutput;
}

SyntheticVolumeComparerPtr ReconstructionManagerTestFixture::getComparerForOutput(SyntheticReconstructInputPtr input, int index)
{
	SyntheticVolumeComparerPtr comparer(new SyntheticVolumeComparer());
	comparer->setVerbose(this->getVerbose());
	comparer->setPhantom(input->getPhantom());
	comparer->setTestImage(this->getOutput()[index]);
	return comparer;
}

cx::UsReconstructionServicePtr ReconstructionManagerTestFixture::getManager()
{
	if (!mManager)
	{
//		cx::XmlOptionFile settings;
//		cx::UsReconstructionServicePtr reconstructer(new cx::ReconstructionManager(settings,""));

		//TODO: mocking UsReconstructionService with null object is probably not enough
//		cx::UsReconstructionServicePtr reconstructer(cx::UsReconstructionService::getNullObject());


		ctkPluginContext *pluginContext = cx::logicManager()->getPluginContext();
		cx::UsReconstructionServicePtr reconstructer = cx::UsReconstructionServicePtr(new cx::UsReconstructionServiceProxy(pluginContext)); //Can't mock


//		reconstructer->setOutputBasePath(cx::DataLocations::getTestDataPath() + "/temp/");
//		reconstructer->setOutputRelativePath("Images");

		mManager = reconstructer;
	}
	return mManager;
}

} // namespace cxtest


