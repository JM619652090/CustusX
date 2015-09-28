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


#ifndef CXOPENIGTLINKCLIENT_H
#define CXOPENIGTLINKCLIENT_H

#include "org_custusx_core_openigtlink_Export.h"

#include <map>
#include <QObject>
#include <QMutex>
#include <QMutexLocker>
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"
#include "igtlStringMessage.h"
#include "cxIGTLinkUSStatusMessage.h"

#include "cxSocketConnection.h"
#include "cxTransform3D.h"
#include "cxImage.h"
#include "cxProbeDefinition.h"
#include "cxLogger.h"
#include "cxDialect.h"
#include "boost/function.hpp"

typedef boost::shared_ptr<QThread> QThreadPtr;

namespace cx {

typedef boost::shared_ptr<class OpenIGTLinkClient> OpenIGTLinkClientPtr;

/**
 * @brief The OpenIGTLinkClient class handles incoming OpenIGTLink packages.
 *
 * To specify how packages should be handled you can specify different kind of
 * supported dialects, which are a way to handle the way different OpenIGTLink
 * servers send packages.
 *
 */

class org_custusx_core_openigtlink_EXPORT OpenIGTLinkClient : public SocketConnection
{
    Q_OBJECT
public:

	explicit OpenIGTLinkClient(QString uid, QObject *parent = 0);
	virtual ~OpenIGTLinkClient();

    //thread safe
	QString getUid() const { return mUid; }
	virtual void setConnectionInfo(ConnectionInfo info);
	QStringList getAvailableDialects() const;
	void invoke(boost::function<void()> func);

	// not thread-safe: use invoke to call in this thread
	void sendMessage(ImagePtr image);
	void sendMessage(MeshPtr image);

signals:
    void transform(QString devicename, Transform3D transform, double timestamp);
    void calibration(QString devicename, Transform3D calibration);
    void image(ImagePtr image);
	void mesh(MeshPtr image);
	void probedefinition(QString devicename, ProbeDefinitionPtr definition);
    void igtlimage(IGTLinkImageMessage::Pointer igtlimage);
    void usstatusmessage(IGTLinkUSStatusMessage::Pointer message);

private slots:
    virtual void internalDataAvailable();
	void onInvoke(boost::function<void()> func);

protected:

private:
	DialectPtr initDialect(DialectPtr value);
	void setDialect(QString dialectname);
	bool receiveHeader(const igtl::MessageHeader::Pointer header) const;
    bool receiveBody(const igtl::MessageHeader::Pointer header);
	qint64 skip(qint64 maxSizeBytes) const;

    template <typename T>
    bool receive(const igtl::MessageBase::Pointer header);

    QMutex mMutex;

    igtl::MessageHeader::Pointer mHeader;
    bool mHeaderReceived;

    DialectPtr mDialect;
    typedef std::map<QString, DialectPtr> DialectMap;
    DialectMap mAvailableDialects;
	const QString mUid;
};

typedef boost::shared_ptr<class OpenIGTLinkClientThreadHandler> OpenIGTLinkClientThreadHandlerPtr;

/** Encapsulates running of the OpenIGTLinkClient in a thread.
 *  Lifetime of the thread equals that of this object.
 *
 */
class org_custusx_core_openigtlink_EXPORT OpenIGTLinkClientThreadHandler : public QObject
{
	Q_OBJECT
public:
	explicit OpenIGTLinkClientThreadHandler(QString threadname);
	~OpenIGTLinkClientThreadHandler();
	OpenIGTLinkClient* client();

	StringPropertyBasePtr getDialectOption() { return mDialects; }
	StringPropertyBasePtr getIpOption() { return mIp; }
	DoublePropertyBasePtr getPortOption() { return mPort; }
	StringPropertyBasePtr getRoleOption() { return mRole; }

private:
	void onConnectionInfoChanged();
	void onPropertiesChanged();
	StringPropertyBasePtr mIp;
	DoublePropertyBasePtr mPort;
	StringPropertyBasePtr mDialects;
	StringPropertyBasePtr mRole;

	StringPropertyBasePtr createDialectOption();
	StringPropertyBasePtr createIpOption();
	DoublePropertyBasePtr createPortOption();
	StringPropertyBasePtr createRoleOption();

	OpenIGTLinkClientPtr mClient;
	QThreadPtr mThread;
	QDomElement mOptionsElement;
};


} //namespace cx

#endif // CXOPENIGTLINKCLIENT_H