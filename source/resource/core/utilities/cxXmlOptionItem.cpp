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
 * sscXmlOptionItem.cpp
 *
 *  Created on: May 28, 2010
 *      Author: christiana
 */
#include "cxXmlOptionItem.h"

#include <map>
#include <iostream>
#include <QFile>
#include <QTextStream>
#include <QDomElement>
#include <QStringList>

#include "cxReporter.h"
#include "cxTypeConversions.h"

namespace cx
{

/**Helper class for reusing opened documents instead of creating new instances to them.
 *
 */
class SharedDocuments
{
public:
	static SharedDocuments* getInstance()
	{
		if (!mInstance)
			mInstance = new SharedDocuments();
		return mInstance;
	}

	/** read the document given by filename from disk,
	  * but use a cached document if already loaded.
	  */
	QDomDocument loadDocument(const QString& filename)
	{
		QDomDocument retval;
		retval = this->getDocument(filename);
		if (retval.isNull())
		{
			retval = this->readFromFile(filename);
			this->addDocument(filename, retval);
		}
		return retval;
	}

private:
	SharedDocuments() : mDocumentMutex(QMutex::Recursive) {}

	QDomDocument getDocument(const QString& filename)
	{
		QMutexLocker lock(&mDocumentMutex);
		DocumentMap::iterator iter = mDocuments.find(filename);
		// if filename found, attempt to retrieve document from node.
		if (iter != mDocuments.end())
		{
			return iter->second.ownerDocument();
		}
		return QDomDocument(); // null node
	}

	void addDocument(const QString& filename, QDomDocument document)
	{
		QMutexLocker lock(&mDocumentMutex);
		mDocuments[filename] = document.documentElement();
	}

	QDomDocument readFromFile(QString filename)
	{
		QFile file(filename);
		if (!file.open(QIODevice::ReadOnly))
		{
			return QDomDocument();
		}

		QDomDocument loadedDoc;
		QString error;
		int line, col;
		if (!loadedDoc.setContent(&file, &error, &line, &col))
		{
			QString msg = QString("error setting xml content [%1,%2] %3").arg(line).arg(col).arg(error);
			reportWarning(msg);
		}
		file.close();
		return loadedDoc;
	}

