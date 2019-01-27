#include "OCR.h"

Mat org;
Mat img;
int flag = 0;
vector<Mat>number;//存储拆分物体
vector<Point2f>centers;//存储物体圆心
vector<vector<Point2f>>points;//存储物体边点

void unit(int a, int bin[]) {
	if (a == 0) {
		for (int i = 0; i < 16; i++) { bin[i] = 0; }
		return;
	}
	int j = 0;
	int temp = a;
	if (temp<0)temp = 65536 + temp;
	while (temp) {
		bin[j++] = temp % 2;
		temp /= 2;
	}
	for (int i = j; i<16; i++)bin[i] = 0;
	//	cout << "dec:" << a << " bin:";
	//	for (int i = 15; i >= 0; i--)cout << bin[i];
	//	cout << endl;
}
void unitsum(int bin1[], int bin2[], int bin3[]) {
	for (int i = 0; i < 16; i++) {
		if (bin1[i] == bin2[i])bin3[i] = 0;
		else bin3[i] = 1;
	}
	//cout << endl << "add1:";
	//for (int i = 15; i >= 0; i--)cout << bin1[i];
	//cout << " add2:";
	//for (int i = 15; i >= 0; i--)cout << bin2[i];
	//cout << endl << "result:";
	//for (int i = 15; i >= 0; i--)cout << bin3[i];
}
int trandec(int a[]) {
	int m = 1;
	int count = 0;
	for (int i = 0; i < 7; i++) {
		int s = a[i] * m;
		count += s;
		m *= 2;
	}
	return count;
}
void checksum(Digit_INFO&d) {
	int bin1[16], bin2[16], bin3[16], bin4[16];
	unit(d.Head, bin1);
	unit(d.FrameID, bin2);
	unitsum(bin1, bin2, bin4);
	if (bin4[0] == 1)bin4[0] = 0;
	else bin4[0] = 1;

	for (int i = 0; i < 11; i++) {
		unit(d.tagDigit_POINT[i].ImgStatue, bin1);
		unit(d.tagDigit_POINT[i].DigitName, bin2);
		unitsum(bin1, bin2, bin3);
		unit(d.tagDigit_POINT[i].xPiont_mm, bin1);
		unitsum(bin1, bin3, bin2);
		unit(d.tagDigit_POINT[i].yPiont_mm, bin1);
		unitsum(bin1, bin2, bin3);
		unitsum(bin3, bin4, bin4);
	}

	d.tmp1 = trandec(bin4);
}

//判定物体颜色
bool range(int a, int min, int max) {
	if (a >= min && a <= max)return true;
	else return false;
}
int color(Mat num, int x, int&criteria, Mat&img, int mark[]) {
	Mat hsv;
	cvtColor(num, hsv, CV_BGR2HSV);
	//	imshow(to_string(flag), num);
	vector<Mat>hsvs;
	split(hsv, hsvs);
	int c[4] = { 0,0,0,0 };//黄绿蓝红

	int index = 0;
	int max = c[0];
	int flagc = 0;
	int standard = (num.cols*num.rows / 9) * 5;
	for (int i = 0; i < num.rows; i++) {
		for (int j = 0; j < num.cols; j++) {
			if (range(hsvs[0].at<uchar>(i, j), 8, 34) && range(hsvs[1].at<uchar>(i, j), 43, 255)) {
				c[0]++;
				if (c[0] >= standard) {
					index = 0;
					flagc = 1;
					max = c[0];
					break;
				}
			}
			if (range(hsvs[0].at<uchar>(i, j), 35, 100) && range(hsvs[1].at<uchar>(i, j), 43, 255)) {
				c[1]++;
				if (c[1] >= standard) {
					index = 1;
					flagc = 1;
					max = c[1];
					break;
				}
			}
			if (range(hsvs[0].at<uchar>(i, j), 100, 124) && range(hsvs[1].at<uchar>(i, j), 43, 255)) {
				c[2]++;
				if (c[2] >= standard) {
					index = 2;
					flagc = 1;
					max = c[2];
					break;
				}
			}
			if ((range(hsvs[0].at<uchar>(i, j), 0, 10) || range(hsvs[0].at<uchar>(i, j), 156, 180)) && range(hsvs[1].at<uchar>(i, j), 43, 255)) {
				c[3]++;
				if (c[3] >= standard) {
					index = 3;
					flagc = 1;
					max = c[3];
					break;
				}
			}
		}
		if (flagc)break;
	}

	if (!flagc) {
		for (int i = 1; i < 4; i++) {
			if (max < c[i]) {
				max = c[i];
				index = i;
			}
		}
		if (max < num.cols*num.rows / 3) {	//无合标准物体
			//cout << "invilid aera" << endl;
			return -1;
		}
		//criteria = 2;//低质量
	}
	criteria = 1;//高质量 

	if (mark[index])return -1;

	String str2;
	switch (index) {
	case 0:cout << "黄色" << endl; str2 = "Yellow"; break;
	case 1:cout << "绿色" << endl; str2 = "Green"; break;
	case 2:cout << "蓝色" << endl; str2 = "Blue"; break;
	case 3:cout << "红色" << endl; str2 = "Red"; break;
	}

	String str = "(" + to_string((int)(centers[x].x - img.cols / 2)) + "," + to_string((int)(centers[x].y - img.rows / 2)) +")" + str2;
	putText(img, str, centers[x], CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 0, 0), 2);

	for (int i = 0; i < 4; i++)line(img, points[x][i], points[x][(i + 1) % 4], Scalar(0, 0, 255), 2);

	mark[index] = 1;

	return index + 1;
}
//将数据存储至协议格式
Digit_INFO* trans(Mat&img, int mark[]) {
	Digit_INFO *d = new Digit_INFO;
	d->FrameID = flag++;
	unsigned char col;
	int criteria;
	int flagt[5] = { 0,0,0,0,0 };
	for (int i = 0; i < number.size(); i++) {
		criteria = 0;
		col = color(number[i], i, criteria, img, mark);
		if (col != -1) {
			if (!flagt[col]) {	//只标记第一次
				if (criteria) d->tagDigit_POINT[col].ImgStatue = criteria;
				else d->tagDigit_POINT[col].ImgStatue = 1;

				d->tagDigit_POINT[col].DigitName = col;

				d->tagDigit_POINT[col].xPiont_mm = centers[i].x - img.cols / 2;
				d->tagDigit_POINT[col].yPiont_mm = centers[i].y - img.rows / 2;

				flagt[col] = 1;
			}
		}
	}

	for (int j = 0; j < 11; j++) {
		if (j >= 5 || !flagt[j]) {
			d->tagDigit_POINT[j].ImgStatue = 0;
		}
	}
	//checksum(*d);
	return d;
}

