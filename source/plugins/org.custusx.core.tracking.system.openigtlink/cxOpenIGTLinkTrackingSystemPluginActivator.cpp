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

#include "cxOpenIGTLinkTrackingSystemPluginActivator.h"

#include <QtPlugin>
#include <iostream>

#include "cxOpenIGTLinkStreamerService.h"
#include "cxOpenIGTLinkTrackingSystemService.h"
#include "cxOpenIGTLinkGuiExtenderService.h"
#include "cxOpenIGTLinkClient.h"
#include "cxRegisteredService.h"

namespace cx
{

OpenIGTLinkTrackingSystemPluginActivator::OpenIGTLinkTrackingSystemPluginActivator():
    mIp("10.218.140.127"),
    mPort(18944)
{}

OpenIGTLinkTrackingSystemPluginActivator::~OpenIGTLinkTrackingSystemPluginActivator()
{}

void OpenIGTLinkTrackingSystemPluginActivator::start(ctkPluginContext* context)
{
    mOpenIGTLinkThread.setObjectName("org.custusx.core.openigtlink");
    OpenIGTLinkClient *client = new OpenIGTLinkClient;
    client->setIpAndPort(mIp, mPort); //this is done before client is moved to another thread
    client->moveToThread(&mOpenIGTLinkThread);

    OpenIGTLinkTrackingSystemService* tracking = new OpenIGTLinkTrackingSystemService(client);
    OpenIGTLinkStreamerService *streamer = new OpenIGTLinkStreamerService(client);
    OpenIGTLinkGuiExtenderService* gui = new OpenIGTLinkGuiExtenderService(client);

    mRegistrationGui = RegisteredService::create<OpenIGTLinkGuiExtenderService>(context, gui, GUIExtenderService_iid);
    mRegistrationTracking = RegisteredService::create<OpenIGTLinkTrackingSystemService>(context, tracking, TrackingSystemService_iid);
    mRegistrationStreaming = RegisteredService::create<OpenIGTLinkStreamerService>(context, streamer, StreamerService_iid);

    mOpenIGTLinkThread.start();
}

void OpenIGTLinkTrackingSystemPluginActivator::stop(ctkPluginContext* context)
{
    mOpenIGTLinkThread.quit();
    mOpenIGTLinkThread.wait();

    mRegistrationGui.reset();
    mRegistrationTracking.reset();
    mRegistrationStreaming.reset();
    Q_UNUSED(context);
}

} // namespace cx



