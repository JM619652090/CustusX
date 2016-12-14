#include "cxtestViewServiceMock.h"

#include "cxVisServices.h"
#include "cxLogger.h"

namespace cxtest
{

ViewServiceMock::ViewServiceMock(ctkPluginContext *context) :
	ViewImplService(context)
{
	cx::VisServicesPtr services = cx::VisServices::create(context);
	mBaseMock = ViewManagerMock::create(services);
	mBase = mBaseMock;
}

ViewServiceMock::~ViewServiceMock()
{
}

QWidget *ViewServiceMock::createLayoutWidget(QWidget *parent, int index)
{
	return mBaseMock->createLayoutWidget(parent, index);
}

std::vector<QPointer<cx::ViewCollectionWidget> > ViewServiceMock::getViewCollectionWidgets() const
{
	return mBaseMock->getViewCollectionWidgets();
}

}
