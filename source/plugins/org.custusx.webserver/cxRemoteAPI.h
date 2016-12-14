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
#ifndef CXREMOTEAPI_H
#define CXREMOTEAPI_H

#include <QObject>
#include "cxVisServices.h"

#include "org_custusx_webserver_Export.h"

namespace cx
{
typedef boost::shared_ptr<class RemoteAPI> RemoteAPIPtr;
typedef boost::shared_ptr<class LayoutVideoSource> LayoutVideoSourcePtr;
class ScreenVideoProvider;

/**
 * API indended to be callable from external applications,
 * e.g. over http or igtl.
 */
class org_custusx_webserver_EXPORT RemoteAPI : public QObject
{
	Q_OBJECT
public:
	explicit RemoteAPI(VisServicesPtr services);

	QStringList getAvailableLayouts() const;
	void createLayoutWidget(QSize size, QString layout);
    void closeLayoutWidget();
    LayoutVideoSourcePtr startStreaming(); ///< stop streaming by destroying the returned object
    QImage grabLayout();
    QImage grabScreen();

	VisServicesPtr getServices() { return mServices; }

private:
	VisServicesPtr mServices;
    ScreenVideoProvider* mScreenVideo;
};

} // namespace cx


#endif // CXREMOTEAPI_H
