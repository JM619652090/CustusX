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

#include <vtkGPUVolumeRayCastMapper.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkVolume.h>

#include "catch.hpp"

#include "sscVolumeHelpers.h"
#include "sscVolumetricRep.h"
#include "cxtestUtilities.h"
#include "sscMessageManager.h"
#include "cxtestDirectSignalListener.h"
#include "sscImage2DRep3D.h"
#include "cxConfig.h"

#include "cxMultiVolume3DRepProducer.h"
#include "cxtestMultiVolume3DRepProducerFixture.h"

#include "cxMehdiGPURayCastMultiVolumeRep.h"
//#include "vtkOpenGLGPUMultiVolumeRayCastMapper.h"

namespace cxtest
{

TEST_CASE("MultiVolume3DRepProducer emits imageChanged and repChanged signals for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	cxtest::DirectSignalListener imagesChanged(&fixture.mBase, SIGNAL(imagesChanged()));

	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", 1);

	CHECK(imagesChanged.isReceived());
}

TEST_CASE("MultiVolume3DRepProducer creates 1 rep using vtkVolumeTextureMapper3D for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", 1);

	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(0);
	REQUIRE(rep);
	CHECK(dynamic_cast<vtkVolumeTextureMapper3D*>(rep->getVtkVolume()->GetMapper()));
}

TEST_CASE("MultiVolume3DRepProducer creates 0 reps using invalid type for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;
	fixture.initializeVisualizerAndImages("invalid type", 1);

	REQUIRE(fixture.mBase.getAllReps().size() == 0);
}

TEST_CASE("MultiVolume3DRepProducer creates 1 rep using vtkGPUVolumeRayCastMapper for 1 added image.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;
	fixture.initializeVisualizerAndImages("vtkGPUVolumeRayCastMapper", 1);

	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(0);
	REQUIRE(rep);
	CHECK(dynamic_cast<vtkGPUVolumeRayCastMapper*>(rep->getVtkVolume()->GetMapper()));
}

TEST_CASE("MultiVolume3DRepProducer creates 1 rep using Image2DRep3D for 1 added 2D image, for any visualizer type.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	fixture.mBase.setVisualizerType("vtkVolumeTextureMapper3D");
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(5,5,1));
	fixture.mBase.addImage(image);
	fixture.mImages.push_back(image);


	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::Image2DRep3DPtr rep = fixture.downcastRep<cx::Image2DRep3D>(0);
	REQUIRE(rep);
}

TEST_CASE("MultiVolume3DRepProducer creates 3 reps using vtkVolumeTextureMapper3D for 3 added images.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", imageCount);

	REQUIRE(fixture.mBase.getAllReps().size() == imageCount);

	for (unsigned i=0; i<imageCount; ++i)
	{
		cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(i);
		REQUIRE(rep);
		CHECK(dynamic_cast<vtkVolumeTextureMapper3D*>(rep->getVtkVolume()->GetMapper()));
	}
}

TEST_CASE("MultiVolume3DRepProducer creates 2 reps using vtkVolumeTextureMapper3D for 3 added and 1 removed image(s).", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkVolumeTextureMapper3D", imageCount);

	fixture.mBase.removeImage(fixture.mImages[0]->getUid());
	imageCount--;

	REQUIRE(fixture.mBase.getAllReps().size() == imageCount);

	for (unsigned i=0; i<imageCount; ++i)
	{
		cx::VolumetricRepPtr rep = fixture.downcastRep<cx::VolumetricRep>(i);
		REQUIRE(rep);
		CHECK(dynamic_cast<vtkVolumeTextureMapper3D*>(rep->getVtkVolume()->GetMapper()));
	}
}

#ifdef CX_BUILD_MEHDI_VTKMULTIVOLUME
TEST_CASE("MultiVolume3DRepProducer creates 1 rep using vtkOpenGLGPUMultiVolumeRayCastMapper for 3 added images.", "[unit]")
{
	MultiVolume3DRepProducerFixture fixture;

	unsigned imageCount = 3;
	fixture.initializeVisualizerAndImages("vtkOpenGLGPUMultiVolumeRayCastMapper", imageCount);

	REQUIRE(fixture.mBase.getAllReps().size() == 1);

	cx::MehdiGPURayCastMultiVolumeRepPtr rep = fixture.downcastRep<cx::MehdiGPURayCastMultiVolumeRep>(0);
	REQUIRE(rep);
//		CHECK(dynamic_cast<vtkOpenGLGPUMultiVolumeRayCastMapper*>(rep->getVtkVolume()->GetMapper()));
}
#endif //CX_BUILD_MEHDI_VTKMULTIVOLUME

} // namespace cxtest
