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

#include "cxCameraStyleInteractor.h"


#include <QAction>
#include "cxLogger.h"
#include "cxView.h"
#include "cxLogger.h"
#include "cxViewGroupData.h"

namespace cx
{

CameraStyleInteractor::CameraStyleInteractor() :
	mCameraStyleGroup(NULL)
{
}

void CameraStyleInteractor::connectCameraStyle(ViewGroupDataPtr vg)
{
	if (mGroup)
		disconnect(mGroup.get(), &ViewGroupData::optionsChanged, this, &CameraStyleInteractor::updateActionGroup);
	mGroup = vg;
	if (mGroup)
		connect(mGroup.get(), &ViewGroupData::optionsChanged, this, &CameraStyleInteractor::updateActionGroup);
	this->updateActionGroup();
}

QActionGroup* CameraStyleInteractor::getInteractorStyleActionGroup()
{
	if (mCameraStyleGroup)
		return mCameraStyleGroup;

	mCameraStyleGroup = new QActionGroup(this);
	mCameraStyleGroup->setExclusive(true);

	this->addInteractorStyleAction("Normal Camera", mCameraStyleGroup,
								   enum2string(cstDEFAULT_STYLE),
								   QIcon(":/icons/camera-n.png"),
								   "Set 3D interaction to the normal camera-oriented style.");
	this->addInteractorStyleAction("Tool", mCameraStyleGroup,
								   enum2string(cstTOOL_STYLE),
								   QIcon(":/icons/camera-t.png"),
								   "Camera following tool.");
	this->addInteractorStyleAction("Angled Tool", mCameraStyleGroup,
								   enum2string(cstANGLED_TOOL_STYLE),
								   QIcon(":/icons/camera-at.png"),
								   "Camera following tool (Placed at an angle of 20 degrees).");
	this->addInteractorStyleAction("Unicam", mCameraStyleGroup,
								   enum2string(cstUNICAM_STYLE),
								   QIcon(":/icons/camera-u.png"),
								   "Set 3D interaction to a single-button style, useful for touch screens.");
	return mCameraStyleGroup;
}

void CameraStyleInteractor::addInteractorStyleAction(QString caption, QActionGroup* group, QString uid, QIcon icon,
				QString helptext)
{
	QAction* action = new QAction(caption, group);
	action->setIcon(icon);
	action->setCheckable(true);
	action->setData(uid);
	action->setToolTip(helptext);
	action->setWhatsThis(helptext);
	connect(action, SIGNAL(triggered(bool)), this, SLOT(setInteractionStyleActionSlot()));
}

void CameraStyleInteractor::updateActionGroup()
{
	QString currentStyle;
	if (mGroup)
		currentStyle = enum2string(mGroup->getOptions().mCameraStyle.getStyle());

	if(mCameraStyleGroup)
	{
		QList<QAction*> actions = mCameraStyleGroup->actions();
		for (int i=0; i<actions.size(); ++i)
		{
			actions[i]->setEnabled(mGroup!=0);
			bool check = actions[i]->data().toString() == currentStyle;
			if(actions[i]->isChecked() != check)
				actions[i]->setChecked(check);
		}
	}
}

void CameraStyleInteractor::setInteractionStyleActionSlot()
{
	QAction* theAction = static_cast<QAction*>(sender());
	if(!theAction)
		return;

	QString uid = theAction->data().toString();
	CAMERA_STYLE_TYPE newStyle = string2enum<cx::CAMERA_STYLE_TYPE>(uid);
	if (newStyle==cstCOUNT)
		return;

	if (mGroup)
	{
		ViewGroupData::Options options = mGroup->getOptions();
		options.mCameraStyle = CameraStyleData(newStyle);
		mGroup->setOptions(options);
	}
}



}//namespace cx
