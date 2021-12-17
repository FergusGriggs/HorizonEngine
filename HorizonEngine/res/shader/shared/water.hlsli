
#ifndef __WATER_HEADER_HLSL__
#define __WATER_HEADER_HLSL__

#include "standard.hlsli"

struct VSPS_TRANSFER_WATER
{
	float4 pos : SV_POSITION;
	float3 baseWorldPos : BASE_WORLD_POSIITION;
};

cbuffer WaterCB : register(b1)
{
    int    cb_waveCount;
    float  cb_waveScale;
    float  cb_wavePeriod;
    float  cb_waveSpeed;

    float  cb_waveSeed;
    float  cb_waveScaleMultiplier;
    float  cb_foamStart;
    float  cb_colourChangeStart;

    int    cb_iscolateWaveNum;
    float3 cb_padding;
};

static float windDir = float3(1.0f, 0.0f, 0.0f);

static const float3 seaBaseColour = float3(0.0f, 0.09f, 0.18f);
static const float3 seaWaterColour = float3(0.0f, 0.45f, 0.65f) * 0.7f;
static const float sampleOffset = 0.025f;

static const float pi = 3.141592f;


float3 getFourierOffset(float3 position)
{
    float3 flatPosition = float3(position.x, 0.0f, position.z);
    float3 finalOffset = float3(0.0f, 0.0f, 0.0f);

    float scale = 0.5f;

    int waveNum = 0;

    [unroll(50)]
    while (waveNum < cb_waveCount)
    {
        if (cb_iscolateWaveNum == -1 || cb_iscolateWaveNum == waveNum)
        {
            float waveAngle = (float) waveNum * cb_waveSeed; // hash11((float)waveNum * waveSeed)* waveSeed;
            float3 waveDir = float3(cos(waveAngle), 0.0f, sin(waveAngle));
            //float3 waveDirRight = float3(waveDir.z, 0.0f, -waveDir.x);

            float windScaleModifier = dot(waveDir, windDir) * 0.35f + 0.7f;

            float initialWaveDist = dot(flatPosition, waveDir);
            float distWaveTravelled = cb_gameTime * cb_waveSpeed * ((float) waveNum * 1.0f + 1.0f) * scale + initialWaveDist;

            float angle = distWaveTravelled / (cb_wavePeriod * scale * pow(1.1f, (float) waveNum - 1.0f));

            //float signedDistanceToWaveCentre = dot(waveDirRight, flatPosition);
			float waveBreakScaleMod = 1.0f; // sin(signedDistanceToWaveCentre * 0.05f + waveAngle * 1024.0f + cb_gameTime * waveSpeed * 0.06f + initialWaveDist * 0.2f) * 0.15f + 0.85f;

            float xOffset = cos(waveAngle) * cos(angle) * cb_waveScale * scale * waveBreakScaleMod * windScaleModifier;
            float yOffset = sin(angle) * cb_waveScale * scale * waveBreakScaleMod * windScaleModifier;
            float zOffset = sin(waveAngle) * cos(angle) * cb_waveScale * scale * waveBreakScaleMod * windScaleModifier;

            finalOffset += float3(xOffset, yOffset, zOffset);
        }

        scale *= cb_waveScaleMultiplier;

        waveNum++;
    }

    //finalOffset += float3(0.0f, noiseTexture.Sample(samplerState, (position + float3(0.0f, 0.5f, 0.0f)) * 0.04f) * waveScale * 0.05f, 0.0f);
    //finalOffset += float3(0.0f, noiseTexture.Sample(samplerState, (position + finalOffset) * 0.1f) * 0.25f, 0.0f);

    //return float3(noiseTexture.Sample(samplerState, (position + finalOffset) * 0.1f), 0.0f, 0.0f);

    return finalOffset;
}

#endif // __WATER_HEADER_HLSL__
