#ifndef CXTRANSFERFUNCTIONWIDGET_H_
#define CXTRANSFERFUNCTIONWIDGET_H_

#include "cxBaseWidget.h"
#include <QCheckBox>
#include <QDomElement>
#include "cxImage.h"
#include "cxTransferFunctions3DPresets.h"
#include "cxDoubleDataAdapter.h"
#include "cxActiveImageProxy.h"

class QVBoxLayout;
class QComboBox;
class QStringList;
class QAction;
class QActionGroup;

namespace cx
{
class TransferFunctionAlphaWidget;
class TransferFunctionColorWidget;

/**
 * \file
 * \addtogroup cx_gui
 * @{
 */

/** Superclass for all doubles interacting with a ImageTFData.
 */
class DoubleDataAdapterImageTFDataBase : public DoubleDataAdapter
{
  Q_OBJECT
public:
  DoubleDataAdapterImageTFDataBase();
  virtual ~DoubleDataAdapterImageTFDataBase() {}
  virtual double getValue() const;
  virtual bool setValue(double val);
  virtual void connectValueSignals(bool on) {}
public:
  void setImageTFData(ImageTFDataPtr tfData, ImagePtr image);
protected:
  virtual double getValueInternal() const = 0;
  virtual void setValueInternal(double val) = 0;

  ImageTFDataPtr mImageTFData;
  ImagePtr mImage;
};
typedef boost::shared_ptr<DoubleDataAdapterImageTFDataBase> DoubleDataAdapterImageTFDataBasePtr;


/**DataInterface implementation for the tf window value
 */
class DoubleDataAdapterImageTFDataWindow : public DoubleDataAdapterImageTFDataBase
{
public:
  virtual ~DoubleDataAdapterImageTFDataWindow() {}
  virtual QString getDisplayName() const { return "Window width"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf level value
 */
class DoubleDataAdapterImageTFDataLevel : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataLevel() {}
  virtual QString getDisplayName() const { return "Window level"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf llr value
 */
class DoubleDataAdapterImageTFDataLLR : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataLLR() {}
  virtual QString getDisplayName() const { return "LLR"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

/**DataInterface implementation for the tf alpha value
 */
class DoubleDataAdapterImageTFDataAlpha : public DoubleDataAdapterImageTFDataBase
{
  Q_OBJECT
public:
  virtual ~DoubleDataAdapterImageTFDataAlpha() {}
  virtual QString getDisplayName() const { return "Alpha"; }
  virtual double getValueInternal() const;
  virtual void setValueInternal(double val);
  virtual DoubleRange getValueRange() const;
};

class TransferFunction3DWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction3DWidget(QWidget* parent);
  virtual ~TransferFunction3DWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
//  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;

  ActiveImageProxyPtr mActiveImageProxy;
};

class TransferFunction2DWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunction2DWidget(QWidget* parent);
  virtual ~TransferFunction2DWidget() {}
  virtual QString defaultWhatsThis() const;

public slots:
  void activeImageChangedSlot();

protected:
  QVBoxLayout* mLayout;
  TransferFunctionAlphaWidget* mTransferFunctionAlphaWidget;
  TransferFunctionColorWidget* mTransferFunctionColorWidget;
  DoubleDataAdapterImageTFDataBasePtr mDataWindow, mDataAlpha, mDataLLR, mDataLevel;

  ActiveImageProxyPtr mActiveImageProxy;
};


/**
 * \class TransferFunctionWidget
 *
 * \brief
 *
 * \date Mar 23, 2009
 * \\author Janne Beate Bakeng, SINTEF
 * \\author Ole Vegard Solberg, SINTEF
 */
class TransferFunctionWidget : public BaseWidget
{
  Q_OBJECT

public:
  TransferFunctionWidget(QWidget* parent);
  virtual ~TransferFunctionWidget() {}
  virtual QString defaultWhatsThis() const;
};

/**
 * @}
 */
}

#endif /* CXTRANSFERFUNCTIONWIDGET_H_ */
