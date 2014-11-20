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

#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <QtWidgets>


#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"

#include "cxImage.h"
#include "cxAxesRep.h"
#include "cxVolumetricRep.h"
#include "cxSliceComputer.h"
#include "cxVector3D.h"
#include "cxTransform3D.h"
#include "cxToolRep3D.h"
#include "cxDummyToolManager.h"
#include "cxDummyTool.h"
#include "cxSliceProxy.h"
#include "cxSlicerRepSW.h"
#include "cxLogger.h"


#include "cxViewsFixture.h"
#include "catch.hpp"
#include "cxtestRenderTester.h"
#include "cxReporter.h"
#include "cxImageLUT2D.h"

#include "QVTKWidget.h"
#include "vtkRendererCollection.h"
#include "cxtestUtilities.h"


using cx::Vector3D;
using cx::Transform3D;

class ImageTestList
{
public:
	ImageTestList()
	{
		image.push_back("Person5/person5_t1_unsigned.mhd");
		image.push_back("Person5/person5_t2_unsigned.mhd");
		image.push_back("Person5/person5_flair_unsigned.mhd");
	}
	QStringList image;
};

TEST_CASE("Visual rendering: Init view",
		  "[unit][resource][visualization]")
{
	cxtest::ViewsFixture fixture;
	REQUIRE(true);
}

TEST_CASE("Visual rendering: Overlapping gridlayout",
		  "[unit][resource][visualization][hide]")
{
	QWidget widget;
	QGridLayout* layout = new QGridLayout(&widget);
	widget.setLayout(layout);
	layout->setSpacing(0);
	layout->setMargin(0);

	QTextEdit* edit = new QTextEdit;
	edit->setText("Visual rendering: Overlapping gridlayout");
	layout->addWidget(edit, 0,0, 2,2);

	QLabel* label = new QLabel("Hello Mixed!");
	layout->addWidget(label, 2,0, 1,1);

	QLabel* label2 = new QLabel("Hello Quixed!");
	layout->addWidget(label2, 1,1, 1,1);

	for (unsigned i=0; i<3; ++i)
	{
		layout->setColumnStretch(i, 1);
		layout->setRowStretch(i, 1);
	}

	widget.show();
//	qApp->exec();
}

TEST_CASE("Visual rendering: Empty QVTKWidget",
		  "[unit][resource][visualization][hide]")
{
	QWidget mainWidget;
	QGridLayout* layout = new QGridLayout;
	layout->setSpacing(2);
	layout->setMargin(10);
	mainWidget.setLayout(layout);
	mainWidget.setGeometry(150,150,400,400);

	QVTKWidget* vtkWidget = new QVTKWidget();
	//	viewWidget clear
//  set custom renderwindow
//	viewWidget clear

	vtkWidget->GetRenderWindow()->GetInteractor()->EnableRenderOff();
	vtkWidget->GetRenderWindow()->GetInteractor()->Disable();
//	std::cout << "renderer count = " << vtkWidget->GetRenderWindow()->GetRenderers()->GetNumberOfItems() << std::endl;
//	vtkWidget->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->Render();
	vtkRendererPtr renderer = vtkRendererPtr::New();
	renderer->SetBackground(0,0,0);
	vtkWidget->GetRenderWindow()->AddRenderer(renderer);
	// dette bomber visualizeringa
//	vtkWidget->GetRenderWindow()->Render();

//	setstretchfactors
	layout->addWidget(vtkWidget, 0,0, 1,1);
//	main show
//	prettyzoom
//	render

	mainWidget.show();
	// did not work
//	QSize size = vtkWidget->size();
//	double factor = 2;
//	vtkWidget->GetRenderWindow()->SetSize(size.width()*factor, size.height()*factor);
	vtkWidget->GetRenderWindow()->Render();

//	sleep(3);

	CHECK(true);
    //sleep(3);
    cxtest::Utilities::sleep_sec(3);
}

