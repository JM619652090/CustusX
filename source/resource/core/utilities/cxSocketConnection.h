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

#ifndef CXSOCKETCONNECTION_H
#define CXSOCKETCONNECTION_H

#include <QPointer>
#include <QTcpServer>
#include <QMutex>
#include "cxEnumConverter.h"
#include "boost/shared_ptr.hpp"

namespace cx {
enum CX_SOCKETCONNECTION_STATE
{
    scsINACTIVE,
    scsCONNECTED,
    scsLISTENING,
    scsCONNECTING,
    scsCOUNT
};
} // namespace cx

SNW_DECLARE_ENUM_STRING_CONVERTERS(cxResource_EXPORT, cx, CX_SOCKETCONNECTION_STATE);

/** Insert into every method that is only allowed to be called inside
  * the object's own thread.
  */
#define assertRunningInObjectThread() \
{ \
    if (QThread::currentThread()!=this->thread()) \
    { \
        CX_LOG_ERROR() \
            << QString("Method should be called in the object's thread [%1] only, current thread = [%2]") \
            .arg(this->thread()->objectName()) \
            .arg(QThread::currentThread()->objectName()); \
    }		\
} \



namespace cx {

typedef boost::shared_ptr<class SocketConnector> SocketConnectorPtr;

class cxResource_EXPORT SocketConnection : public QObject
{
    Q_OBJECT
public:
    explicit SocketConnection(QObject *parent = 0);

    struct cxResource_EXPORT ConnectionInfo
    {
        QString role;
        QString protocol;
        QString host;
        int port;

        bool operator==(const ConnectionInfo& rhs) const;
        bool isServer() const;
        bool isClient() const;
        bool isLocalhostConnection() const;
        QString getDescription() const;
    };

    /**
     * Return the connection info for the *next* connection.
     * Any existing connection might have other values.
     */
    ConnectionInfo getConnectionInfo(); ///< thread-safe
    CX_SOCKETCONNECTION_STATE getState(); ///< thread-safe
    virtual void setConnectionInfo(ConnectionInfo info); ///< thread-safe

    virtual void requestConnect(); ///< not thread-safe: use invoke
    virtual void requestDisconnect(); ///< not thread-safe: use invoke

public slots:
    bool sendData(const char* data, qint64 maxSize); ///< not thread-safe

signals:
    void connectionInfoChanged();
    void stateChanged(CX_SOCKETCONNECTION_STATE status);
    void connected();
    void disconnected();
    void error();

private slots:
    void internalConnected();
    void internalDisconnected();
    void internalError(QAbstractSocket::SocketError socketError);
    virtual void internalDataAvailable() = 0;

protected:
    virtual void setProtocol(QString protocolname) = 0;
    SocketConnectorPtr createConnector(ConnectionInfo info);
    bool socketIsConnected();
    bool enoughBytesAvailableOnSocket(int bytes) const;
    bool socketReceive(void *packPointer, int packSize) const;
    QStringList getAllServerHostnames();
    void setCurrentConnectionInfo();
    void stateChange(CX_SOCKETCONNECTION_STATE newState);
    CX_SOCKETCONNECTION_STATE computeState();

    QTcpSocket* mSocket;

    CX_SOCKETCONNECTION_STATE mCurrentState;
    QMutex mNextConnectionInfoMutex;
    ConnectionInfo mNextConnectionInfo; ///< info to be used for the next connect(), mutexed.

protected:
    SocketConnectorPtr mConnector;
};

} //namespace cx

#endif // CXSOCKETCONNECTION_H
