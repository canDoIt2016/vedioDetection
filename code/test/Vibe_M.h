#pragma once   
#include <vector>  
#include <memory>  
#include <iosfwd>  
#include "opencv2/core/core.hpp"  
#include "opencv2/core/gpumat.hpp"  
#include "opencv2/gpu/gpu.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include "opencv2/objdetect/objdetect.hpp"  
#include "opencv2/features2d/features2d.hpp"  
using namespace std;
using namespace cv;
using namespace cv::gpu;

class  Vibe_M
{
public:
	//! the default constructor  
	explicit Vibe_M(unsigned long rngSeed = 1234567);
	//! re-initiaization method  
	void initialize(const GpuMat& firstFrame, Stream& stream = Stream::Null());
	//! the update operator  
	void operator()(const GpuMat& frame, GpuMat& fgmask, Stream& stream = Stream::Null());
	//! releases all inner buffers  
	void release();
	int nbSamples;         // number of samples per pixel  
	int reqMatches;        // #_min  
	int radius;            // R  
	int subsamplingFactor; // amount of random subsampling  

private:
	Size frameSize_;
	unsigned long rngSeed_;
	GpuMat randStates_;
	GpuMat samples_;
};