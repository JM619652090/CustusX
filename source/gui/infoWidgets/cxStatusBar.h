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

#ifndef CXSTATUSBAR_H_
#define CXSTATUSBAR_H_

#include <QStatusBar>
#include <map>
#include "cxReporter.h"
#include "cxForwardDeclarations.h"
#include "cxLegacySingletons.h"


class QLabel;
class QPixmap;
class QAction;
class QToolButton;

namespace cx
{
/**
 * \class StatusBar
 *
 * \brief Statusbar with extended functionality.
 * \ingroup cx_gui
 *
 * \date Jan 21, 2009
 * \author Janne Beate Bakeng, SINTEF
 */
class StatusBar: public QStatusBar
{
  Q_OBJECT

public:
  StatusBar(); ///< connects signals and slots
  virtual ~StatusBar(); ///< empty

private slots:
  void connectToToolSignals(); ///< connect to all available tools
  void disconnectFromToolSignals(); ///< disconnect from all tool
//  void receiveToolVisible(); ///< updates the color label for a tool
//  void receiveToolDominant(); ///< updates the color label for a tool
  void renderingFpsSlot(int numFps); ///< Show rendered frames per seconds
  void grabbingFpsSlot(int numFps); ///< Show grabbed frames per seconds
  void grabberConnectedSlot(bool connected);
  void tpsSlot(int numTps); ///< Show transforms per seconds
  void showMessageSlot(Message message); ///< prints the incomming message to the statusbar
  void updateToolButtons();

private:
  void activateTool(QString uid);
  QString getToolStyle(bool visible, bool initialized, bool dominant);

  QLabel* mRenderingFpsLabel; ///< Label for showing rendering FPS
  QLabel* mGrabbingInfoLabel; ///< Label for showing info about the grabber
  QLabel* mTpsLabel; ///< Label for showing TPS
//  QLabel* mMessageLevelLabel;
  QToolButton* mMessageLevelLabel;

  struct ToolData
  {
	  boost::shared_ptr<QAction> mAction;
	  boost::shared_ptr<QToolButton> mButton;
	  ToolPtr mTool;
  };
  std::vector<ToolData> mToolData;
};
}

#endif /* CXSTATUSBAR_H_ */