TEST_CASE("Visual rendering: Empty view",
		  "[unit][resource][visualization]")
{
	cxtest::ViewsFixture fixture;

	cx::ViewPtr view = fixture.addView(0, 0);
//fixture.runWidget();
		REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("emptyview", 0);
	REQUIRE(fixture.getFractionOfBrightPixelsInView(0,0) == Approx(0));

//	sleep(3);

}

TEST_CASE("Visual rendering: Several empty views in a sequence.",
		  "[unit][resource][visualization]")
{
	int numberOfIterations = 2;
	for (unsigned i=0; i<numberOfIterations; ++i)
	{
		cxtest::ViewsFixture fixture;

		cx::ViewPtr view = fixture.addView(0, 0);
		REQUIRE(fixture.quickRunWidget());

		fixture.dumpDebugViewToDisk("emptyview", 0);
		REQUIRE(fixture.getFractionOfBrightPixelsInView(0,0) == Approx(0));
	}
}

TEST_CASE("Visual rendering: Show 3D volume - vtkGPU render",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 0);

	REQUIRE(fixture.quickRunWidget());
	fixture.dumpDebugViewToDisk("3DvtkGPU", 0);
	REQUIRE(fixture.getFractionOfBrightPixelsInView(0,0) > 0.01);
}

TEST_CASE("Visual rendering: Show ACS+3D, centered hidden tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 1, 1);
	fixture.defineSlice("A", imagenames.image[0], cx::ptAXIAL, 0, 0);
	fixture.defineSlice("C", imagenames.image[0], cx::ptCORONAL, 1, 0);
	fixture.defineSlice("S", imagenames.image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(fixture.quickRunWidget());

//	REQUIRE(fixture.runWidget());

	fixture.dumpDebugViewToDisk("acs3d0", 0);
	fixture.dumpDebugViewToDisk("acs3d1", 1);
	fixture.dumpDebugViewToDisk("acs3d2", 2);
	fixture.dumpDebugViewToDisk("acs3d3", 3);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(2,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(3,20) > 0.02);
}

TEST_CASE("Visual rendering: Show layout, clear, show new layout",
		  "[unit][resource][visualization][not_win32][not_win64][hide]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

    fixture.define3D(imagenames.image[0], NULL, 1, 1);
	fixture.defineSlice("A", imagenames.image[0], cx::ptAXIAL, 0, 0);
	fixture.defineSlice("C", imagenames.image[0], cx::ptCORONAL, 1, 0);
	fixture.defineSlice("S", imagenames.image[0], cx::ptSAGITTAL, 0, 1);
	REQUIRE(fixture.quickRunWidget());
    cxtest::Utilities::sleep_sec(1);


    std::cout << "===== CLEAR START" << std::endl;

	fixture.clear();
    std::cout << "===== CLEAR STOP" << std::endl;
    REQUIRE(fixture.quickRunWidget());
    cxtest::Utilities::sleep_sec(1);
//    return;

    std::cout << "===== DEFINE 2 START" << std::endl;
    fixture.define3D(imagenames.image[0], NULL, 0, 2);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 0);
    fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 1);

    std::cout << "===== DEFINE 2 STOP" << std::endl;
    REQUIRE(fixture.quickRunWidget());
//    REQUIRE(fixture.quickRunWidget());
    REQUIRE(fixture.runWidget());
    cxtest::Utilities::sleep_sec(1);

	CHECK(fixture.getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(2,20) > 0.02);
}

TEST_CASE("Visual rendering: Show AnyDual+3D, centered hidden tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 2);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 0);
	fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 1);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("anydual3d0", 0);
	fixture.dumpDebugViewToDisk("anydual3d1", 1);
	fixture.dumpDebugViewToDisk("anydual3d2", 2);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(2,20) > 0.02);
}

