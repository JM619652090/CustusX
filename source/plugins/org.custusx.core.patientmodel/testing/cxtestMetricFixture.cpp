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

#include "cxtestMetricFixture.h"

#include "cxDummyToolManager.h"
#include "cxDataLocations.h"
#include "catch.hpp"
#include "cxTypeConversions.h"
#include "cxStringHelpers.h"
#include "cxSpaceProviderImpl.h"
#include "cxRegistrationTransform.h"

namespace cxtest {

MetricFixture::MetricFixture()
{
	mMessageListener = cx::MessageListener::createWithQueue();
	mServices = TestVisServices::create();
}

MetricFixture::~MetricFixture()
{
	mServices.reset();
	CHECK(!mMessageListener->containsErrors());
}

cx::SpaceProviderPtr MetricFixture::getSpaceProvider()
{
	return mServices->spaceProvider();
}

FrameMetricWithInput MetricFixture::getFrameMetricWithInput()
{
	FrameMetricWithInput retval;

	retval.m_qMt = cx::createTransformRotateZ(M_PI_2) * cx::createTransformTranslate(cx::Vector3D(1,2,3));
	retval.mSpace = cx::CoordinateSystem::reference();

	retval.mMetric = this->createTestMetric<cx::FrameMetric>("testMetric%1");
    retval.mMetric->setFrame(retval.m_qMt);
    retval.mMetric->setSpace(retval.mSpace);
	this->insertData(retval.mMetric);

    return retval;
}

ToolMetricWithInput MetricFixture::getToolMetricWithInput()
{
	ToolMetricWithInput retval;

	retval.m_qMt = cx::createTransformRotateZ(M_PI_2) * cx::createTransformTranslate(cx::Vector3D(1,2,3));
	retval.mSpace = cx::CoordinateSystem::reference();
	retval.mName = "TestTool";
	retval.mOffset = 5;

//	retval.mMetric = cx::ToolMetric::create("testMetric%1");
	retval.mMetric = this->createTestMetric<cx::ToolMetric>("testMetric%1");
	retval.mMetric->setFrame(retval.m_qMt);
	retval.mMetric->setSpace(retval.mSpace);
	retval.mMetric->setToolName(retval.mName);
	retval.mMetric->setToolOffset(retval.mOffset);
	this->insertData(retval.mMetric);

	return retval;
}

PointMetricWithInput MetricFixture::getPointMetricWithInput(cx::Vector3D point)
{
	PointMetricWithInput retval;

    retval.mPoint = point;
	retval.mSpace = cx::CoordinateSystem::reference();

//	retval.mMetric = cx::PointMetric::create("testMetric%1");
	retval.mMetric = this->createTestMetric<cx::PointMetric>("testMetric%1");
	retval.mMetric->setCoordinate(point);
    retval.mMetric->setSpace(retval.mSpace);
	this->insertData(retval.mMetric);

    return retval;
}

PlaneMetricWithInput MetricFixture::getPlaneMetricWithInput(cx::Vector3D point, cx::Vector3D normal, cx::DataMetricPtr p0, cx::DataMetricPtr p1)
{
	PlaneMetricWithInput retval;

	retval.mPoint = point;
	retval.mNormal = normal;

	retval.mMetric = this->createTestMetric<cx::PlaneMetric>("testMetric%1");
	retval.mMetric->getArguments()->set(0, p0);
	retval.mMetric->getArguments()->set(1, p1);
	this->insertData(retval.mMetric);

    return retval;
}

DistanceMetricWithInput MetricFixture::getDistanceMetricWithInput(double distance, cx::DataMetricPtr p0, cx::DataMetricPtr p1)
{
	DistanceMetricWithInput retval;

    retval.mDistance = distance;

	retval.mMetric = this->createTestMetric<cx::DistanceMetric>("testMetric%1");
	retval.mMetric->getArguments()->set(0, p0);
	retval.mMetric->getArguments()->set(1, p1);
	this->insertData(retval.mMetric);

    return retval;
}

DistanceMetricWithInput MetricFixture::getDistanceMetricWithInput(double distance)
{
	DistanceMetricWithInput retval;

    retval.mDistance = distance;

	retval.mMetric = this->createTestMetric<cx::DistanceMetric>("testMetric%1");
	retval.mMetric->getArguments()->set(0, this->getPointMetricWithInput(cx::Vector3D(0,0,0)).mMetric);
	retval.mMetric->getArguments()->set(1, this->getPointMetricWithInput(cx::Vector3D(distance,0,0)).mMetric);
	this->insertData(retval.mMetric);

    return retval;
}


bool MetricFixture::inputEqualsMetric(FrameMetricWithInput data)
{
    INFO("qMt");
    INFO(qstring_cast(data.m_qMt));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getFrame()));

	return cx::similar(data.m_qMt, data.mMetric->getFrame())
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::inputEqualsMetric(ToolMetricWithInput data)
{
	INFO("qMt");
	INFO(qstring_cast(data.m_qMt));
	INFO("==");
	INFO(qstring_cast(data.mMetric->getFrame()));

	return cx::similar(data.m_qMt, data.mMetric->getFrame())
			&& (data.mSpace == data.mMetric->getSpace())
			&& (cx::similar(data.mOffset, data.mMetric->getToolOffset()))
			&& (data.mName == data.mMetric->getToolName());
}

bool MetricFixture::inputEqualsMetric(DistanceMetricWithInput data)
{
    INFO("distance");
    INFO(qstring_cast(data.mDistance));
    INFO("==");
    INFO(qstring_cast(data.mMetric->getDistance()));

    INFO("p0:");
	if (data.mMetric->getArguments()->get(0))
		INFO(streamXml2String(*data.mMetric->getArguments()->get(0)));
    INFO("p1:");
	if (data.mMetric->getArguments()->get(1))
		INFO(streamXml2String(*data.mMetric->getArguments()->get(1)));

	CHECK(cx::similar(data.mDistance, data.mMetric->getDistance()));
	return (cx::similar(data.mDistance, data.mMetric->getDistance()));
}

bool MetricFixture::inputEqualsMetric(PointMetricWithInput data)
{
	return (cx::similar(data.mPoint, data.mMetric->getCoordinate()))
            && (data.mSpace == data.mMetric->getSpace());
}

bool MetricFixture::inputEqualsMetric(PlaneMetricWithInput data)
{
	return (cx::similar(data.mPoint, data.mMetric->getRefCoord()))
			&& (cx::similar(data.mNormal, data.mMetric->getRefNormal()));
}

QStringList MetricFixture::getSingleLineDataList(cx::DataMetricPtr metric)
{
	QString singleLine = metric->getAsSingleLineString();
	CHECK(!singleLine.isEmpty());
	INFO("line: " + singleLine);
	QStringList list = cx::splitStringContaingQuotes(singleLine);
	INFO("list: " + list.join("\n"));
	CHECK(!list.empty());
	return list;
}

QDomNode MetricFixture::createDummyXmlNode()
{
	QDomDocument document;
	document.appendChild(document.createElement("root"));
    return document.firstChild();
}

void MetricFixture::setPatientRegistration()
{
	cx::Transform3D testRegistration;
	testRegistration = cx::Transform3D(cx::createTransformTranslate(cx::Vector3D(5,6,7)));
	mServices->patient()->get_rMpr_History()->setRegistration(testRegistration);
}

bool MetricFixture::verifySingleLineHeader(QStringList list, cx::DataMetricPtr metric)
{
    if (list.size()<2)
        return false;
    if (list[0]!=metric->getType())
        return false;
    if (list[1]!=metric->getName())
        return false;
    return true;
}

} //namespace cxtest
