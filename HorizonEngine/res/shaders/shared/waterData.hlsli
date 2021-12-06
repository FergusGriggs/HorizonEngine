
cbuffer waterCBuffer : register(b0)
{
    int waveCount;
    float waveScale;
    float wavePeriod;
    float waveSpeed;

    float waveSeed;
    float waveScaleMultiplier;
    float foamStart;
    float colourChangeStart;

    int    iscolateWaveNum;
    float3 padding;
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
    while (waveNum < waveCount)
    {
        if (iscolateWaveNum == -1 || iscolateWaveNum == waveNum)
        {
            float waveAngle = (float)waveNum * waveSeed;// hash11((float)waveNum * waveSeed)* waveSeed;
            float3 waveDir = float3(cos(waveAngle), 0.0f, sin(waveAngle));
            //float3 waveDirRight = float3(waveDir.z, 0.0f, -waveDir.x);

            float windScaleModifier = dot(waveDir, windDir) * 0.35f + 0.7f;

            float initialWaveDist = dot(flatPosition, waveDir);
            float distWaveTravelled = gameTime * waveSpeed * ((float)waveNum * 1.0f + 1.0f) * scale + initialWaveDist;

            float angle = distWaveTravelled / (wavePeriod * scale * pow(1.1f, (float)waveNum - 1.0f));

            //float signedDistanceToWaveCentre = dot(waveDirRight, flatPosition);
            float waveBreakScaleMod = 1.0f;// sin(signedDistanceToWaveCentre * 0.05f + waveAngle * 1024.0f + gameTime * waveSpeed * 0.06f + initialWaveDist * 0.2f) * 0.15f + 0.85f;

            float xOffset = cos(waveAngle) * cos(angle) * waveScale * scale * waveBreakScaleMod * windScaleModifier;
            float yOffset = sin(angle) * waveScale * scale * waveBreakScaleMod * windScaleModifier;
            float zOffset = sin(waveAngle) * cos(angle) * waveScale * scale * waveBreakScaleMod * windScaleModifier;

            finalOffset += float3(xOffset, yOffset, zOffset);
        }

        scale *= waveScaleMultiplier;

        waveNum++;
    }

    //finalOffset += float3(0.0f, noiseTexture.Sample(samplerState, (position + float3(0.0f, 0.5f, 0.0f)) * 0.04f) * waveScale * 0.05f, 0.0f);
    //finalOffset += float3(0.0f, noiseTexture.Sample(samplerState, (position + finalOffset) * 0.1f) * 0.25f, 0.0f);

    //return float3(noiseTexture.Sample(samplerState, (position + finalOffset) * 0.1f), 0.0f, 0.0f);

    return finalOffset;
}