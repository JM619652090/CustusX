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

#include "cxBinaryThresholdImageFilter.h"

#include "cxAlgorithmHelpers.h"
#include <itkBinaryThresholdImageFilter.h>
#include <vtkImageCast.h>
#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxColorProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxDoublePairProperty.h"
#include "cxContourFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxSelectDataStringProperty.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"

namespace cx
{

BinaryThresholdImageFilter::BinaryThresholdImageFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString BinaryThresholdImageFilter::getName() const
{
	return "Segmentation";
}

QString BinaryThresholdImageFilter::getType() const
{
	return "BinaryThresholdImageFilter";
}

QString BinaryThresholdImageFilter::getHelp() const
{
	return "<html>"
	        "<h3>Binary Threshold Image Filter.</h3>"
	        "<p><i>Segment out areas from the selected image using a threshold.</i></p>"
	        "<p>This filter produces an output image whose pixels are either one of two values"
	        "( OutsideValue or InsideValue ), depending on whether the corresponding input"
	        "image pixels lie between the two thresholds ( LowerThreshold and UpperThreshold )."
	        "Values equal to either threshold is considered to be between the thresholds.<p>"
	        "</html>";
}

DoublePairPropertyPtr BinaryThresholdImageFilter::getThresholdOption(QDomElement root)
{
	DoublePairPropertyPtr retval = DoublePairProperty::initialize("Thresholds", "",
																			  "Select lower and upper threshold for the segmentation", DoubleRange(0, 100, 1), 0,
																			  root);
	return retval;
}

BoolPropertyPtr BinaryThresholdImageFilter::getGenerateSurfaceOption(QDomElement root)
{
	BoolPropertyPtr retval = BoolProperty::initialize("Generate Surface", "",
																  "Generate a surface of the output volume", true,
																  root);
	return retval;
}

ColorPropertyPtr BinaryThresholdImageFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

void BinaryThresholdImageFilter::createOptions()
{
	mThresholdOption = this->getThresholdOption(mOptions);
	connect(mThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
	mOptionsAdapters.push_back(mThresholdOption);
	mOptionsAdapters.push_back(this->getGenerateSurfaceOption(mOptions));
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void BinaryThresholdImageFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->getPatientService());
	temp->setValueName("Input");
	temp->setHelp("Select image input for thresholding");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void BinaryThresholdImageFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectData::New(mServices->getPatientService());
	temp->setValueName("Output");
	temp->setHelp("Output thresholded binary image");
	mOutputTypes.push_back(temp);

	temp = StringPropertySelectData::New(mServices->getPatientService());
	temp->setValueName("Contour");
	temp->setHelp("Output contour generated from thresholded binary image.");
	mOutputTypes.push_back(temp);
}

void BinaryThresholdImageFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	if (!mActive)
		mServices->visualizationService->removePreview();
}

void BinaryThresholdImageFilter::imageChangedSlot(QString uid)
{
	this->updateThresholdPairFromImageChange(uid, mThresholdOption);
	mServices->visualizationService->removePreview();
}

void BinaryThresholdImageFilter::thresholdSlot()
{
	if (mActive)
	{
		ImagePtr image = boost::dynamic_pointer_cast<Image>(mInputTypes[0]->getData());
		std::vector<double> threshold;
		threshold.push_back(mThresholdOption->getValue()[0]);
		threshold.push_back(mThresholdOption->getValue()[1]);
		mServices->visualizationService->setPreview(image, threshold);
	}
}

bool BinaryThresholdImageFilter::preProcess()
{
	mServices->visualizationService->removePreview();
	return FilterImpl::preProcess();

}

bool BinaryThresholdImageFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	DoublePairPropertyPtr thresholds = this->getThresholdOption(mCopiedOptions);
	BoolPropertyPtr generateSurface = this->getGenerateSurfaceOption(mCopiedOptions);

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	//Binary Thresholding
	typedef itk::BinaryThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
	thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
	thresholdFilter->SetInput(itkImage);
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->SetInsideValue(1);
	thresholdFilter->SetLowerThreshold(thresholds->getValue()[0]);
	thresholdFilter->SetUpperThreshold(thresholds->getValue()[1]);
	thresholdFilter->Update();
	itkImage = thresholdFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());

	vtkImageCastPtr imageCast = vtkImageCastPtr::New();
	imageCast->SetInputData(rawResult);
	imageCast->SetOutputScalarTypeToUnsignedChar();
	imageCast->Update();
	rawResult = imageCast->GetOutput();

	// TODO: possible memory problem here - check debug mem system of itk/vtk

	mRawResult =  rawResult;

	if (generateSurface->getValue())
	{
		double threshold = 1;/// because the segmented image is 0..1
		mRawContour = ContourFilter::execute(mRawResult, threshold);
	}

	return true;
}

bool BinaryThresholdImageFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	QString uid = input->getUid() + "_seg%1";
	QString name = input->getName()+" seg%1";
	ImagePtr output = createDerivedImage(mServices->getPatientService(),
										 uid, name,
										 mRawResult, input);

	mRawResult = NULL;

	output->resetTransferFunctions();
	mServices->getPatientService()->insertData(output);

	// set output
	mOutputTypes.front()->setValue(output->getUid());

	// set contour output
	if (mRawContour!=NULL)
	{
		ColorPropertyPtr colorOption = this->getColorOption(mOptions);
		MeshPtr contour = ContourFilter::postProcess(mServices->getPatientService(), mRawContour, output, colorOption->getValue());
		mOutputTypes[1]->setValue(contour->getUid());
		mRawContour = vtkPolyDataPtr();
	}

	return true;
}


}//namespace cx