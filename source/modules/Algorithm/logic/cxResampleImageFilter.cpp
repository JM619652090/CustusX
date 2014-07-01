// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxResampleImageFilter.h"

#include <QApplication>

#include "cxImageAlgorithms.h"
#include "cxDataManager.h"
#include "cxImage.h"
#include "vtkImageData.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxDoubleDataAdapterXml.h"

namespace cx
{


QString ResampleImageFilter::getName() const
{
	return "Resample";
}

QString ResampleImageFilter::getType() const
{
	return "ResampleImageFilter";
}

QString ResampleImageFilter::getHelp() const
{
	return "<html>"
	        "<h3>Resample.</h3>"
	        "<p><i>Resample the volume into the space of the reference volume. Also crop to the same volume.</i></p>"
	        "</html>";
}

DoubleDataAdapterXmlPtr ResampleImageFilter::getMarginOption(QDomElement root)
{
	return DoubleDataAdapterXml::initialize("Margin", "",
	                                             "mm Margin added to ref image bounding box",
	                                             5.0, DoubleRange(0, 50, 1), 1, root);
}

void ResampleImageFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getMarginOption(mOptions));
}

void ResampleImageFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select input to be resampled");
	mInputTypes.push_back(temp);

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Reference");
	temp->setHelp("Select reference. Resample input into this coordinate system and bounding box");
	mInputTypes.push_back(temp);
}

void ResampleImageFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectDataStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Output thresholded binary image");
	mOutputTypes.push_back(temp);
}

//bool ResampleImageFilter::preProcess()
//{
//    return FilterImpl::preProcess();
//}

/** Crop the image to the bounding box bb_q.
 *  bb_q is given in the output space q, defined relative to the image space d
 *  with qMd. If qMd is non-identity, image is resampled to space q.
 *  outputSpacing can be used to resample the volume (after cropping).
 */
bool ResampleImageFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage(0);
	ImagePtr reference = this->getCopiedInputImage(1);
	if (!input || !reference)
		return false;

	DoubleDataAdapterXmlPtr marginOption = this->getMarginOption(mCopiedOptions);
	double margin = marginOption->getValue();

	Transform3D refMi = reference->get_rMd().inv() * input->get_rMd();
	ImagePtr oriented = resampleImage(dataService(), input, refMi);//There is an error with the transfer functions in this image

	Transform3D orient_M_ref = oriented->get_rMd().inv() * reference->get_rMd();
	DoubleBoundingBox3D bb_crop = transform(orient_M_ref, reference->boundingBox());

	// increase bb size by margin
	bb_crop[0] -= margin;
	bb_crop[1] += margin;
	bb_crop[2] -= margin;
	bb_crop[3] += margin;
	bb_crop[4] -= margin;
	bb_crop[5] += margin;

	oriented->setCroppingBox(bb_crop);

	ImagePtr cropped = cropImage(dataService(), oriented);

	QString uid = input->getUid() + "_resample%1";
	QString name = input->getName() + " resample%1";

	ImagePtr resampled = resampleImage(dataService(), cropped, Vector3D(reference->getBaseVtkImageData()->GetSpacing()), uid, name);

	// important! move thread affinity to main thread - ensures signals/slots is still called correctly
	resampled->moveThisAndChildrenToThread(QApplication::instance()->thread());

	mRawResult =  resampled;
	return true;
}

bool ResampleImageFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr output = mRawResult;
	mRawResult.reset();
	//    output->resetTransferFunctions();
	dataManager()->loadData(output);
	dataManager()->saveImage(output, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(output->getUid());
	return true;
}



} // namespace cx