TEST_CASE("Visual rendering: Show 3D+AnyDual, centered hidden tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	fixture.define3D(imagenames.image[0], NULL, 0, 0);
	fixture.defineSlice("Any", imagenames.image[0], cx::ptANYPLANE, 0, 1);
	fixture.defineSlice("Dua", imagenames.image[0], cx::ptSIDEPLANE, 0, 2);
	REQUIRE(fixture.quickRunWidget());

	fixture.dumpDebugViewToDisk("anydual3d0", 0);
	fixture.dumpDebugViewToDisk("anydual3d1", 1);
	fixture.dumpDebugViewToDisk("anydual3d2", 2);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,0) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(1,20) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(2,20) > 0.02);
}

TEST_CASE("Visual rendering: Show ACS, 3 volumes, centered hidden tool",
		  "[ca_test][unit][resource][visualization]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	for (unsigned i = 0; i < 3; ++i)
	{
		fixture.defineSlice("A", imagenames.image[i], cx::ptAXIAL, 0, i);
		fixture.defineSlice("C", imagenames.image[i], cx::ptCORONAL, 1, i);
		fixture.defineSlice("S", imagenames.image[i], cx::ptSAGITTAL, 2, i);
	}
	REQUIRE(fixture.quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(fixture.getFractionOfBrightPixelsInView(i,20) > 0.02);
	}
}

TEST_CASE("Visual rendering: Show Axial GPU slice, 1 volume",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	std::vector<cx::ImagePtr> images(1);
	images[0] = fixture.loadImage(imagenames.image[0]);

	cx::ImageLUT2DPtr lut0 = images[0]->getLookupTable2D();
	lut0->addColorPoint(images[0]->getMax(), QColor::fromRgbF(0,0,1,1));

	REQUIRE(fixture.defineGPUSlice("A", images, cx::ptAXIAL, 0, 0));
	REQUIRE(fixture.quickRunWidget());

	CHECK(fixture.getFractionOfBrightPixelsInView(0,20,2) > 0.02);
}

TEST_CASE("Visual rendering: Show Axial GPU slice, 2 volumes",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	std::vector<cx::ImagePtr> images(2);
	images[0] = fixture.loadImage(imagenames.image[0]);
	images[1] = fixture.loadImage(imagenames.image[1]);

	cx::ImageLUT2DPtr lut0 = images[0]->getLookupTable2D();
	//std::cout << "setting llr for vol1 = " << llr << std::endl;
	lut0->addColorPoint(images[0]->getMax(), QColor::fromRgbF(0,1,0,1));

	cx::ImageLUT2DPtr lut1 = images[1]->getLookupTable2D();
	//std::cout << "setting llr for vol1 = " << llr << std::endl;
	lut1->addColorPoint(images[1]->getMax(), QColor::fromRgbF(0,0,1,1));
	double llr = images[1]->getMin() + (images[1]->getMax()-images[1]->getMin())*0.25;
	lut1->setLLR(llr);

	REQUIRE(fixture.defineGPUSlice("A", images, cx::ptAXIAL, 0, 0));
	REQUIRE(fixture.quickRunWidget());

	CHECK(fixture.getFractionOfBrightPixelsInView(0,20,1) > 0.02);
	CHECK(fixture.getFractionOfBrightPixelsInView(0,20,2) > 0.02);
}

TEST_CASE("Visual rendering: Show ACS, 3 GPU volumes, moving tool",
		  "[unit][resource][visualization][not_win32][not_win64]")
{
	cxtest::ViewsFixture fixture;
	ImageTestList imagenames;

	for (unsigned i = 0; i < 3; ++i)
	{
		REQUIRE(fixture.defineGPUSlice("A", imagenames.image[i], cx::ptAXIAL, 0, i));
		REQUIRE(fixture.defineGPUSlice("C", imagenames.image[i], cx::ptCORONAL, 1, i));
		REQUIRE(fixture.defineGPUSlice("S", imagenames.image[i], cx::ptSAGITTAL, 2, i));
    }
	REQUIRE(fixture.quickRunWidget());

	for (unsigned i = 0; i < 3*3; ++i)
	{
		CHECK(fixture.getFractionOfBrightPixelsInView(i,20) > 0.02);
	}
}
