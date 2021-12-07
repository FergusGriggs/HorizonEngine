
cbuffer RenderTechniquesCB : register(b0)
{
    //PACK_SEAM
    int    useNormalMapping;
    int    useParallaxOcclusionMapping;
    float  depthScale;
    int    showWorldNormals;

    //PACK_SEAM
    int    showUVs;
    int    cullBackNormals;
    int    selfShadowing;
    int    gammaCorrection;

    //PACK_SEAM
    int    miscToggleA;
    int    miscToggleB;
    int    miscToggleC;
    int    roughnessMapping;
};
