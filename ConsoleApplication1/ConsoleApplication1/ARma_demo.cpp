#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "cameraparams.h"
#include "patterndetector.h"

using namespace std;
using namespace cv;
using namespace ARma;

#define PAT_SIZE 64;//equal to pattern_size variable (see below)

char* filename1="pattern1.png";//id=1
char* filename2="pattern2.png";//id=2
char* filename3="pattern3.png";//id=3

static int loadPattern(const char* , std::vector<cv::Mat>& , int& );

int main(int argc, char** argv){

	std::vector<cv::Mat> patternLibrary;
	std::vector<Pattern> detectedPattern;
	int patternCount=0;

	/*create patterns' library using rotated versions of patterns 
	*/
	loadPattern(filename1, patternLibrary, patternCount);
	//loadPattern(filename2, patternLibrary, patternCount);
	//loadPattern(filename3, patternLibrary, patternCount);
	cout << patternCount << " patterns are loaded." << endl;


	int norm_pattern_size = PAT_SIZE;
	double fixed_thresh = 40;
	double adapt_thresh = 5;//non-used with FIXED_THRESHOLD mode
	int adapt_block_size = 45;//non-used with FIXED_THRESHOLD mode
	double confidenceThreshold = 0.35;
	int mode = 2;//1:FIXED_THRESHOLD, 2: ADAPTIVE_THRESHOLD

	PatternDetector myDetector( fixed_thresh, adapt_thresh, adapt_block_size, confidenceThreshold, norm_pattern_size, mode);

	CvCapture* capture = cvCaptureFromCAM(0);
	CvVideoWriter *video_writer = cvCreateVideoWriter( "output2.avi", -1, 25, cvSize(640,480) );

	Mat imgMat;
	int k=0;
	while(k<500){
		//mycapture >> imgMat; 
		IplImage* img = cvQueryFrame(capture);
		Mat imgMat = Mat(img);
		
		double tic=(double)cvGetTickCount();

		//run the detector
		myDetector.detect(imgMat, cameraMatrix, distortions, patternLibrary, detectedPattern); 

		double toc=(double)cvGetTickCount();
		double detectionTime = (toc-tic)/((double) cvGetTickFrequency()*1000);
		cout << "Detected Patterns: " << detectedPattern.size() << endl;
		cout << "Detection time: " << detectionTime << endl;

		//augment the input frame (and print out the properties of pattern if you want)
		for (unsigned int i =0; i<detectedPattern.size(); i++){
			//detectedPattern.at(i).showPattern();
			detectedPattern.at(i).draw( imgMat, cameraMatrix, distortions);
		}


		cvWriteFrame(video_writer, &((IplImage) imgMat));

		imshow("result", imgMat);
		cvWaitKey(1);
		k++;

		detectedPattern.clear();
	}

	cvReleaseVideoWriter(&video_writer);
	cvReleaseCapture(&capture);

	return 0;

}

int loadPattern(const char* filename, std::vector<cv::Mat>& library, int& patternCount){
	Mat img = imread(filename,0);
	
	if(img.cols!=img.rows){
		return -1;
		printf("Not a square pattern");
	}

	int msize = PAT_SIZE; 

	cv::Mat src(msize, msize, CV_8UC1);
	Point2f center((msize-1)/2.0f,(msize-1)/2.0f);
	Mat rot_mat(2,3,CV_32F);
	
	cv::resize(img, src, Size(msize,msize));
	Mat subImg = src(cv::Range(msize/4,3*msize/4), cv::Range(msize/4,3*msize/4));
	library.push_back(subImg);

	rot_mat = getRotationMatrix2D( center, 90, 1.0);

	for (int i=1; i<4; i++){
		Mat dst= Mat(msize, msize, CV_8UC1);
		rot_mat = getRotationMatrix2D( center, -i*90, 1.0);
		cv::warpAffine( src, dst , rot_mat, Size(msize,msize));
		Mat subImg = dst(cv::Range(msize/4,3*msize/4), cv::Range(msize/4,3*msize/4));
		library.push_back(subImg);	
	}

	patternCount++;
	return 1;
}

