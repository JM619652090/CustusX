#include "sscSliceProxy.h"

#include <boost/lexical_cast.hpp>
#include "sscDataManager.h"
#include "sscToolManager.h"

namespace ssc
{

SliceProxy::SliceProxy()
{
	connect(ssc::DataManager::getInstance(), SIGNAL(centerChanged()),this, SLOT(centerChangedSlot()) ) ;
	//TODO connect to toolmanager rMpr changed
	mCutplane.setFixedCenter(ssc::DataManager::getInstance()->getCenter());
}

SliceProxy::~SliceProxy()
{
}

void SliceProxy::setTool(ToolPtr tool)
{
	if (mTool)
	{
		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));		

		disconnect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(toolTransformAndTimestamp(Transform3D,double)));
		disconnect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));		
	}	
	
	mTool = tool;
	
	if (mTool)
	{
		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SLOT(toolTransformAndTimestampSlot(Transform3D,double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SLOT(toolVisibleSlot(bool)));		

		connect(mTool.get(), SIGNAL(toolTransformAndTimestamp(Transform3D,double)), this, SIGNAL(toolTransformAndTimestamp(Transform3D,double)));
		connect(mTool.get(), SIGNAL(toolVisible(bool)), this, SIGNAL(toolVisible(bool)));		
	}	
	
	// TODO fill data from tool
	changed();	
}

void SliceProxy::toolTransformAndTimestampSlot(Transform3D prMt, double timestamp)
{
	//std::cout << "proxy get transform" << std::endl;
	Transform3D rMpr = *ssc::ToolManager::getInstance()->get_rMpr();
	mCutplane.setToolPosition(rMpr*prMt);	
	changed();
}

void SliceProxy::toolVisibleSlot(bool visible)
{
	
}

void SliceProxy::centerChangedSlot()
{
	mCutplane.setFixedCenter(ssc::DataManager::getInstance()->getCenter());
	//std::cout << "center changed: " + boost::lexical_cast<std::string>(ssc::DataManager::instance()->getCenter());
	changed();
}

void SliceProxy::setOrientation(ORIENTATION_TYPE orientation)
{
	mCutplane.setOrientationType(orientation);	
	changed();
}

void SliceProxy::setPlane(PLANE_TYPE plane)
{
	mCutplane.setPlaneType(plane);	
	changed();
}

void SliceProxy::setFollowType(FOLLOW_TYPE followType)
{
	mCutplane.setFollowType(followType);	
	changed();
}

ToolPtr SliceProxy::getTool()
{
	return mTool;
}

Transform3D SliceProxy::get_sMr()
{
	SlicePlane plane = mCutplane.getPlane();
	//std::cout << "proxy get transform :\n"+boost::lexical_cast<std::string>(plane) << std::endl;
	return createTransformIJC(plane.i, plane.j, plane.c).inv();	
}

void SliceProxy::changed()
{
	emit transformChanged(get_sMr());
}


} // namespace ssc
