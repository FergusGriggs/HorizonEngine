
#ifndef __CLOUDS_HEADER_HLSL__
#define __CLOUDS_HEADER_HLSL__

cbuffer CloudsCB : register(b1)
{
	float  cb_cloudCoverage;
	float  cb_lightAbsorbtionThroughClouds;
	float  cb_lightAbsorbtionTowardsSun;
	float  cb_phaseFactor;
	
	float  cb_darknessThreshold;
	float  cb_cloudSpeed;
	float  cb_cloudHeight;
	int    cb_numSteps;
	
	float  cb_stepSize;
	float3 cb_cloudsPadding1;
};

#endif //__CLOUDS_HEADER_HLSL__
