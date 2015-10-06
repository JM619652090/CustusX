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
#include "cxScreenVideoProvider.h"

#include <QPixmap>
#include "cxPatientModelService.h"
#include <QtConcurrent>
#include <QDesktopWidget>
#include <QApplication>
#include "cxReporter.h"
#include "boost/bind.hpp"
#include <QScreen>
#include <QVBoxLayout>
#include "cxViewService.h"
//#include "cxSecondaryViewLayoutWindow.h"
#include "cxViewCollectionWidget.h"
#include "vtkRenderer.h"
#include "vtkWindowToImageFilter.h"
#include "vtkRenderWindow.h"
#include "vtkPNGWriter.h"
#include "vtkUnsignedCharArray.h"

namespace cx
{

SecondaryViewLayoutWindow::SecondaryViewLayoutWindow(QWidget* parent, VisualizationServicePtr viewService) :
	QWidget(parent),
	mViewService(viewService)
{
	this->setLayout(new QVBoxLayout(this));
	this->layout()->setMargin(0);
	this->setWindowTitle("View Layout");
}

void SecondaryViewLayoutWindow::showEvent(QShowEvent* event)
{
	QWidget* widget = mViewService->getLayoutWidget(this, 1);
	this->layout()->addWidget(widget);
	if (mViewService->getActiveLayout(1).isEmpty())
		mViewService->setActiveLayout("LAYOUT_OBLIQUE_3DAnyDual_x1", 1);
}

void SecondaryViewLayoutWindow::hideEvent(QCloseEvent* event)
{
	mViewService->setActiveLayout("", 1);
}

void SecondaryViewLayoutWindow::closeEvent(QCloseEvent *event)
{
	mViewService->setActiveLayout("", 1);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ScreenVideoProvider::ScreenVideoProvider(VisServicesPtr services) :
	mServices(services)
{

}


void ScreenVideoProvider::saveScreenShot(QImage image, QString id)
{
	QString ending = "png";
	if (!id.isEmpty())
		ending = id + "." + ending;
	QString path = mServices->patientModelService->generateFilePath("Screenshots", ending);
	QtConcurrent::run(boost::bind(&ScreenVideoProvider::saveScreenShotThreaded, this, image, path));
}

QByteArray ScreenVideoProvider::generatePNGEncoding(QImage image)
{
	QByteArray ba;
	QBuffer buffer(&ba);
	buffer.open(QIODevice::WriteOnly);
	image.save(&buffer, "PNG"); // writes image into ba in PNG format//	QString ending = "png";
	return ba;
}

/**Intended to be called in a separate thread.
 * \sa saveScreenShot()
 */
void ScreenVideoProvider::saveScreenShotThreaded(QImage pixmap, QString filename)
{
	pixmap.save(filename, "png");
	report("Saved screenshot to " + filename);
	reporter()->playScreenShotSound();
}

QPixmap ScreenVideoProvider::grabScreen(unsigned screenid)
{
	QDesktopWidget* desktop = qApp->desktop();
	QList<QScreen*> screens = qApp->screens();

	QWidget* screenWidget = desktop->screen(screenid);
	WId screenWinId = screenWidget->winId();
	QRect geo = desktop->screenGeometry(screenid);
	QString name = "";
	if (desktop->screenCount()>1)
	{
		name = screens[screenid]->name().split(" ").join("");
		//On windows screens[i]->name() is "\\.\DISPLAY1",
		//Have to remove unvalid chars for the filename
		name.replace("\\", "");
		name.replace(".", "");
	}
	QPixmap pixmap = screens[screenid]->grabWindow(screenWinId, geo.left(), geo.top(), geo.width(), geo.height());
	return pixmap;
}

//void ScreenVideoProvider::shootScreen()
//{
//	QDesktopWidget* desktop = qApp->desktop();
//	QList<QScreen*> screens = qApp->screens();

//	for (int i=0; i<desktop->screenCount(); ++i)
//	{
//		QPixmap pixmap = this->grabScreen(i);
////		this->saveScreenShot(screens[i]->grabWindow(screenWinId, geo.left(), geo.top(), geo.width(), geo.height()), name);
//	}

//}


void ScreenVideoProvider::showSecondaryLayout()
{
	std::cout << "show window" << std::endl;
	if (!mSecondaryViewLayoutWindow)
		mSecondaryViewLayoutWindow = new SecondaryViewLayoutWindow(NULL, mServices->visualizationService);
	mSecondaryViewLayoutWindow->show();

	QRect rect = QRect(QPoint(50,50), QSize(320,568));
	mSecondaryViewLayoutWindow->setGeometry(rect);
	mSecondaryViewLayoutWindow->move(rect.topLeft());
}

vtkImageDataPtr ScreenVideoProvider::view2vtkImageData(ViewPtr view)
{
	Eigen::Array4d vp(view->getRenderer()->GetViewport());
	vtkWindowToImageFilterPtr w2i = vtkWindowToImageFilterPtr::New();
	w2i->SetInput(view->getRenderWindow());
	w2i->SetViewport(vp.data());
	w2i->SetReadFrontBuffer(false);
	w2i->Update();
	CX_LOG_CHANNEL_DEBUG("CA") << "Updated view " << vp;
	vtkImageDataPtr image = w2i->GetOutput();
	return image;
}

QImage ScreenVideoProvider::vtkImageData2QImage(vtkImageDataPtr input)
{
	unsigned char* ptr = reinterpret_cast<unsigned char*>(input->GetScalarPointer());
	Eigen::Array3i dim(input->GetDimensions());
//	int len = image->GetNumberOfScalarComponents() * image->GetScalarSize() * dim[0] * dim[1];

	QImage retval(ptr, dim[0], dim[1], QImage::Format_RGB888);
	return retval;
}

QImage ScreenVideoProvider::grabSecondaryLayout()
{
	QWidget* widget = mServices->visualizationService->getLayoutWidget(NULL, 1);
	ViewCollectionWidget* vcWidget = dynamic_cast<ViewCollectionWidget*>(widget);
	std::vector<ViewPtr> views = vcWidget->getViews();

	CX_LOG_CHANNEL_DEBUG("CA") << "views: " << views.size();
	for (unsigned i=0; i<views.size(); ++i)
	{
		vtkImageDataPtr vtkImage = this->view2vtkImageData(views[i]);
		QImage qImage = vtkImageData2QImage(vtkImage);
		return qImage;

//		char* ptr = reinterpret_cast<char*>(image->GetScalarPointer());
//		QImage img();
//		Eigen::Array3i dim(image->GetDimensions());
//		int len = image->GetNumberOfScalarComponents() * image->GetScalarSize() * dim[0] * dim[1];
//		pm.loadFromData(reinterpret_cast<char*>(image->GetScalarPointer()), len);

//		vtkPNGWriterPtr writer = vtkPNGWriterPtr::New();
//		writer->SetWriteToMemory(true);
//		writer->SetInputData(image);
//		writer->SetCompressionLevel(0);
//		writer->Write();
//		vtkUnsignedCharArrayPtr result = writer->GetResult();
	}

	return QImage();
}


} // namespace cx
