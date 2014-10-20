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

/*
 * cxShadingWidget.cpp
 *
 *  \date Aug 20, 2010
 *      \author christiana
 */
#include "cxShadingWidget.h"

#include <QVBoxLayout>
#include <QComboBox>
#include <QStringList>
#include "cxImageTF3D.h"
#include "cxShadingParamsInterfaces.h"
#include "cxImagePropertiesWidget.h"
#include "cxPatientModelService.h"
#include "cxLogicManager.h"

namespace cx
{

ShadingWidget::ShadingWidget(PatientModelServicePtr patientModelService, QWidget* parent) :
	BaseWidget(parent, "ShadingWidget", "Shading"),
	mLayout(new QVBoxLayout(this)),
	mPatientModelService(patientModelService)
{
  this->init();
}

ShadingWidget::~ShadingWidget()
{}

void ShadingWidget::init()
{
  mShadingCheckBox = new QCheckBox("Shading", this);
//  mLayout->addWidget(mShadingCheckBox);
//  mShadingCheckBox->setEnabled(true);
//

  connect(mShadingCheckBox, SIGNAL(toggled(bool)), this, SLOT(shadingToggledSlot(bool)));

  QGridLayout* shadingLayput = new QGridLayout();
	shadingLayput->addWidget(mShadingCheckBox, 0,0);
  SliderGroupWidget* shadingAmbientWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingAmbient(mPatientModelService)), shadingLayput, 1);
  SliderGroupWidget* shadingDiffuseWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingDiffuse(mPatientModelService)), shadingLayput, 2);
  SliderGroupWidget* shadingSpecularWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecular(mPatientModelService)), shadingLayput, 3);
  SliderGroupWidget* shadingSpecularPowerWidget = new SliderGroupWidget(this, DoubleDataAdapterPtr(new DoubleDataAdapterShadingSpecularPower(mPatientModelService)), shadingLayput, 4);

  shadingAmbientWidget->setEnabled(false);
  shadingDiffuseWidget->setEnabled(false);
  shadingSpecularWidget->setEnabled(false);
  shadingSpecularPowerWidget->setEnabled(false);

  mActiveImageProxy = ActiveImageProxy::New(mPatientModelService);
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChangedSlot()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SLOT(activeImageChangedSlot()));

	ImagePropertiesWidget* imagePropertiesWidget = new ImagePropertiesWidget(mPatientModelService, NULL);
	shadingLayput->addWidget(imagePropertiesWidget, 5, 0, 1, 2);
  mLayout->addLayout(shadingLayput);
	mLayout->addStretch(1);

//  this->setLayout(mLayout);
}

void ShadingWidget::shadingToggledSlot(bool val)
{
  ImagePtr image = mPatientModelService->getActiveImage();
  if (image)
  {
    image->setShadingOn(val);
  }
}

void ShadingWidget::activeImageChangedSlot()
{
  ImagePtr activeImage = mPatientModelService->getActiveImage();

  if (activeImage)
  {
    //std::cout << "shading updated to " << activeImage->getShadingOn() << std::endl;
    mShadingCheckBox->setChecked(activeImage->getShadingOn());
  }
}

QString ShadingWidget::defaultWhatsThis() const
{
	return "<html></html>";
}

}//namespace cx
