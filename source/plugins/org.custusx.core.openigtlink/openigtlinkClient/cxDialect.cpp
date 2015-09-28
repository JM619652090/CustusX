#include "cxDialect.h"

#include "cxIGTLinkConversion.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxIGTLinkConversionImage.h"
#include "cxIGTLinkConversionBase.h"
#include "cxIGTLinkConversionPolyData.h"

namespace cx
{

Dialect::Dialect(QObject *parent) :
    QObject(parent)
{
    qRegisterMetaType<Transform3D>("Transform3D");
    qRegisterMetaType<ImagePtr>("ImagePtr");
    qRegisterMetaType<ProbeDefinitionPtr>("ProbeDefinitionPtr");
    qRegisterMetaType<IGTLinkUSStatusMessage::Pointer>("IGTLinkUSStatusMessage::Pointer");
    qRegisterMetaType<IGTLinkImageMessage::Pointer>("IGTLinkImageMessage::Pointer");
}

QString Dialect::getName() const
{
    return "Basic";
}

bool Dialect::doCRC() const
{
    return true;
}

void Dialect::translate(const igtl::TransformMessage::Pointer body)
{
    QString deviceName = body->GetDeviceName();

    IGTLinkConversion converter;
    Transform3D prMs = converter.decode(body);

	IGTLinkConversionBase baseConverter;
	double timestamp_ms = baseConverter.decode_timestamp(body).toMSecsSinceEpoch();

    emit transform(deviceName, prMs, timestamp_ms);
}

void Dialect::translate(const igtl::PolyDataMessage::Pointer body)
{
	this->writeAcceptingMessage(body);

	IGTLinkConversionPolyData polyConverter;
	MeshPtr retval = polyConverter.decode(body, this->coordinateSystem());
	emit mesh(retval);
}

void Dialect::translate(const igtl::ImageMessage::Pointer body)
{
	this->writeAcceptingMessage(body);

	IGTLinkConversionImage imageConverter;
	ImagePtr retval = imageConverter.decode(body);
	emit image(retval);
}

void Dialect::translate(const igtl::StatusMessage::Pointer body)
{
	IGTLinkConversion converter;
    QString status = converter.decode(body);
	CX_LOG_CHANNEL_VOLATILE(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL status: " << status;
}

void Dialect::translate(igtl::StringMessage::Pointer body)
{
	IGTLinkConversion converter;
    QString string = converter.decode(body);
    //This was spamming the console
	CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << "IGTL string: " << string;
}

void Dialect::writeNotSupportedMessage(igtl::MessageBase* body)
{
	QString dtype(body->GetDeviceType());
	QString dname(body->GetDeviceName());
	CX_LOG_CHANNEL_INFO(CX_OPENIGTLINK_CHANNEL_NAME) << QString("Ignoring incoming igtlink message (%1, %2): ")
														.arg(dtype)
														.arg(dname);
}

void Dialect::writeAcceptingMessage(igtl::MessageBase* body)
{
	QString dtype(body->GetDeviceType());
	QString dname(body->GetDeviceName());
	CX_LOG_CHANNEL_DEBUG(CX_OPENIGTLINK_CHANNEL_NAME) << QString("Accepting incoming igtlink message (%1, %2): ")
														.arg(dtype)
														.arg(dname);
}

void Dialect::translate(const IGTLinkUSStatusMessage::Pointer body)
{
	this->writeNotSupportedMessage(body);
}

void Dialect::translate(const IGTLinkImageMessage::Pointer body)
{
	this->writeNotSupportedMessage(body);
}

} //namespace cx