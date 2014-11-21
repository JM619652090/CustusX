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

#include "cxIGTLinkedImageReceiverThread.h"

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlPositionMessage.h"
#include "igtlImageMessage.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#include "cxTypeConversions.h"
#include "cxReporter.h"
#include "cxIGTLinkConversion.h"
#include "cxCyclicActionLogger.h"


namespace cx
{

IGTLinkedImageReceiverThread::IGTLinkedImageReceiverThread(QString address, int port, QObject* parent) :
	ImageReceiverThread(parent),
	mHeadingReceived(false),
	mAddress(address),
	mPort(port),
	mSocket(NULL)
{
//  std::cout << "client::create thread: " << QThread::currentThread() << std::endl;
}

void IGTLinkedImageReceiverThread::run()
{
//  std::cout << "client::run thread: " << QThread::currentThread() << std::endl;
	//std::cout << "run client thread, connecting to " << mAddress << " " << mPort << std::endl;

	//------------------------------------------------------------
	// Establish Connection
	mSocket = new QTcpSocket();
	connect(mSocket, SIGNAL(readyRead()), this, SLOT(readyReadSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(hostFound()), this, SLOT(hostFoundSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(connected()), this, SLOT(connectedSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(disconnected()), this, SLOT(disconnectedSlot()), Qt::DirectConnection);
	connect(mSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(errorSlot(QAbstractSocket::SocketError)),
					Qt::DirectConnection);

	report("Looking for host: " + this->hostDescription());
	mSocket->connectToHost(mAddress, mPort);

	int timeout = 5000;
	if (!mSocket->waitForConnected(timeout))
	{
		reportWarning("Timeout looking for host " + this->hostDescription());
		mSocket->disconnectFromHost();
		return;
	}

	// Create a message buffer to receive header
	mHeaderMsg = igtl::MessageHeader::New();

//	mFPSTimer->reset(2000);

	// run event loop
	this->exec();

	mSocket->disconnectFromHost();
//  std::cout << "finished openIGTLink client thread" << std::endl;
	delete mSocket;
}

QString IGTLinkedImageReceiverThread::hostDescription() const
{
	return mAddress + ":" + qstring_cast(mPort);
}

void IGTLinkedImageReceiverThread::hostFoundSlot()
{
	report("Host found: " + this->hostDescription());
}
void IGTLinkedImageReceiverThread::connectedSlot()
{
	reportSuccess("Connected to host " + this->hostDescription());
	emit connected(true);
}
void IGTLinkedImageReceiverThread::disconnectedSlot()
{
	report("Disconnected to host " + this->hostDescription());
	emit connected(false);
}
void IGTLinkedImageReceiverThread::errorSlot(QAbstractSocket::SocketError socketError)
{
	reportError(
					"Socket error [Host=" + this->hostDescription() + ", Code=" + socketError + "]\n"
									+ mSocket->errorString());
}

void IGTLinkedImageReceiverThread::readyReadSlot()
{
	// read messages until one fails
	while (this->readOneMessage());

//	readOneMessage();
}

/**Read one IGTLink message from the socket.
 * Return false if there was not enough data to
 * read the entire message.
 */
bool IGTLinkedImageReceiverThread::readOneMessage()
{

//  std::cout << "tick " << std::endl;

	if (!mHeadingReceived)
	{
//    std::cout << "client::tick: received: " << mSocket->bytesAvailable() << ", head needed: " << mHeaderMsg->GetPackSize() << std::endl;
		// Initialize receive buffer
		mHeaderMsg->InitPack();

		// ignore if not enough data (yet)
		if (mSocket->bytesAvailable() < mHeaderMsg->GetPackSize())
		{
			//std::cout << "Incomplete heading received, ignoring. " << std::endl;
			//std::cout << "available: " << mSocket->bytesAvailable() << ", needed " << mHeaderMsg->GetPackSize() << std::endl;
			return false;
		}

		// after peek: read to increase pos
		mSocket->read(reinterpret_cast<char*>(mHeaderMsg->GetPackPointer()), mHeaderMsg->GetPackSize());
		mHeadingReceived = true;

		// Deserialize the header
		mHeaderMsg->Unpack();
	}

	if (mHeadingReceived)
	{
//    std::cout << "client::tick: received: " << mSocket->bytesAvailable() << ", body needed: " << mHeaderMsg->GetBodySizeToRead() << std::endl;
		bool success = false;
		// Check data type and receive data body
//    if (QString(mHeaderMsg->GetDeviceType()) == "TRANSFORM")
//    {
//      ReceiveTransform(mSocket, mHeaderMsg);
//    }
//    else if (QString(mHeaderMsg->GetDeviceType() == "POSITION")
//    {
//      ReceivePosition(mSocket, mHeaderMsg);
//    }
		if (QString(mHeaderMsg->GetDeviceType()) == "IMAGE")
		{
			success = this->ReceiveImage(mSocket, mHeaderMsg);
		}
		else if (QString(mHeaderMsg->GetDeviceType()) == "CX_US_ST")
		{
			success = this->ReceiveSonixStatus(mSocket, mHeaderMsg);
		}
//    else if (QString(mHeaderMsg->GetDeviceType() == "STATUS")
//    {
//      ReceiveStatus(mSocket, mHeaderMsg);
//    }
		else
		{
			std::cerr << "Receiving : " << mHeaderMsg->GetDeviceType() << std::endl;
			mSocket->read(mHeaderMsg->GetBodySizeToRead());
		}

		if (success)
			mHeadingReceived = false; // restart
		else
			return false;
	}
//  std::cout << "  tock " << std::endl;
	return true;
}

bool IGTLinkedImageReceiverThread::ReceiveSonixStatus(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
{
	IGTLinkUSStatusMessage::Pointer msg;
	msg = IGTLinkUSStatusMessage::New();
	msg->SetMessageHeader(header);
	msg->AllocatePack();

	if (socket->bytesAvailable() < msg->GetPackBodySize())
	{
		//std::cout << "Incomplete body received, ignoring. " << std::endl;
		return false;
	}
	socket->read(reinterpret_cast<char*>(msg->GetPackBodyPointer()), msg->GetPackBodySize());
	// Deserialize the transform data
	// If you want to do a CRC check, call Unpack(1).
	// If you want to skip CRC check, call Unpack() without argument.
	int c = msg->Unpack();
	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
	{
		this->addToQueue(msg);

		return true;
	}

	std::cout << "body crc failed!" << std::endl;
	return true;
}

bool IGTLinkedImageReceiverThread::ReceiveImage(QTcpSocket* socket, igtl::MessageHeader::Pointer& header)
{
	// Create a message buffer to receive transform data
	IGTLinkImageMessage::Pointer imgMsg;
	imgMsg = IGTLinkImageMessage::New();
	imgMsg->SetMessageHeader(header);
	imgMsg->AllocatePack();

	// Receive transform data from the socket
	// ignore if not enough data (yet)
	if (socket->bytesAvailable() < imgMsg->GetPackBodySize())
	{
		//std::cout << "Incomplete body received, ignoring. " << std::endl;
		return false;
	}

	socket->read(reinterpret_cast<char*>(imgMsg->GetPackBodyPointer()), imgMsg->GetPackBodySize());
	// Deserialize the transform data
	// If you want to do a CRC check, call Unpack(1).
	// If you want to skip CRC check, call Unpack() without argument.
	int c = imgMsg->Unpack();

	if (c & (igtl::MessageHeader::UNPACK_BODY | igtl::MessageHeader::UNPACK_UNDEF)) // if CRC check is OK or skipped
	{
		this->addToQueue(imgMsg);
		return true;
	}

	std::cout << "body crc failed!" << std::endl;
	return true;
}

void IGTLinkedImageReceiverThread::addToQueue(IGTLinkUSStatusMessage::Pointer msg)
{
	// set temporary, then assume the image adder will pass this message on.
	mUnsentUSStatusMessage = msg;
}

void IGTLinkedImageReceiverThread::addToQueue(IGTLinkImageMessage::Pointer msg)
{
	IGTLinkConversion converter;
	this->addImageToQueue(converter.decode(msg));

	// if us status not sent, do it here
	if (mUnsentUSStatusMessage)
	{
		this->addSonixStatusToQueue(converter.decode(mUnsentUSStatusMessage, msg, ProbeDefinitionPtr(new ProbeDefinition())));
		mUnsentUSStatusMessage = IGTLinkUSStatusMessage::Pointer();
	}
}

} //end namespace cx
