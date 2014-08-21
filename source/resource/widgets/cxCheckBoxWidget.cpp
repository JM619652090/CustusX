// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscCheckBoxWidget.cpp
 *
 *  Created on: Feb 7, 2011
 *      Author: christiana
 */

#include "cxCheckBoxWidget.h"
#include <QGridLayout>
#include "cxHelperWidgets.h"

namespace cx
{

CheckBoxWidget::CheckBoxWidget(QWidget* parent, BoolDataAdapterPtr dataInterface, QGridLayout* gridLayout, int row) :
    OptimizedUpdateWidget(parent)
{
	this->setEnabled(dataInterface->getEnabled());

	mData = dataInterface;
    connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

    mLabel = new QLabel(this);
    mLabel->setText(dataInterface->getDisplayName());

	mCheckBox = new QCheckBox(this);
	connect(mCheckBox, SIGNAL(toggled(bool)), this, SLOT(valueChanged(bool)));

    if (gridLayout) // add to input gridlayout
    {
        gridLayout->addLayout(mergeWidgetsIntoHBoxLayout(mLabel, addDummyMargin(this)), row, 0);
        gridLayout->addWidget(mCheckBox, row, 1);
    }
    else // add directly to this
    {
        QHBoxLayout* topLayout = new QHBoxLayout;
        topLayout->setMargin(0);
        this->setLayout(topLayout);

        topLayout->addWidget(mLabel);
        topLayout->addWidget(mCheckBox, 1);
    }

    this->setModified();
}

void CheckBoxWidget::valueChanged(bool val)
{
	if (val == mData->getValue())
		return;
	mData->setValue(val);
}

void CheckBoxWidget::prePaintEvent()
{
	mCheckBox->blockSignals(true);

	this->setEnabled(mData->getEnabled());

	mCheckBox->setChecked(mData->getValue());
	mCheckBox->setToolTip(mData->getHelp());

	mCheckBox->blockSignals(false);
}

// --------------------------------------------------------


}