//物体分离
void draw(Mat & img, vector<vector<Point>> contours, int i) {
	Point2f v[4];
	Rect mr = boundingRect(Mat(contours[i]));
	if (mr.area() < img.cols*img.rows / 40)return;//去除噪声区域

	v[0] = mr.tl();
	v[1].x = mr.tl().x;
	v[1].y = mr.br().y;
	v[2] = mr.br();
	v[3].x = mr.br().x;
	v[3].y = mr.tl().y;


	Point2f center;
	float r;
	center.x = (mr.tl().x + mr.br().x) / 2;
	center.y = (mr.tl().y + mr.br().y) / 2;
	int hgt, wid;
	hgt = mr.br().y - mr.tl().y;
	wid = mr.br().x - mr.br().x;
	r = (hgt > wid ? hgt : wid) / 2;

	Mat show;//分离各特征物体
	show = img(mr);
	resize(show, show, Size(50, 96));
	number.push_back(show);
	centers.push_back(center);
	vector<Point2f>vx;
	for (int i = 0; i < 4;i++)vx.push_back(v[i]); 
	points.push_back(vx);
	//radius.push_back(r);
}

Turn* imgProcess(Mat& video) {

	Turn * t = new Turn;
	t->d = new Digit_INFO;
	video.copyTo(img);
	number.clear();
	centers.clear();
	points.clear();
	int mark[5] = { 0, 0, 0, 0, 0 };

	int thresh = 0;
	
	Mat blur;
	medianBlur(img, blur, 5);
	Mat img_gray;
	cvtColor(blur, img_gray, CV_BGR2GRAY);

	//Mat thresh_img;
	//threshold(img_gray, thresh_img, 80, 255, THRESH_BINARY_INV);//二值化 方法一

	Mat edge;
	Canny(img_gray, edge, 50, 80);//边缘检测 方法二

	vector<Vec4i>hier;
	vector<vector<Point>> contours;
	findContours(edge, contours, hier, RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

	if (contours.size() > 220)thresh = 1;

	if (!thresh){
		for (int i = 0; i < contours.size(); i++) {
			draw(img, contours, i);
		}
		t->d = trans(video, mark);
	}
	else {
		t->d->FrameID = flag++;
		for (int i = 0; i < 11; i++){
			t->d->tagDigit_POINT[i].ImgStatue = 0;
			
			//checksum(*t->d);
		}
	}
	t->canny = edge;

	return t;
}


/*飞机颜色识别*//*带协议转换*//*校验和修改版*//*已存*/