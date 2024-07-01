Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct vsoutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

float LinearizeDepth(float depth)
{
    return (depth * 0.05 * 100) / (100 - depth * (100 - 05));
}

float4 psmain(vsoutput input) : SV_TARGET
{
    float linearDepth = LinearizeDepth(Texture.Sample(TextureSampler, input.uv).r);
    return linearDepth;
}