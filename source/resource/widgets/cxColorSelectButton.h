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
#ifndef CXCOLORSELECTBUTTON_H_
#define CXCOLORSELECTBUTTON_H_

#include "cxResourceWidgetsExport.h"

#include <QToolButton>
#include <QColor>

namespace cx
{

/**
 * \brief Convenience button for chosing color.
 * Will open a color select dialog, and color
 * the button itself in the selected color.
 *
 * \ingroup cx_resource_widgets
 */
class cxResourceWidgets_EXPORT ColorSelectButton : public QToolButton
{
  Q_OBJECT
public:
  ColorSelectButton(QString caption, QColor color=Qt::white, QWidget* parent=NULL);
  void setColor(QColor color);
signals:
  void colorChanged(QColor);

private:
  QColor mColor;
private slots:
  void clickedSlot();
};
//------------------------------------------------------------------------------

/**
 * Helper class for setting a color in settings()
 */
class cxResourceWidgets_EXPORT SelectColorSettingButton : public ColorSelectButton
{
	Q_OBJECT

public:
	SelectColorSettingButton(QString caption, QString id, QString help);
	void storeColor(QColor color);

private:
	QString mId;
};


}

#endif /* CXCOLORSELECTBUTTON_H_ */
