#ifndef CAMDIGITS_H
#define CAMDIGITS_H

#include <QtWidgets/QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTimer>

#include <opencv.hpp>
#include "opencv2\core\core.hpp"
#include "opencv2\highgui.hpp"
#include "opencv2\imgproc.hpp"
#include "qlabel.h"

#include "OCR.h"
#include "ui_camdigits.h"

class CamDigits : public QMainWindow
{
	Q_OBJECT

public:
	CamDigits(QWidget *parent = 0);
	~CamDigits();
	 
	void conn();
	QImage cvMat2Image(cv::Mat);
	QImage cvMat2Img(cv::Mat);
	void paintEvent(QPaintEvent * event);

private slots:
	void Read_Data();
	void ctrlCamera();
	void doFrame();

private:
	Ui::CamDigitsClass ui;

	QSerialPort *_port;
	QByteArray serialBuffer;
	QImage *ImgOut;
	QImage *ImgOut2;
	QLabel *showImg;
	QLabel *showImg2;
	QTimer *time;
	QLabel *head;
	QLabel *FrameID;
	QLabel *key;
	QString Info;

	QString portName = "COM5";

private:
	Uav2Img_INFO uavInfo;
	int frameHeight;
	int frameWidth;
	cv::VideoCapture vCapture;
	Turn *entity;


};

#endif // CAMDIGITS_H
