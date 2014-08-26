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

#include "catch.hpp"
#include "cxDataManagerImpl.h"
#include "cxtestDummyDataManager.h"
#include "cxReporter.h"
#include "cxMessageListener.h"

namespace cx
{

TEST_CASE("Core test services correctly contructed/destructed", "[unit]")
{
	cx::MessageListenerPtr messageListener = cx::MessageListener::create();

	cxtest::TestServicesPtr services = cxtest::TestServices::create();
	services.reset();

	CHECK(!messageListener->containsErrors());

//	// [HACK] break loop by removing connection to DataFactory and SpaceProvider
//	cx::DataManagerImplPtr dataManagerImpl;
//	dataManagerImpl = boost::dynamic_pointer_cast<cx::DataManagerImpl>(services.mDataService);
//	if (dataManagerImpl)
//	{
//		dataManagerImpl->setSpaceProvider(cx::SpaceProviderPtr());
//		dataManagerImpl->setDataFactory(cx::DataFactoryPtr());
//		dataManagerImpl.reset();
//	}

//	CHECK(services.mDataFactory.unique());
//	services.mDataFactory.reset();

//	CHECK(services.mSpaceProvider.unique());
//	services.mSpaceProvider.reset();

//	CHECK(services.mDataService.unique());
//	services.mDataService.reset();

//	CHECK(services.mTrackingService.unique());
//	services.mTrackingService.reset();

//	cx::Reporter::shutdown();
}

} // namespace cx


