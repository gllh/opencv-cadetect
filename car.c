///�˶������⡪��֡�  
#include"opencv2/opencv.hpp"  
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/core/core.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace cv;
#include <iostream>  
using namespace std;
//�˶������⺯������  
Mat MoveDetect(Mat temp, Mat frame);

int main()
{
	//��ȡ֡��ƽ����֡��򱳾����ֵ�������͡���ʴ��

	VideoCapture video("2.avi");
	if (!video.isOpened())
		return -1;
	while (1)
	{
		int frameCount = video.get(CV_CAP_PROP_FRAME_COUNT);//��ȡ֡��  
		double FPS = video.get(CV_CAP_PROP_FPS);//��ȡFPS  
		Mat frame;//�洢֡  
		Mat temp;//�洢ǰһ֡ͼ��  
		Mat result;//�洢���ͼ��  
		for (int i = 0; i < frameCount; i++)
		{
			//��ȡ֡
			video >> frame;//��֡��frame  
			imshow("frame", frame);
			if (frame.empty())//��֡�����쳣���  
			{
				cout << "frame is empty!" << endl;
				break;
			}

			//����֡
			if (i == 0)//���Ϊ��һ֡��temp��Ϊ�գ�  
			{
				result = MoveDetect(frame, frame);//����MoveDetect()�����˶������⣬����ֵ����result  
			}
			else//�����ǵ�һ֡��temp��ֵ�ˣ�  
			{
				result = MoveDetect(temp, frame);//����MoveDetect()�����˶������⣬����ֵ����result  

			}

			imshow("result", result);
			if (waitKey(1000.0 / FPS) == 27)//��ԭFPS��ʾ  
			{
				cout << "ESC�˳�!" << endl;
				break;
			}
			temp = frame.clone();
		}
	}
	return 0;

}
Mat MoveDetect(Mat temp, Mat frame)
{

	//ƽ����֡��򱳾����ֵ�������͡���ʴ��
	Mat result = frame.clone();
	//1.ƽ������
	IplImage *tempimg_src, *tempimg_dst;
	tempimg_src = &IplImage(temp);
	tempimg_dst = cvCreateImage(cvGetSize(tempimg_src), 8, 3);
	cvSmooth(tempimg_src, tempimg_dst, CV_BLUR, 3, 3, 0, 0);//ƽ������

	IplImage *frameimg_src, *frameimg_dst;
	frameimg_src = &IplImage(frame);
	frameimg_dst = cvCreateImage(cvGetSize(frameimg_src), 8, 3);
	cvSmooth(frameimg_src, frameimg_dst, CV_BLUR, 3, 3, 0, 0);//ƽ������

	//2.֡��
	Mat temp1, frame1;
	temp1 = cvarrToMat(tempimg_dst);
	frame1 = cvarrToMat(frameimg_dst);
	//2.1��background��frameתΪ�Ҷ�ͼ  
	Mat gray1, gray2;
	cvtColor(temp1, gray1, CV_BGR2GRAY);
	cvtColor(frame1, gray2, CV_BGR2GRAY);
	//2.2.��background��frame����  
	Mat diff;
	absdiff(gray1, gray2, diff);
	imshow("֡��ͼ", diff);


	//3.�Բ�ֵͼdiff_thresh������ֵ������  ��ֵ��
	Mat diff_thresh;

	/*
	getStructuringElement() 
	��������ĵ�һ��������ʾ�ں˵���״����������״����ѡ��
	���Σ�MORPH_RECT;
	�����Σ�MORPH_CORSS;
	��Բ�Σ�MORPH_ELLIPSE;
	�ڶ��͵����������ֱ����ں˵ĳߴ��Լ�ê���λ��
	*/
	Mat kernel_erode = getStructuringElement(MORPH_RECT, Size(3, 3));//�����᷵��ָ����״�ͳߴ�ĽṹԪ�ء�
	//����֮�󣬵��������븯ʴ������ʱ�򣬵���������ֵ������getStructuringElement����ֵ��Mat���ͱ�����Ҳ����element������
	Mat kernel_dilate = getStructuringElement(MORPH_RECT, Size(18, 18));

	//���ж�ֵ������ѡ��50��255Ϊ��ֵ
	threshold(diff, diff_thresh, 50, 255, CV_THRESH_BINARY);
	imshow("��ֵ�������", diff_thresh);
	//4.����  
	dilate(diff_thresh, diff_thresh, kernel_dilate);
	imshow("���ʹ����", diff_thresh);
	//5.��ʴ  
	erode(diff_thresh, diff_thresh, kernel_erode);
	imshow("��ʴ�����", diff_thresh);
	
	//6.������������������  
	vector<vector<Point> > contours;
	findContours(diff_thresh, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//����������
	drawContours(result, contours, -1, Scalar(0, 0, 255), 2);//��result�ϻ�������  
	//7.��������Ӿ���  
	vector<Rect> boundRect(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		boundRect[i] = boundingRect(contours[i]);
		rectangle(result, boundRect[i], Scalar(0, 255, 0), 2);//��result�ϻ�������Ӿ���  
	}
	return result;//����result  
}