#include "Vibe_M.h"  

namespace cv {
	namespace gpu {
		namespace device
		{
			namespace vibe_m
			{
				void loadConstants(int nbSamples, int reqMatches, int radius, int subsamplingFactor);

				void init_gpu(PtrStepSzb frame, int cn, PtrStepSzb samples, PtrStepSz<unsigned int> randStates, cudaStream_t stream);

				void update_gpu(PtrStepSzb frame, int cn, PtrStepSzb fgmask, PtrStepSzb samples, PtrStepSz<unsigned int> randStates, cudaStream_t stream);
			}
		}
	}
}

namespace
{
	const int defaultNbSamples = 20;
	const int defaultReqMatches = 2;
	const int defaultRadius = 20;
	const int defaultSubsamplingFactor = 16;
}

Vibe_M::Vibe_M(unsigned long rngSeed) :
frameSize_(0, 0), rngSeed_(rngSeed)
{
	nbSamples = defaultNbSamples;
	reqMatches = defaultReqMatches;
	radius = defaultRadius;
	subsamplingFactor = defaultSubsamplingFactor;
}

void Vibe_M::initialize(const GpuMat& firstFrame, Stream& s)
{
	using namespace cv::gpu::device::vibe_m;

	CV_Assert(firstFrame.type() == CV_8UC1 || firstFrame.type() == CV_8UC3 || firstFrame.type() == CV_8UC4);

	//cudaStream_t stream = StreamAccessor::getStream(s);  

	loadConstants(nbSamples, reqMatches, radius, subsamplingFactor);

	frameSize_ = firstFrame.size();

	if (randStates_.size() != frameSize_)
	{
		cv::RNG rng(rngSeed_);
		cv::Mat h_randStates(frameSize_, CV_8UC4);
		rng.fill(h_randStates, cv::RNG::UNIFORM, 0, 255);
		randStates_.upload(h_randStates);
	}

	int ch = firstFrame.channels();
	int sample_ch = ch == 1 ? 1 : 4;

	samples_.create(nbSamples * frameSize_.height, frameSize_.width, CV_8UC(sample_ch));

	init_gpu(firstFrame, ch, samples_, randStates_, 0);
}

void Vibe_M::operator()(const GpuMat& frame, GpuMat& fgmask, Stream& s)
{
	using namespace cv::gpu::device::vibe_m;

	CV_Assert(frame.depth() == CV_8U);

	int ch = frame.channels();
	int sample_ch = ch == 1 ? 1 : 4;

	if (frame.size() != frameSize_ || sample_ch != samples_.channels())
		initialize(frame);

	fgmask.create(frameSize_, CV_8UC1);

	update_gpu(frame, ch, fgmask, samples_, randStates_, StreamAccessor::getStream(s));
}

void Vibe_M::release()
{
	frameSize_ = Size(0, 0);

	randStates_.release();

	samples_.release();
}