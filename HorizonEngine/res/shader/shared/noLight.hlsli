
#ifndef __NO_LIGHT_HEADER_HLSL__
#define __NO_LIGHT_HEADER_HLSL__

cbuffer NoLightCB : register(b1)
{
    float3 cb_colour;
    int    cb_justColour;
};

#endif //__NO_LIGHT_HEADER_HLSL__
