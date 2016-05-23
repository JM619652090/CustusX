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

#include "cxCustomMetricRep.h"

#include "cxView.h"

#include <vtkVectorText.h>
#include <vtkFollower.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkRenderWindow.h>
#include "cxTypeConversions.h"
#include "vtkTextActor.h"
#include "cxGraphicalPrimitives.h"
#include "cxCustomMetric.h"
#include "cxGraphicalPrimitives.h"
#include "vtkMatrix4x4.h"
#include "cxGraphicalTorus3D.h"
#include "cxGraphicalDisk.h"

namespace cx
{

CustomMetricRepPtr CustomMetricRep::New(const QString& uid)
{
    return wrap_new(new CustomMetricRep(), uid);
}

CustomMetricRep::CustomMetricRep()
{
}

void CustomMetricRep::clear()
{
	DataMetricRep::clear();
	mTorus.reset();
	mDisk.reset();
}

CustomMetricPtr CustomMetricRep::getCustomMetric()
{
    return boost::dynamic_pointer_cast<CustomMetric>(mMetric);
}

void CustomMetricRep::onModifiedStartRender()
{
	if (!mMetric)
		return;

	this->updateTorus();
	this->updateDisc();

	this->drawText();
}

void CustomMetricRep::updateTorus()
{
	if (!mMetric)
		return;

    CustomMetricPtr custom = this->getCustomMetric();

    if (custom->getFlat())
	{
		mTorus.reset();
		return;
	}

	if (!mTorus && this->getView() && mMetric)
		mTorus.reset(new GraphicalTorus3D(this->getRenderer()));

	if (!mTorus)
		return;

    mTorus->setPosition(custom->getPosition());
    mTorus->setDirection(custom->getDirection());
    mTorus->setRadius(custom->getRadius());
    mTorus->setThickness(custom->getThickness());
    mTorus->setColor(custom->getColor());
}

void CustomMetricRep::updateDisc()
{
	if (!mMetric)
		return;

    CustomMetricPtr custom = this->getCustomMetric();

    if (!custom->getFlat())
	{
		mDisk.reset();
		return;
	}

	if (!mDisk && this->getView() && mMetric)
	{
		mDisk.reset(new GraphicalDisk());
		mDisk->setRenderer(this->getRenderer());
	}

	if (!mDisk)
		return;

    mDisk->setPosition(custom->getPosition());
    mDisk->setDirection(custom->getDirection());
    mDisk->setRadius(custom->getRadius());
    mDisk->setHeight(custom->getHeight());
    mDisk->setColor(custom->getColor());
    mDisk->setOutlineColor(custom->getColor());
    mDisk->setOutlineWidth(custom->getThickness());
	mDisk->setFillVisible(false);
	mDisk->setLighting(true);

	mDisk->update();
}

}
