/**
 * @file solvePnP.cpp
 * @brief mex interface for solvePnP
 * @author Kota Yamaguchi
 * @date 2011
 */
#include "mexopencv.hpp"
using namespace std;
using namespace cv;

/// Conversion to vector<Point_<T> >
template <typename T>
vector<Point_<T> > MxArrayToVecPt(MxArray& arr)
{
	vector<MxArray> va = arr.toStdVector<MxArray>();
	vector<Point_<T> > vp;
	vp.reserve(va.size());
	for (vector<MxArray>::iterator it=va.begin(); it<va.end(); ++it)
		vp.push_back((*it).toPoint_<T>());
	return vp;
}

/// Conversion to vector<Point_<T> >
template <typename T>
vector<Point3_<T> > MxArrayToVecPt3(MxArray& arr)
{
	vector<MxArray> va = arr.toStdVector<MxArray>();
	vector<Point3_<T> > vp;
	vp.reserve(va.size());
	for (vector<MxArray>::iterator it=va.begin(); it<va.end(); ++it)
		vp.push_back((*it).toPoint3_<T>());
	return vp;
}


/**
 * Main entry called from Matlab
 * @param nlhs number of left-hand-side arguments
 * @param plhs pointers to mxArrays in the left-hand-side
 * @param nrhs number of right-hand-side arguments
 * @param prhs pointers to mxArrays in the right-hand-side
 */
void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
	// Check the number of arguments
	if (nrhs<3 || ((nrhs%2)!=0) || nlhs>2)
        mexErrMsgIdAndTxt("mexopencv:error","Wrong number of arguments");
    
	// Argument vector
	vector<MxArray> rhs(prhs,prhs+nrhs);
	
	Mat cameraMatrix(rhs[2].toMat(CV_32F));
	Mat distCoeffs((nrhs>3) ? rhs[3].toMat(CV_32F) : Mat());
	
	// Option processing
	Mat rvec, tvec;
	bool useExtrinsicGuess=false;
	for (int i=4; i<nrhs; i+=2) {
		string key = rhs[i].toString();
		if (key=="UseExtrinsicGuess")
			useExtrinsicGuess = rhs[i+1].toBool();
		else if (key=="Rvec")
			rvec = rhs[i+1].toMat(CV_32F);
		else if (key=="Tvec")
			tvec = rhs[i+1].toMat(CV_32F);
		else
			mexErrMsgIdAndTxt("mexopencv:error","Unrecognized option");
	}
	
	// Process
	if (rhs[0].isNumeric() && rhs[1].isNumeric()) {
		Mat objectPoints(rhs[0].toMat(CV_32F)), imagePoints(rhs[1].toMat(CV_32F));
		solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec,
			tvec, useExtrinsicGuess);
	}
#if CV_MINOR_VERSION >= 2
	else if (rhs[0].isCell() && rhs[1].isCell()) {
		vector<Point3f> objectPoints(MxArrayToVecPt3<float>(rhs[0]));
		vector<Point2f> imagePoints(MxArrayToVecPt<float>(rhs[0]));
		solvePnP(objectPoints, imagePoints, cameraMatrix, distCoeffs, rvec,
			tvec, useExtrinsicGuess);
	}
#endif
	else
		mexErrMsgIdAndTxt("mexopencv:error","Invalid argument");

	plhs[0] = MxArray(rvec);
	if (nlhs>1)
		plhs[1] = MxArray(tvec);
}
