#include"opencv2/highgui/highgui.hpp"
#include"opencv2/core/core.hpp"
#include"opencv2/imgproc/imgproc.hpp"
#include<iostream>
#include<vector>
using namespace cv;
using namespace std;

int main() {
	VideoCapture cap(1);
	int lowc = 50;
	int highc = 150;
	
	int lowh = 156;
	int lows = 80;
	int highh = 180;

	namedWindow("control");
	//createTrackbar("lowc", "control", &lowc, 500);
	//createTrackbar("highc", "control", &highc, 550);

	createTrackbar("lowh", "control", &lowh, 180);
	createTrackbar("highh", "control", &highh, 180);
	createTrackbar("lows", "control", &lows, 256);
	while (1) {
		Mat img;
		cap.read(img);
		Mat can1;
		//Canny(img, can1, lowc, highc);
		//imshow("canny1", can1);
		Mat hsv;
		cvtColor(img, hsv, CV_RGB2HSV);
		vector<Mat> hsvs;
		split(hsv, hsvs);
		for (int i = 1; i < img.rows; i++)
			for (int j = 1; j < img.cols; j++) {
				if (hsvs[0].at<uchar>(i, j) < lowh || hsvs[0].at<uchar>(i, j) > highh  || hsvs[1].at<uchar>(i,j)<lows) {
					img.at<Vec3b>(i, j)[0] = 255;
					img.at<Vec3b>(i, j)[1] = 255;
					img.at<Vec3b>(i, j)[2] = 255;
				}
			}
		Mat bin;
		Mat can;
		/*cvtColor(img, bin, CV_RGB2GRAY);
		Canny(bin, can, lowc, highc);*/
/*		for (int i = 1; i < can.rows; i++) {				//»­ÖÐÏß
			int a1 = 0, a2 = 0;
			int mid;
			for (int j = 1; j < can.cols; j++) {
				if (can.at<uchar>(i, j) != 0) {
					if (a1 == 0)a1 = j;
					else {
						if (j - a1 < 10)continue;
						else {
							a2 = j;
							mid = (a1 + a2) / 2;
							a1 = 0;
							can.at<uchar>(i, mid) = 255;
							mid = mid - can.cols / 2;

						}
					}
				}
			}
		}*/
		imshow("result", img);
		//imshow("canny", can);
		waitKey(10);
	}
	system("pause");
}