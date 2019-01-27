#include "camdigits.h"
#include <qmessagebox.h>
#include <QPixmap.h>

CamDigits::CamDigits(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	
	portName = "COM5";
	frameHeight = 480;
	frameWidth = 640;
	connect(ui.OpenBtn, SIGNAL(clicked()), this, SLOT(ctrlCamera()));

	time = new QTimer;
	connect(time, SIGNAL(timeout()), this, SLOT(doFrame()));
	
	ImgOut = new QImage(QSize(frameWidth, frameHeight), QImage::Format_RGB888);

	showImg = new QLabel(ui.canny);
	showImg->setFixedSize(QSize(frameWidth, frameHeight));//图一

	entity = new Turn;

	foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
	{
		QSerialPort serial;
		serial.setPort(info);
		if (serial.open(QIODevice::ReadWrite))
		{
			ui.serial->addItem(serial.portName());
			serial.close();
		}
	}

}

CamDigits::~CamDigits()
{
	delete entity;
	delete showImg;
	delete ImgOut;
}

void CamDigits::ctrlCamera()
{
	if (vCapture.open(0))
	{
		portName = ui.serial->currentText();
		time->start(100);
		_port = new QSerialPort;
		_port->setPortName(QString(portName));
		_port->setBaudRate(QSerialPort::Baud115200);
		_port->setDataBits(QSerialPort::Data8);
		_port->setFlowControl(QSerialPort::NoFlowControl);
		_port->setStopBits(QSerialPort::OneStop);
		_port->setParity(QSerialPort::NoParity);
		_port->clearError();
		_port->clear();

		connect(_port, &QSerialPort::readyRead, this, &CamDigits::Read_Data);
		if (_port->open(QIODevice::ReadWrite) == false)
		{
			QMessageBox::warning(this, "Serial Port Warnings!", "Application has a SerialPort ERROR!\nCheck the following errors: \n1. COM is not listed in the Device Manager.\n2. COM is busy right now.");
		}

	}
}

void CamDigits::doFrame()
{
	Mat Img;
	if (false == vCapture.read(Img))
		return;

	BYTE * ImgData;

	entity = imgProcess(Img);

	unsigned char * pData = (unsigned char *)entity->d;

	for (int cnt = 0; cnt < sizeof(Digit_INFO) - 2; cnt++)
		entity->d->tmp1 += pData[cnt];
	pData[sizeof(Digit_INFO) - 2] = entity->d->tmp1;
	pData[sizeof(Digit_INFO) - 1] = entity->d->tmp2;

	_port->write((char *)entity->d, sizeof(Digit_INFO));

	Info.clear();

	Info += QString("Head\t%1\nFrame\t%2\nKey\t%3\n")
		.arg(QString::number(entity->d->Head, 16))
		.arg(QString::number(int(entity->d->FrameID)))
		.arg(QString::number(int(entity->d->Key)));

	for (int i = 0; i < 11; i++){
		Info += QString("%1 %2 %3 %4 ")
			.arg(QString::number((int)entity->d->tagDigit_POINT[i].ImgStatue))
			.arg(QString::number((int)entity->d->tagDigit_POINT[i].DigitName))
			.arg(QString::number((int)entity->d->tagDigit_POINT[i].xPiont_mm))
			.arg(QString::number((int)entity->d->tagDigit_POINT[i].yPiont_mm));
	}

	Info += QString("\nchecksum\t%1\ntail\t%2")
		.arg(QString::number((int)entity->d->tmp1))
		.arg(QString::number((int)entity->d->tmp2));
	
	*ImgOut = cvMat2Img(Img);

	update();
}

void CamDigits::conn()
{
	_port = new QSerialPort;
	_port->setPortName(QString(portName));
	_port->setBaudRate(QSerialPort::Baud115200);
	_port->setDataBits(QSerialPort::Data8);
	_port->setFlowControl(QSerialPort::NoFlowControl);
	_port->setStopBits(QSerialPort::OneStop);
	_port->setParity(QSerialPort::NoParity);
	_port->clearError();
	_port->clear();

	connect(_port, &QSerialPort::readyRead, this, &CamDigits::Read_Data);
	if (_port->open(QIODevice::ReadWrite) == false)
	{
		QMessageBox::warning(this, "Serial Port Warnings!", "Application has a SerialPort ERROR!\nCheck the following errors: \n1. COM5 is not listed in the Device Manager.\n2. COM5 is busy right now.");
	}
}

void CamDigits::Read_Data()
{
	//读取数据，添加到缓冲区中
	QByteArray arr = _port->readAll();

	//QString str = ui.textEdit->toPlainText();
	//for(int i = 0; i < sizeof Digit_INFO; i++)
	//{
	//	qint8 outChar = arr[i];    //每字节填充一次，直到结束
	//	str += QString("%1").arg(outChar & 0xFF, 2, 16, QLatin1Char('0'));
	//	str += tr(" ");
	//}

	//ui.textEdit->setText(str);
	//从缓冲区中检测到目标头
	int index = 0;
	const int len = sizeof(Uav2Img_INFO);
	while (index + len <= serialBuffer.size())
	{
		if (serialBuffer.at(index) == 0x24 && serialBuffer.at(index + 1) == 0x02 && serialBuffer.at(index + len - 1) == 0x0d)
		{
			//检查校验和
			ui.textEdit->setText("begin");
			unsigned char checksum = 0x00;
			for (int cnt = 0; cnt < len - 2; cnt++)
				checksum += (unsigned char)serialBuffer.at(index + cnt);
			if (checksum == serialBuffer.at(index + len - 2))
			{
				ui.textEdit->setText("ok");
				memcpy(&uavInfo, serialBuffer.data(), sizeof(Uav2Img_INFO));
				serialBuffer.remove(0, index + len);
			}
		}
		index++;
	}

	return;
}

void CamDigits::paintEvent(QPaintEvent * event)
{
	Q_UNUSED(event);
	showImg->setPixmap(QPixmap::fromImage(*ImgOut));
	ui.textEdit->setText(Info);
}

QImage CamDigits::cvMat2Image(cv::Mat src)
{
	cv::Mat temp; // make the same cv::Mat  
	cvtColor(src, temp, CV_BGR2RGB); // cvtColor Makes a copt, that what i need  

	QImage dest((const uchar *)temp.data, temp.cols, temp.rows, temp.step, QImage::Format_RGB888);
	dest.bits(); // enforce deep copy, see documentation  
				 // of QImage::QImage ( const uchar * data, int width, int height, Format format )  
	return dest;
}

QImage CamDigits::cvMat2Img(cv::Mat mat)
{
	// 8-bits unsigned, NO. OF CHANNELS = 1
	if (mat.type() == CV_8UC1)
	{
		QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
		// Set the color table (used to translate colour indexes to qRgb values)
		image.setColorCount(256);
		for (int i = 0; i < 256; i++)
		{
			image.setColor(i, qRgb(i, i, i));
		}
		// Copy input Mat
		uchar *pSrc = mat.data;
		for (int row = 0; row < mat.rows; row++)
		{
			uchar *pDest = image.scanLine(row);
			memcpy(pDest, pSrc, mat.cols);
			pSrc += mat.step;
		}
		return image;
	}
	// 8-bits unsigned, NO. OF CHANNELS = 3
	else if (mat.type() == CV_8UC3)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
		return image.rgbSwapped();
	}
	else if (mat.type() == CV_8UC4)
	{
		// Copy input Mat
		const uchar *pSrc = (const uchar*)mat.data;
		// Create QImage with same dimensions as input Mat
		QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
		return image.copy();
	}
	else
	{
		return QImage();
	}
}