	static SharedDocuments* mInstance;
	typedef std::map<QString, QDomElement> DocumentMap;
	QMutex mDocumentMutex;
	DocumentMap mDocuments; ///< QDomElement points to the documentElement. This acts as a weak_ptr.
};
///--------------------------------------------------------
SharedDocuments* SharedDocuments::mInstance = NULL;
///--------------------------------------------------------

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

XmlOptionItem::XmlOptionItem(const QString& uid, QDomElement root) :
	mUid(uid), mRoot(root)
{

}

QString XmlOptionItem::readValue(const QString& defval) const
{
	// read value is present
	QDomElement item = this->findElemFromUid(mUid, mRoot);
	if (!item.isNull() && item.hasAttribute("value"))
	{
		return item.attribute("value");
	}
	return defval;
}

void XmlOptionItem::writeValue(const QString& val)
{
	if (mRoot.isNull())
		return;
	QDomElement item = findElemFromUid(mUid, mRoot);
	// create option if not present
	if (item.isNull())
	{
		item = mRoot.ownerDocument().createElement("option");
		item.setAttribute("id", mUid);
		mRoot.appendChild(item);
	}
	item.setAttribute("value", val);
}

QDomElement XmlOptionItem::findElemFromUid(const QString& uid, QDomNode root) const
{
	QDomNodeList settings = root.childNodes();
	for (int i = 0; i < settings.size(); ++i)
	{
		QDomElement item = settings.item(i).toElement();
		if (item.attribute("id") == uid)
			return item;
	}
	return QDomElement();
}

/// -------------------------------------------------------
/// -------------------------------------------------------
/// -------------------------------------------------------

XmlOptionFile XmlOptionFile::createNull()
{
	XmlOptionFile retval;
	retval.mDocument = QDomDocument();
	retval.mCurrentElement = QDomElement();
	return retval;
}

XmlOptionFile::XmlOptionFile()
{
	mDocument.appendChild(mDocument.createElement("root"));
	mCurrentElement = mDocument.documentElement();
}

XmlOptionFile::XmlOptionFile(QString filename, QString name) :
	mFilename(filename)
{
	mDocument = SharedDocuments::getInstance()->loadDocument(filename);

//	if (!mSharedDocuments)
//	{
//		mSharedDocuments = new SharedDocuments;
//	}

//	mDocument = mSharedDocuments->getDocument(filename);
//	if (mDocument.isNull())
//	{
//		//    std::cout << "    no doc found, creating new. " << std::endl;
//		this->load();
//		mSharedDocuments->addDocument(filename, mDocument);
//	}
//	else
//	{
//		//    std::cout << "    reusing cached document" << std::endl;
//	}

	mCurrentElement = mDocument.documentElement();

	if (mCurrentElement.isNull())
	{
		mDocument.appendChild(mDocument.createElement("root"));
		mCurrentElement = mDocument.documentElement();
	}
}

XmlOptionFile::~XmlOptionFile()
{}

QString XmlOptionFile::getFileName()
{
	return mFilename;
}

bool XmlOptionFile::isNull() const
{
	if(mCurrentElement.isNull() || mDocument.isNull())
		return true;
	return false;
}

XmlOptionFile XmlOptionFile::root() const
{
	XmlOptionFile retval = *this;
	retval.mCurrentElement = mDocument.documentElement();
	return retval;
}

XmlOptionFile XmlOptionFile::descend(QString element) const
{
	XmlOptionFile retval = *this;
	retval.mCurrentElement = retval.getElement(element);
	return retval;
}

XmlOptionFile XmlOptionFile::descend(QString element, QString attributeName, QString attributeValue) const
{
	XmlOptionFile retval = this->tryDescend(element, attributeName, attributeValue);
	if (!retval.getDocument().isNull())
		return retval;

	// create a new element if not found
	retval = *this;
	QDomElement current = retval.getDocument().createElement(element);
	current.setAttribute(attributeName, attributeValue);
	retval.mCurrentElement.appendChild(current);
	retval.mCurrentElement = current;
	return retval;
}

XmlOptionFile XmlOptionFile::tryDescend(QString element, QString attributeName, QString attributeValue) const
{
	XmlOptionFile retval = *this;

	QDomNodeList presetNodeList = retval.getElement().elementsByTagName(element);
	for (int i = 0; i < presetNodeList.count(); ++i)
	{
		QDomElement current = presetNodeList.item(i).toElement();
		QString name = current.attribute(attributeName);
		if (attributeValue == name)
		{
			retval.mCurrentElement = current;
			return retval;
		}
	}

	return XmlOptionFile::createNull();
}

XmlOptionFile XmlOptionFile::ascend() const
{
	XmlOptionFile retval = *this;
	retval.mCurrentElement = mCurrentElement.parentNode().toElement();
	if (retval.mCurrentElement.isNull())
		return *this;
	return retval;
}

/**return an element child of parent. Create if not existing.
 */
QDomElement XmlOptionFile::safeGetElement(QDomElement parent, QString childName)
{
	QDomElement child = parent.namedItem(childName).toElement();

	if (child.isNull())
	{
		child = mDocument.createElement(childName);
		parent.appendChild(child);
	}

	return child;
}

void XmlOptionFile::printDocument()
{
	QTextStream stream(stdout);
	stream << "\n" << mDocument.toString(4) << "\n";
}

void XmlOptionFile::printElement()
{
	QTextStream stream(stdout);
	stream << "\n";
	mCurrentElement.save(stream, 4);
	stream << "\n";
}

QDomDocument XmlOptionFile::getDocument()
{
	return mDocument;
}

QDomElement XmlOptionFile::getElement()
{
	return mCurrentElement;
}

QDomElement XmlOptionFile::getElement(QString level1)
{
	QDomElement elem1 = this->safeGetElement(mCurrentElement, level1);
	return elem1;
}

QDomElement XmlOptionFile::getElement(QString level1, QString level2)
{
	QDomElement elem1 = this->safeGetElement(mCurrentElement, level1);
	QDomElement elem2 = this->safeGetElement(elem1, level2);
	return elem2;
}

void XmlOptionFile::removeChildren()
{
	while (mCurrentElement.hasChildNodes())
		mCurrentElement.removeChild(mCurrentElement.firstChild());
}

void XmlOptionFile::deleteNode()
{
	QDomNode parentNode = mCurrentElement.parentNode();
	parentNode.removeChild(mCurrentElement);
	mCurrentElement = QDomElement();// Create null element
}

void XmlOptionFile::save()
{
	if (mFilename.isEmpty())
		return; //Don't do anything if on filename isn't supplied

	QFile file(mFilename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream stream(&file);
		stream << mDocument.toString(4);
		file.close();
//		report("Created " + file.fileName());
	}
	else
	{
		reportError("XmlOptionFile::save() Could not open " + file.fileName() + " Error: "
			+ file.errorString());
	}
}

//void XmlOptionFile::load()
//{
//	QFile file(mFilename);
//	if (!file.open(QIODevice::ReadOnly))
//	{
//		// ok to not find file - we have nice defaults.
//		//reportWarning("file not found: "+ QString(defPath+filename).toStdString());
//	}
//	else
//	{
//		QDomDocument loadedDoc;
//		QString error;
//		int line, col;
//		if (!loadedDoc.setContent(&file, &error, &line, &col))
//		{
//			reportWarning("error setting xml content [" + qstring_cast(line) + "," + qstring_cast(
//				col) + "]" + qstring_cast(error));
//		}
//		file.close();
//		mDocument = loadedDoc;
//		mCurrentElement = mDocument.documentElement();
//	}

//}

} // namespace cx