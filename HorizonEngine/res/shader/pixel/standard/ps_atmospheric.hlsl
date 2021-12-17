
#include "../../shared/standard.hlsli"
#include "../../shared/scene.hlsli"
#include "../../shared/atmospheric.hlsli"

SamplerState samplerState : SAMPLER : register(s0);

static const float pi = 3.1415926f;
static const float invPi = 1.0f / 3.1415926f;

static const float3 sunColour = float3(1.0f, 1.0f, 1.0f);
static const float3 skyColour = float3(0.39f, 0.57f, 1.0f) * (1.0f + cb_anisotropicIntensity); //Make sure one of the conponents is never 0.0 //0.180f, 0.725f, 1.0f//0.39f, 0.57f, 1.0f//0.06f, 0.625f, 1.0f
static const float3 nightSkyColour = float3(0.1f, 0.15f, 0.25f);// *(1.0f + anisotropicIntensity);

#define smooth(x) x * x * (3.0f - 2.0f * x)
#define dayZenithDensity(yCoord) cb_density / pow(max(yCoord - cb_zenithOffset, 0.0035f), 0.75f)
#define nightZenithDensity(yCoord) cb_nightDensity / pow(max(yCoord + 0.05f, cb_nightZenithYClamp), 0.75f)

float radialDistance(float3 direction1, float3 direction2)
{
	return acos(dot(direction1, direction2));
}

float3 getSkyAbsorption(float3 x, float y)
{
	float3 absorption = x * -y;
	absorption = exp2(absorption) * 2.0f;

	return absorption;
}

float getSunPoint(float3 viewDirection, float3 sunDirection)
{
	return smoothstep(cb_sunSize * 0.0006f, cb_sunSize * 0.00052f, radialDistance(viewDirection, sunDirection)) * 50.0f;
}

float getRayleigMultiplier(float3 viewDirection, float3 sunDirection)
{
	return 1.0f + pow(1.0f - clamp(radialDistance(viewDirection, sunDirection), 0.0f, 1.0f), 2.0f) * pi * 0.5f;
}

float getMie(float3 viewDirection, float3 sunDirection)
{
	float scale = 75.0f / cb_sunSize;

	float disk = clamp(1.0f - pow(radialDistance(viewDirection, sunDirection) * scale, 0.1f), 0.0f, 1.0f);

	return smooth(disk) * 2.0f * pi;
}

float hash11(float p)
{
	p = frac(p * 0.1031f);
	p *= p + 33.33f;
	p *= p + p;
	return frac(p);
}

float isPixelStar(float3 x)
{
	// The noise function returns a value in the range 0.0f -> 1.0f

	float3 p = floor(x);
	float3 f = frac(x);

	f = smooth(f);
	float n = p.x + p.y * 57.0f + 113.0f * p.z;

	return lerp(lerp(lerp(hash11(n), hash11(n + 1.0f), f.x),
		lerp(hash11(n + 57.0f), hash11(n + 58.0f), f.x), f.y),
		lerp(lerp(hash11(n + 113.0f), hash11(n + 114.0f), f.x),
			lerp(hash11(n + 170.0f), hash11(n + 171.0f), f.x), f.y), f.z);
}

float getLinearProgress(float edge0, float edge1, float x)
{
	return clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
}

float3 getAtmosphericScattering(float3 viewDirection, float3 sunDirection)
{
	float zenith = dayZenithDensity(viewDirection.y);
	float sunPointDistMult = clamp(length(max(sunDirection.y + cb_multiScatterPhase - cb_zenithOffset, 0.0f)), 0.0f, 1.0f);

	float rayleighMult = getRayleigMultiplier(viewDirection, sunDirection);

	float3 absorption = getSkyAbsorption(skyColour, zenith) * sunColour;
	float3 sunAbsorption = getSkyAbsorption(skyColour, dayZenithDensity(sunDirection.y + cb_multiScatterPhase)) * sunColour;
	float3 sky = skyColour * zenith * rayleighMult;
	float3 sun = getSunPoint(viewDirection, sunDirection) * absorption;
	float3 mie = getMie(viewDirection, sunDirection) * sunAbsorption;

	float3 totalSky = lerp(sky * absorption, sky / (sky + 0.5f), sunPointDistMult);
	totalSky += (sun + mie) * sunColour;
	totalSky *= sunAbsorption * 0.5f + 0.5f * length(sunAbsorption);

	if (sunDirection.y < cb_zenithOffset)
	{
		float skyRemovalStart = min(1.0f, (-sunDirection.y + cb_zenithOffset) / (0.45f + cb_zenithOffset)) * 2.0f;
		totalSky *= getLinearProgress(skyRemovalStart - 1.0f, skyRemovalStart, dot(viewDirection, sunDirection) * 0.5f + 0.5f);


		float nightSkyBlendFactor = min(1.0f, (-sunDirection.y + cb_zenithOffset) / (0.25f + cb_zenithOffset));

		float nightZenith = nightZenithDensity(viewDirection.y);
		nightZenith *= pow(getLinearProgress(-1.0f, 1.0f, dot(viewDirection, sunDirection)), 1.5f);
		float3 nightAbsorption = getSkyAbsorption(nightSkyColour, nightZenith);
		totalSky += nightSkyColour * nightZenith * nightAbsorption * nightSkyBlendFactor * 0.2f;

		float noiseSample = isPixelStar(float3(viewDirection) * 100.0f);
		if (noiseSample > 0.9f)
		{
			float intensity = (noiseSample - 0.9f) * 10.0f * nightSkyBlendFactor;
			totalSky += float3(intensity, intensity, intensity);
		}
	}

	//float nightSkyIntensity = smoothstep(0.2f, -0.2f, sunDirection.y - zenithOffset);
	//getSkyAbsorption(skyColour, zenith)
	//float nightZenith = zenithDensity(abs(viewDirection.y));
	//totalSky += skyColour * zenith * rayleighMult * nightSkyIntensity * pow(max(0.0f, dot(viewDirection, sunDirection)), 5.0f) * 0.1f;


	return totalSky;
}

float3 jodieReinhardTonemap(float3 c)
{
	float l = dot(c, float3(0.2126f, 0.7152f, 0.0722f));
	float3 tc = c / (c + 1.0f);

	return lerp(c / (l + 1.0f), tc, tc);
}

float4 main(VSPS_TRANSFER input) : SV_TARGET
{
	float3 viewDirection = normalize(input.worldPos - cb_cameraPosition);
    //float dirDot = dot(viewDirection, cb_directionalLight.direction);
    //return float4(dirDot, dirDot, dirDot, 0.0f);
	
	float3 colour = getAtmosphericScattering(viewDirection, -cb_directionalLight.direction) * pi;
	//colour += max(getAtmosphericScattering(viewDirection, -cb_directionalLight.direction) * pi, 0.0f);

	colour = jodieReinhardTonemap(colour);
	colour = pow(colour, float3(2.2f, 2.2f, 2.2f)); //Back to linear

    return float4(colour, 1.0f);
}