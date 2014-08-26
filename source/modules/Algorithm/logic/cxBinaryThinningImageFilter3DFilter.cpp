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

#include "cxBinaryThinningImageFilter3DFilter.h"

#include <itkBinaryThinningImageFilter3D.h>

#include "cxReporter.h"
#include "cxDataManager.h"
#include "cxRegistrationTransform.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxColorDataAdapterXml.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxAlgorithmHelpers.h"

namespace cx
{

QString BinaryThinningImageFilter3DFilter::getName() const
{
	return "Centerline";
}

QString BinaryThinningImageFilter3DFilter::getType() const
{
	return "BinaryThinningImageFilter3DFilter";
}

QString BinaryThinningImageFilter3DFilter::getHelp() const
{
	return "<html>"
	        "<h3>itk::BinaryThinningImageFilter3D</h3>"
	        "<p>"
	        "This filter computes one-pixel-wide skeleton of a 3D input image."
	        "</p><p>"
	        "This class is parametrized over the type of the input image "
	        "and the type of the output image."
	        "</p><p>"
	        "The input is assumed to be a binary image. All non-zero valued voxels "
	        "are set to 1 internally to simplify the computation. The filter will "
	        "produce a skeleton of the object.  The output background values are 0, "
	        "and the foreground values are 1."
	        "</p><p>"
	        "A 26-neighbourhood configuration is used for the foreground and a "
	        "6-neighbourhood configuration for the background. Thinning is performed "
	        "symmetrically in order to guarantee that the skeleton lies medial within "
	        "the object."
	        "</p><p>"
	        "This filter is a parallel thinning algorithm and is an implementation "
	        "of the algorithm described in:"
	        "</p><p>"
	        "T.C. Lee, R.L. Kashyap, and C.N. Chu.<br>"
	        "Building skeleton models via 3-D medial surface/axis thinning algorithms.<br>"
	        "Computer Vision, Graphics, and Image Processing, 56(6):462--478, 1994."
	        "</p></html>";
}

ColorDataAdapterPtr BinaryThinningImageFilter3DFilter::getColorOption(QDomElement root)
{
	return ColorDataAdapterXml::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

void BinaryThinningImageFilter3DFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void BinaryThinningImageFilter3DFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select binary volume input for thinning");
	//    connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void BinaryThinningImageFilter3DFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectMeshStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Output centerline model");
	mOutputTypes.push_back(temp);
}

bool BinaryThinningImageFilter3DFilter::preProcess()
{
	bool retval = FilterImpl::preProcess();
	if (!retval)
		return false;

	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	if (input->getMax() != 1 || input->getMin() != 0)
	{
		reportWarning(QString("Centerline: Input image %1 must be binary, aborting.").arg(input->getName()));
		return false;
	}

	return true;
}

bool BinaryThinningImageFilter3DFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	if (input->getMax() != 1 || input->getMin() != 0)
	{
		return false;
	}

	//      report(QString("Creating centerline from \"%1\"...").arg(input->getName()));

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	//Centerline extraction
	typedef itk::BinaryThinningImageFilter3D<itkImageType, itkImageType> centerlineFilterType;
	centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
	centerlineFilter->SetInput(itkImage);
	centerlineFilter->Update();
	itkImage = centerlineFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());

	mRawResult =  rawResult;
	return true;
}

bool BinaryThinningImageFilter3DFilter::postProcess()
{
	bool success = false;
	if(!mRawResult)
		return success;

	ColorDataAdapterPtr outputColor = this->getColorOption(mCopiedOptions);

	ImagePtr input = this->getCopiedInputImage();

	ImagePtr outImage = dataManager()->createDerivedImage(mRawResult,input->getUid() + "_cl_temp%1", input->getName()+" cl_temp%1", input);
	mRawResult = NULL;
	outImage->resetTransferFunctions();

	//automatically generate a mesh from the centerline
	vtkPolyDataPtr centerlinePolyData = SeansVesselReg::extractPolyData(outImage, 1, 0);

	QString uid = input->getUid() + "_cl%1";
	QString name = input->getName()+" cl%1";
	MeshPtr mesh = dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
	mesh->setColor(outputColor->getValue());
	mesh->get_rMd_History()->setParentSpace(input->getUid());
	dataManager()->loadData(mesh);
	dataManager()->saveMesh(mesh, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(mesh->getUid());
	success = true;

	return success;
}



} // namespace cx

