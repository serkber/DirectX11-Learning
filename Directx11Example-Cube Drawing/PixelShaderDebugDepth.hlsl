Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct vsoutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

float4 psmain(vsoutput input) : SV_TARGET
{
    return Texture.Sample(TextureSampler, input.uv).r;
}