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

#ifndef CXLAYOUTWIDGETMIXED_H
#define CXLAYOUTWIDGETMIXED_H

#include "cxView.h"
#include "cxLayoutData.h"
#include "cxViewCache.h"
#include "cxViewWidget.h"
#include "cxLayoutWidget.h"

class QGridLayout;

namespace cx
{
class LayoutWidgetUsingViewCollection;
class LayoutWidgetUsingViewWidgets;

/**
 * Widget for displaying Views, minimizing number of renderwindows but keeping
 * the 3D views in separate renderwindows.
 *
 * The rationale behind this class is:
 *  - The cost of rendering several vtkRenderWindows is high, especially on new
 *    Linux NVidia cards/drivers. I.e. try to reduce the number using
 *    LayoutWidgetUsingViewCollection.
 *  - vtkRenderWindowInteractor is connected to a vtkRenderWindow, and this is
 *    used in 3D views, thus we need unique vtkRenderWindows for each 3D View.
 *
 * The solution here is to use a LayoutWidgetUsingViewCollection as basis, then
 * adding LayoutWidgetUsingViewWidgets each containing one 3D view, and placing
 * these on top of the basis in the QGridLayout.
 */
class LayoutWidgetMixed : public LayoutWidget
{
	Q_OBJECT
public:
	LayoutWidgetMixed();
	~LayoutWidgetMixed();

	ViewPtr addView(View::Type type, LayoutRegion region);
	void clearViews();
	virtual void setModified();
	virtual void render();
	virtual void setGridSpacing(int val);
	virtual void setGridMargin(int val);

private:
	void addWidgetToLayout(QGridLayout* layout, QWidget* widget, LayoutRegion region);
	void setStretchFactors(LayoutRegion region, int stretchFactor);
	LayoutRegion mBaseRegion;
	LayoutRegion mTotalRegion;
	QGridLayout* mLayout;
	LayoutWidgetUsingViewCollection* mBaseLayout;
	std::vector<LayoutRegion> mOverlayRegions;
	std::vector<LayoutWidgetUsingViewWidgets*> mOverlays;
};



} // namespace cx

#endif // CXLAYOUTWIDGETMIXED_H