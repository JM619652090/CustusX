// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include <sscPlaneMetric.h>

#include "sscBoundingBox3D.h"
#include "sscTool.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"

namespace cx
{

DataPtr PlaneMetricReader::load(const QString& uid, const QString& filename)
{
	return DataPtr(new PlaneMetric(uid, filename));
}

PlaneMetric::PlaneMetric(const QString& uid, const QString& name) :
	DataMetric(uid, name),
	mSpace(SpaceHelpers::getR())
{
	mSpaceListener.reset(new CoordinateSystemListener(mSpace));
	connect(mSpaceListener.get(), SIGNAL(changed()), this, SIGNAL(transformChanged()));
}

PlaneMetricPtr PlaneMetric::create(QString uid, QString name)
{
    return PlaneMetricPtr(new PlaneMetric(uid, name));
}

PlaneMetricPtr PlaneMetric::create(QDomNode node)
{
    PlaneMetricPtr retval = PlaneMetric::create("");
    retval->parseXml(node);
    return retval;
}

PlaneMetric::~PlaneMetric()
{
}

Plane3D PlaneMetric::getRefPlane() const
{
	Transform3D rM1 = SpaceHelpers::get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	Vector3D p = rM1.coord(this->getCoordinate());
	Vector3D n = rM1.vector(this->getNormal()).normalized();

	return Eigen::Hyperplane<double, 3>(n, p);
}

Vector3D PlaneMetric::getRefCoord() const
{
    Transform3D rM0 = SpaceHelpers::get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
    Vector3D p0_r = rM0.coord(this->getCoordinate());
    return p0_r;
}

void PlaneMetric::setCoordinate(const Vector3D& p)
{
	if (p == mCoordinate)
		return;
	mCoordinate = p;
	emit transformChanged();
}

Vector3D PlaneMetric::getCoordinate() const
{
	return mCoordinate;
}

void PlaneMetric::setNormal(const Vector3D& p)
{
	if (p == mNormal)
		return;
	mNormal = p;
	emit transformChanged();
}

Vector3D PlaneMetric::getNormal() const
{
	return mNormal;
}

void PlaneMetric::setSpace(CoordinateSystem space)
{
	if (space == mSpace)
		return;

	// keep the absolute position (in ref) constant when changing space.
	Transform3D new_M_old = SpaceHelpers::get_toMfrom(this->getSpace(), space);
	mCoordinate = new_M_old.coord(mCoordinate);
	mNormal = new_M_old.vector(mNormal);

	mSpace = space;

	mSpaceListener->setSpace(space);
//	//TODO connect to the owner of space - data or tool or whatever
//	if (mSpace.mId == csTOOL)
//	{
//		connect(toolManager()->getTool(mSpace.mRefObject).get(),
//						SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this,
//						SIGNAL(transformChanged()));
//	}

	emit transformChanged();
}

CoordinateSystem PlaneMetric::getSpace() const
{
	return mSpace;
}

void PlaneMetric::addXml(QDomNode& dataNode)
{
	Data::addXml(dataNode);

	dataNode.toElement().setAttribute("space", mSpace.toString());
	dataNode.toElement().setAttribute("coord", qstring_cast(mCoordinate));
	dataNode.toElement().setAttribute("normal", qstring_cast(mNormal));
}

void PlaneMetric::parseXml(QDomNode& dataNode)
{
	Data::parseXml(dataNode);

	this->setSpace(CoordinateSystem::fromString(dataNode.toElement().attribute("space", mSpace.toString())));
	this->setCoordinate(Vector3D::fromString(dataNode.toElement().attribute("coord", qstring_cast(mCoordinate))));
	this->setNormal(Vector3D::fromString(dataNode.toElement().attribute("normal", qstring_cast(mNormal))));

}

DoubleBoundingBox3D PlaneMetric::boundingBox() const
{
	// convert both inputs to r space
	Transform3D rM0 = SpaceHelpers::get_toMfrom(this->getSpace(), CoordinateSystem(csREF));
	Vector3D p0_r = rM0.coord(this->getCoordinate());

	return DoubleBoundingBox3D(p0_r, p0_r);
}

QString PlaneMetric::getAsSingleLineString() const
{
	return QString("%1 \"%2\" %3 %4")
			.arg(this->getSingleLineHeader())
			.arg(mSpace.toString())
			.arg(qstring_cast(this->getCoordinate()))
			.arg(qstring_cast(this->getNormal()));
}


}