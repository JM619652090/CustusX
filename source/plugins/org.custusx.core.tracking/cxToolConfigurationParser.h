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

#ifndef CXTOOLCONFIGURATIONPARSER_H_
#define CXTOOLCONFIGURATIONPARSER_H_

#include "org_custusx_core_tracking_Export.h"

#include <utility>
#include <map>
#include <vector>
#include <QString>
#include <QDomDocument>
#include "cxToolUsingIGSTK.h"
#include "cxIgstkTracker.h"
#include "cxIgstkTool.h"

namespace cx
{

/**
 * \brief Class for reading the files defining a CustusX tool
 * \ingroup cx_service_tracking
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_tracking_EXPORT ConfigurationFileParser
{
public:
	typedef std::pair<QString, bool> ToolFileAndReference;
	typedef std::vector<ToolFileAndReference> ToolFilesAndReferenceVector;
	typedef std::map<TRACKING_SYSTEM, ToolFilesAndReferenceVector> TrackersAndToolsMap;
	struct Configuration
	{
		QString mFileName; ///< absolute path and filename for the new config file
		CLINICAL_APPLICATION mClinical_app; ///< the clinical application this config is made for
		TrackersAndToolsMap mTrackersAndTools; ///< the trackers and tools (relative path) that should be used in the config
		Configuration() : mClinical_app(mdCOUNT) {}
	};

public:
	ConfigurationFileParser(QString absoluteConfigFilePath, QString loggingFolder = "");
	~ConfigurationFileParser();

	CLINICAL_APPLICATION getApplicationapplication();
	std::vector<IgstkTracker::InternalStructure> getTrackers();
	std::vector<QString> getAbsoluteToolFilePaths();
	QString getAbsoluteReferenceFilePath();

	static QString getTemplatesAbsoluteFilePath();
	static void saveConfiguration(Configuration& config);

private:
	void setConfigDocument(QString configAbsoluteFilePath);
	bool isConfigFileValid();
	QString getAbsoluteToolFilePath(QDomElement toolfileelement); ///< get the absolute toolfile path for a toolfile element containg a relative toolfile path
	QString findXmlFileWithDirNameInPath(QString path);
	QString searchForExistingToolFilePath(QString relativeToolFilePath);
	static QString convertToRelativeToolFilePath(QString configFilename, QString absoluteToolFilePath);
	static QString compactVirtualPath(QString filepath);

	QString mConfigurationFilePath; ///< absolute path to the configuration file
	QString mLoggingFolder; ///< absolutepath to the logging folder

	QDomDocument mConfigureDoc; ///< the config xml document
	const QString mConfigTag, mConfigTrackerTag, mConfigTrackerToolFile; ///< names of necessary tags in the configuration file
	const QString mTypeAttribute, mClinicalAppAttribute, mReferenceAttribute; ///< names of necessary attributes in the configuration file
};

/**
 * \brief Class for reading the files defining a CustusX tool
 * \ingroup cx_service_tracking
 *
 * \date 21. march 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class org_custusx_core_tracking_EXPORT ToolFileParser
{
public:
	ToolFileParser(QString absoluteToolFilePath, QString loggingFolder = "");
	~ToolFileParser();

	IgstkTool::InternalStructure getTool();

	static QString getTemplatesAbsoluteFilePath();

private:
	QDomNode getToolNode(QString toolAbsoluteFilePath);
	igstk::Transform readCalibrationFile(QString absoluteFilePath);

	QString mToolFilePath; ///< absolutepath to the tool file
	QString mLoggingFolder; ///< absolutepath to the logging folder

	QDomDocument mToolDoc; ///< the tool xml document
	const QString mToolTag, mToolTypeTag, mToolIdTag, mToolNameTag, mToolDescriptionTag, mToolManufacturerTag,
					mToolClinicalAppTag, mToolGeoFileTag, mToolPicFileTag, mToolDocFileTag, mToolInstrumentTag,
					mToolInstrumentTypeTag, mToolInstrumentIdTag, mToolInstrumentNameTag,
					mToolInstrumentManufacturerTag, mToolInstrumentScannerIdTag, mToolInstrumentDescriptionTag,
					mToolSensorTag, mToolSensorTypeTag, mToolSensorIdTag, mToolSensorNameTag, mToolSensorWirelessTag,
					mToolSensorDOFTag, mToolSensorPortnumberTag, mToolSensorChannelnumberTag,
					mToolSensorReferencePointTag, mToolSensorManufacturerTag, mToolSensorDescriptionTag,
					mToolSensorRomFileTag, mToolCalibrationTag, mToolCalibrationFileTag;
	///< names of necessary tags in the tool file
};

/**
 * @}
 */
} //namespace cx
#endif /* CXTOOLCONFIGURATIONPARSER_H_ */