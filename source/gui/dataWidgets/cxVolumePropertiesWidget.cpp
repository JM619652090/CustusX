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

#include "cxVolumePropertiesWidget.h"

#include <QComboBox>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include "cxLabeledComboBoxWidget.h"
#include "cxLabeledLineEditWidget.h"
#include "cxImage.h"
#include "cxTransferFunctionWidget.h"
#include "cxCroppingWidget.h"
#include "cxClippingWidget.h"
#include "cxShadingWidget.h"
#include "cxDataInterface.h"
#include "cxDataSelectWidget.h"
#include "cxSelectDataStringProperty.h"
#include "cxVolumeInfoWidget.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"
#include "cxSelectClippersForDataWidget.h"

namespace cx
{

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

ActiveVolumeWidget::ActiveVolumeWidget(PatientModelServicePtr patientModelService, ViewServicePtr viewService, QWidget* parent) :
  BaseWidget(parent, "active_volume_widget", "Active Volume")
{
	this->setToolTip("The currently selected active volume");
  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
	layout->addWidget(new DataSelectWidget(viewService, patientModelService, this, StringPropertyActiveImage::New(patientModelService)));
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

VolumePropertiesWidget::VolumePropertiesWidget(VisServicesPtr services, QWidget *parent) :
		TabbedWidget(parent, "volume_properties_widget", "Volume Properties")
{
	this->setToolTip("Volume properties");
	this->insertWidgetAtTop(new ActiveVolumeWidget(services->patient(), services->view(), this));

	bool connectToActiveImage = true;

	this->addTab(new VolumeInfoWidget(services->patient(), this), "Info");
	this->addTab(new TransferFunctionWidget(services->patient(), this, connectToActiveImage), QString("Transfer Functions"));
	this->addTab(new ShadingWidget(services->patient()->getActiveData(), this, connectToActiveImage), "Properties");
	this->addTab(new CroppingWidget(services->patient(), services->view(), this), "Crop");
	this->addTab(new SelectClippersForImageWidget(services, this), "Clip");
}

}//namespace
