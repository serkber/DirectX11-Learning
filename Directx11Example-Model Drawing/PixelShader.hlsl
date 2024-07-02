Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct vsoutput {
    float4 position : SV_POSITION;
};

float4 psmain(vsoutput input) : SV_TARGET
{
    return 1;
}