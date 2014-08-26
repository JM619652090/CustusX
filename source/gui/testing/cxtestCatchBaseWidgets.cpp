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
#include "catch.hpp"

#include "cxLogicManager.h"

//cxBaseWidget children in alphabetical order
#include "cxActiveToolWidget.h"
#include "cxClippingWidget.h"
#include "cxColorWidget.h"
#include "cxCroppingWidget.h"
//#include "cxDataSelectWidget.h" //special case
#include "cxEraserWidget.h"
#include "cxFrameTreeWidget.h"
#include "cxMetricWidget.h"
#include "cxNavigationWidget.h"
#include "cxOverlayWidget.h"
#include "cxPlaybackWidget.h"
#include "cxPointSamplingWidget.h"
#include "cxProbeConfigWidget.h"
#include "cxSamplerWidget.h"
#include "cxShadingWidget.h"
//#include "cxSimulateUSWidget.h"
#include "cxToolPropertiesWidget.h"
#include "cxTrackPadWidget.h"
#include "cxTransferFunction2DColorWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransform3DWidget.h"
#include "cxVideoConnectionWidget.h"
//cxFileWatcherWidgets
#include "cxFilePreviewWidget.h"
#include "cxImagePreviewWidget.h"
//cxInfoWidgets
#include "cxMeshInfoWidget.h"
#include "cxVolumeInfoWidget.h"
//cxPresetWidgets
#include "cxTransferFunctionPresetWidget.h"
//cxTabbedWidgets
#include "cxSlicePropertiesWidget.h"
#include "cxVolumePropertiesWidget.h"

//All these widgets in the plugins folder should also be tested
//#include "cxAllFiltersWidget.h"
//#include "cxCompactFilterSetupWidget.h"
//#include "cxConnectedThresholdImageFilterWidget.h"
//#include "cxFilterWidget.h" //in plugings
//#include "cxLapFrameToolCalibrationWidget.h"
//#include "cxPipelineWidget.h"
//#include "cxPipelineWidgetFilterLine.h"
//#include "cxReconstructionWidget.h"
//#include "cxRecordSessionWidget.h"
//#include "cxRegistrationHistoryWidget.h"
//#include "cxSoundSpeedConverterWidget.h"
//#include "cxTemporalCalibrationWidget.h"
//#include "cxToolManualCalibrationWidget.h"
//#include "cxToolTipCalibrateWidget.h"
//#include "cxToolTipSampleWidget.h"
//#include "cxTrackPadWidget.h"
////cxPresetWidgets
//#include "FilterPresetWidget.h"
////cxRecordBaseWidgets
//#include "cxTrackedCenterlineWidget.h"
//#include "cxUSAcqusitionWidget.h"
////RegistrationBaseWidgets
//#include "cxElastixWidget.h"
//#include "cxFastOrientationRegistrationWidget.h"
//#include "cxLandmarkImage2ImageRegistrationWidget.h"
//#include "cxPlateImageRegistrationWidget.h"
//#include "cxFastPatientRegistrationWidget.h"
//#include "cxManualImageRegistrationWidget.h"
//#include "cxManualPatientRegistrationWidget.h"
//#include "cxPatientOrientationWidget.h"
//#include "cxPlateRegistrationWidget.h"
//#include "cxPrepareVesselsWidget.h"
//#include "cxRegisterI2IWidget.h"
//#include "cxSeansVesselRegistrationWidget.h"
//#include "cxWirePhantomWidget.h"
////cxTabbedWidgets
//#include "cxCalibrationMethodsWidget.h"
//#include "cxRegistrationMethodsWidget.h"

namespace cxtest
{

namespace
{
void testAndDeleteBaseWidgetChild(cx::BaseWidget* widget)
{
	REQUIRE(widget);
	CHECK_THAT(widget->objectName().toStdString(), EndsWith("Widget"));
	CHECK_FALSE(widget->windowTitle().isEmpty());
	CHECK_THAT(widget->defaultWhatsThis().toStdString(), StartsWith("<html>"));

	delete widget;
}

void init()
{
	cx::LogicManager::initialize();
//	cx::viewManager()->initialize();
}
void shutdown()
{
//	cx::viewManager()->destroyInstance();
	cx::LogicManager::shutdown();
}
}

TEST_CASE("BaseWidget's children in gui/dataWidgets correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	QWidget* testParent = new QWidget();

	testAndDeleteBaseWidgetChild(new cx::ActiveToolWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ActiveVolumeWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ClippingWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ColorWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::CroppingWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::DataSelectWidget(testParent));//special case: Needs a SelectDataStringDataAdapterBasePtr moc object
	testAndDeleteBaseWidgetChild(new cx::EraserWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::FrameTreeWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::MetricWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::NavigationWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::OverlayWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::PlaybackWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::PointSamplingWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ProbeConfigWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::SamplerWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ShadingWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::SimulateUSWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ToolPropertiesWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::TrackPadWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::Transform3DWidget(testParent));


	//cxTabbedWidgets in plugins
//	testAndDeleteBaseWidgetChild(new cx::CalibrationMethodsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::FastRegistrationsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::Image2ImageRegistrationWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::Image2PlateRegistrationWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::LandmarkRegistrationsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::ManualRegistrationsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::RegistrationMethodsWidget(testParent));

	delete testParent;
	shutdown();
}

TEST_CASE("VideoConnectionWidget is correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	testAndDeleteBaseWidgetChild(new cx::VideoConnectionWidget(NULL));
	shutdown();
}

TEST_CASE("FileWatcherWidgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	//cxFileWatcherWidgets
	testAndDeleteBaseWidgetChild(new cx::FilePreviewWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::ImagePreviewWidget(NULL));
	shutdown();
}

TEST_CASE("TabbedWidgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	//cxTabbedWidgets
	testAndDeleteBaseWidgetChild(new cx::SlicePropertiesWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::VolumePropertiesWidget(NULL));
	shutdown();
}

TEST_CASE("InfoWidgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	//cxInfoWidgets
	testAndDeleteBaseWidgetChild(new cx::VolumeInfoWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::MeshInfoWidget(NULL));
	shutdown();
}

TEST_CASE("TransferFunction widgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	testAndDeleteBaseWidgetChild(new cx::TransferFunction2DColorWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunction2DOpacityWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionAlphaWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionColorWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunction2DWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunction3DWidget(NULL));
	shutdown();
}

} //namespace cxtest
