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
#ifndef CXSECONDARYVIEWLAYOUTWINDOW_H
#define CXSECONDARYVIEWLAYOUTWINDOW_H

#include "cxGuiExport.h"

#include <QMainWindow>

namespace cx
{

/**
 * \brief Experimental class for IPad usage.
 *
 * This detached main window can be moved onto a secondary screen.
 * The use case was a IPad where scrolling etc could control the
 * CustusX scene.
 *
 * \sa SecondaryMainWindow
 *
 * \ingroup cx_gui
 * \date 05.11.2013
 * \author christiana
 */
class cxGui_EXPORT SecondaryViewLayoutWindow: public QMainWindow
{
Q_OBJECT

public:
	SecondaryViewLayoutWindow(QWidget* parent);
	~SecondaryViewLayoutWindow();

	void tryShowOnSecondaryScreen();

protected:
	virtual void showEvent(QShowEvent* event);
	virtual void hideEvent(QCloseEvent* event);
	virtual void closeEvent(QCloseEvent *event);
private:
	QString toString(QRect r) const;
	int findSmallestSecondaryScreen();
};

} // namespace cx

#endif // CXSECONDARYVIEWLAYOUTWINDOW_H
