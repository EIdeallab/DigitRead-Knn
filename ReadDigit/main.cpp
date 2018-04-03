#include <iostream>
#include <cstdio>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

//�̹����� 64*64�� ������ִ� �����Լ�
IplImage* filter(IplImage* src) 
{
	CvSize size = cvGetSize(src);
	CvScalar s;
	int x_max = 0, y_max = 0;
	int x_min = size.width, y_min = size.height;
	for (int y = 0; y < size.height; y++) {
		for (int x = 0; x < size.width; x++) {
			s = cvGet2D(src, y, x);
			if (s.val[0] == 0 && s.val[1] == 0 && s.val[2] == 0) {
				if (x > x_max) x_max = x;
				if (x < x_min) x_min = x;
				if (y > y_max) y_max = y;
				if (y < y_min) y_min = y;
			}
		}
	}
	//�������� �������� �̹��� �ڸ���
	int r_width = x_max - x_min;
	int r_height = y_max - y_min;
	IplImage *res = cvCreateImage(cvSize(r_width + 1, r_height + 1), 8, 3);
	for (int y = y_min; y < y_max + 1; y++) {
		for (int x = x_min; x < x_max + 1; x++) {
			s = cvGet2D(src, y, x);
			cvSet2D(res, y - y_min, x - x_min, s);
		}
	}
	//�ڸ� �̹����� �ٽ� 64x64���� �缳��
	IplImage *dst = cvCreateImage(cvSize(32, 32), res->depth, res->nChannels);
	cvResize(res, dst);
	cvReleaseImage(&res);
	return dst;
}

//ǥ���� �����ϴ� �Լ�
void saveSample(int* sample, IplImage* img) 
{
	CvScalar s;
	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 32; x++) {
			s = cvGet2D(img, y, x);
			sample[y * 32 + x] = (s.val[0] + s.val[1] + s.val[2]) / 256;
			cout << sample[y * 32 + x] << " ";
		}
		cout << endl;
	}
	cout << endl;
}

//��Ŭ���� �Ÿ� ����
int findDist(int *trSample, int *sample)
{
	int dist = 0;
	for (int i = 0; i < 32 * 32; i++)
	{
		dist += (sample[i] - trSample[i]) * (sample[i] - trSample[i]);
	}
	return dist;
}

int main()
{
	//���� �޾ƿ� �����¿� ���� ã��

	int **sampleSpace = new int*[30];
	for (int i = 0; i < 30; i++)
		sampleSpace[i] = new int[32*32];

	//10���� ���ڿ� ���� 3���� ��Ʈ�� ����
	for(int i = 0; i < 10; i++)
		for (int j = 0; j < 3; j++)
		{
			IplImage *src;
			char filename[20] = "3fonts\\0\\0.png";
			filename[7] = i + '0';
			filename[9] = j + '0';
			cout << filename << endl;
			src = cvLoadImage(filename);
			
			//�̹��� ������¡
			IplImage *dst = filter(src);
			cvShowImage("dst", dst);
			saveSample(sampleSpace[i*3+j], dst);
			cout << "A sample has been saved.." << endl;
			
			cvReleaseImage(&src);
			cvReleaseImage(&dst);
			cvWaitKey();
		}

	cout << "�˻��� �̹��� ��� �Է�" << endl;
	char filename[30];
	cin >> filename;
	
	IplImage *tst = cvLoadImage(filename);
	//�Էµ� �����͸� ���͸�
	tst = filter(tst);

	//�Էµ� �׽�Ʈ �����͸� ���ϱ� ���� ��ȯ
	int *sample = new int[32*32];
	saveSample(sample, tst);

	
	//������ ǥ�������Ϳ� �Ÿ��� ����
	int rank[10];
	for (int i = 0; i < 10; i++)
	{
		int numTotal = 0;
		for (int j = 0; j < 3; j++)
		{
			numTotal += findDist(sampleSpace[i*3+j], sample);	
		}
		rank[i] = numTotal;
	}

	//�Ÿ��� �ּ��� �� ã��
	int min = 99999999;
	int minIndex;
	for (int i = 0; i < 10; i++)
	{
		if (min > rank[i])
		{
			min = rank[i];
			minIndex = i;
		}
	}

	//��º�
	cout << "�ν��� ���ڴ� " << minIndex << " �Դϴ�." << endl;
	cout << endl;

	cvReleaseImage(&tst);
	for (int i = 0; i < 30; i++)
		delete[] sampleSpace[i];
	delete[] sampleSpace;
	delete[] sample;
	return 0;
}