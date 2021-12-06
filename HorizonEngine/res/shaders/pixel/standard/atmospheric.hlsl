
cbuffer constantBuffer : register(b0)
{
	float3 sunDirection;
	float  sunSize;

	float3 cameraPosition;
	float  gameTime;

	float  density;
	float  multiScatterPhase;
	float  anisotropicIntensity;
	float  zenithOffset;

	float nightDensity;
	float nightZenithYClamp;
	float padding2;
	float padding3;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    float3 worldPos : WORLD_POSIITION;

	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
    //float3x3 TBNMatrix : TBN_MATRIX;
};

Texture2D diffuseTexture : TEXTURE : register(t0);
SamplerState samplerState : SAMPLER : register(s0);

static const float pi = 3.1415926f;
static const float invPi = 1.0f / 3.1415926f;

static const float3 sunColour = float3(1.0f, 1.0f, 1.0f);
static const float3 skyColour = float3(0.39f, 0.57f, 1.0f) * (1.0f + anisotropicIntensity); //Make sure one of the conponents is never 0.0 //0.180f, 0.725f, 1.0f//0.39f, 0.57f, 1.0f//0.06f, 0.625f, 1.0f
static const float3 nightSkyColour = float3(0.1f, 0.15f, 0.25f);// *(1.0f + anisotropicIntensity);

#define smooth(x) x * x * (3.0f - 2.0f * x)
#define dayZenithDensity(yCoord) density / pow(max(yCoord - zenithOffset, 0.0035f), 0.75f)
#define nightZenithDensity(yCoord) nightDensity / pow(max(yCoord + 0.05f, nightZenithYClamp), 0.75f)

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
	return smoothstep(sunSize * 0.0006f, sunSize * 0.00052f, radialDistance(viewDirection, sunDirection)) * 50.0f;
}

float getRayleigMultiplier(float3 viewDirection, float3 sunDirection)
{
	return 1.0f + pow(1.0f - clamp(radialDistance(viewDirection, sunDirection), 0.0f, 1.0f), 2.0f) * pi * 0.5f;
}

float getMie(float3 viewDirection, float3 sunDirection)
{
	float scale = 75.0f / sunSize;

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
	float sunPointDistMult = clamp(length(max(sunDirection.y + multiScatterPhase - zenithOffset, 0.0f)), 0.0f, 1.0f);

	float rayleighMult = getRayleigMultiplier(viewDirection, sunDirection);

	float3 absorption = getSkyAbsorption(skyColour, zenith) * sunColour;
	float3 sunAbsorption = getSkyAbsorption(skyColour, dayZenithDensity(sunDirection.y + multiScatterPhase)) * sunColour;
	float3 sky = skyColour * zenith * rayleighMult;
	float3 sun = getSunPoint(viewDirection, sunDirection) * absorption;
	float3 mie = getMie(viewDirection, sunDirection) * sunAbsorption;

	float3 totalSky = lerp(sky * absorption, sky / (sky + 0.5f), sunPointDistMult);
	totalSky += (sun + mie) * sunColour;
	totalSky *= sunAbsorption * 0.5f + 0.5f * length(sunAbsorption);

	if (sunDirection.y < zenithOffset)
	{
		float skyRemovalStart = min(1.0f, (-sunDirection.y + zenithOffset) / (0.45f + zenithOffset)) * 2.0f;
		totalSky *= getLinearProgress(skyRemovalStart - 1.0f, skyRemovalStart, dot(viewDirection, sunDirection) * 0.5f + 0.5f);


		float nightSkyBlendFactor = min(1.0f, (-sunDirection.y + zenithOffset) / (0.25f + zenithOffset));

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

float4 main(PS_INPUT input) : SV_TARGET
{
	float3 viewDirection = normalize(input.worldPos - cameraPosition);
	float3 color = getAtmosphericScattering(viewDirection, sunDirection) *pi;
	//color += max(getAtmosphericScattering(viewDirection, -sunDirection) * pi, 0.0f);

	color = jodieReinhardTonemap(color);
	color = pow(color, float3(2.2f, 2.2f, 2.2f)); //Back to linear

	return float4(color, 1.0f);
}