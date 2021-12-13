
#ifndef __ATMOSPHERIC_HEADER_HLSL__
#define __ATMOSPHERIC_HEADER_HLSL__

cbuffer AtmosphericCB : register(b1)
{
	float3 cb_sunDirection;
	float  cb_sunSize;

	float  cb_density;
	float  cb_multiScatterPhase;
	float  cb_anisotropicIntensity;
	float  cb_zenithOffset;

	float  cb_nightDensity;
	float  cb_nightZenithYClamp;
	float2 cb_atmosphericPadding1;
};

#endif //__ATMOSPHERIC_HEADER_HLSL__
