cbuffer constantBuffer : register(b0)
{
    matrix mvp;
    float time;
    float2 mousePos;
};

Texture2D Texture : register(t0);
sampler TextureSampler : register(s0);

struct vsoutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

float4 psmain(vsoutput input) : SV_TARGET
{
    float2 mouse = mousePos;
    mouse.y = -mouse.y;
    mouse /= 2;
    mouse += 0.5;

    float2 magnifiedUvs = ((input.uv - 0.5) / 2) + 0.5;
    float mask = smoothstep(sin(time * 1.5) * 0.2 + 0.3, 1, saturate(1 - length(input.uv - mouse)));
    float2 finalUvs = lerp(input.uv, magnifiedUvs, mask);

    return float4(Texture.Sample(TextureSampler, finalUvs).rgb, 1) + float4(mouse.x, mouse.y, sin(time), 1) * mask;;
}