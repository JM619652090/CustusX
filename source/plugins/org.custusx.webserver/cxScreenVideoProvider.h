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
#ifndef CXSCREENVIDEOPROVIDER_H
#define CXSCREENVIDEOPROVIDER_H

#include <QObject>
#include <QPointer>
#include <QMainWindow>
#include "vtkSmartPointer.h"
#include "cxVisServices.h"
#include "cxForwardDeclarations.h"
#include "cxScreenShotImageWriter.h"

typedef vtkSmartPointer<class vtkWindowToImageFilter> vtkWindowToImageFilterPtr;
typedef vtkSmartPointer<class vtkPNGWriter> vtkPNGWriterPtr;
typedef vtkSmartPointer<class vtkUnsignedCharArray> vtkUnsignedCharArrayPtr;

namespace cx
{

class SecondaryViewLayoutWindow: public QWidget
{
Q_OBJECT

public:
	SecondaryViewLayoutWindow(QWidget* parent, ViewServicePtr viewService);
	~SecondaryViewLayoutWindow() {}

	void tryShowOnSecondaryScreen();
    int mSecondaryLayoutId;

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void hideEvent(QCloseEvent* event);
	virtual void closeEvent(QCloseEvent *event);
private:
	QString toString(QRect r) const;
	int findSmallestSecondaryScreen();

	ViewServicePtr mViewService;
};

class ScreenVideoProvider : public QObject
{
	Q_OBJECT
public:
	ScreenVideoProvider(VisServicesPtr services);

    class ViewCollectionWidget* getSecondaryLayoutWidget();
    void saveScreenShot(QImage image, QString id);
	QByteArray generatePNGEncoding(QImage image);
	QPixmap grabScreen(unsigned screenid);
    void showSecondaryLayout(QSize size, QString layout);
	QImage grabSecondaryLayout();
    void closeSecondaryLayout();
private:
	VisServicesPtr mServices;
	SecondaryViewLayoutWindow* mSecondaryViewLayoutWindow;
	QPointer<class QWidget> mTopWindow;
	ScreenShotImageWriter mWriter;
	void setWidgetToNiceSizeInLowerRightCorner(QSize size);
};

} // namespace cx


#endif // CXSCREENVIDEOPROVIDER_H
