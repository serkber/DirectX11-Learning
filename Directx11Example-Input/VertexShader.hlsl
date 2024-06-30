cbuffer constantBuffer : register(b0)
{
    matrix mvp;
};

struct vsoutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

struct vsinput {
    float4 position : POSITION;
    float2 uv : TEXCOORD0;
    float4 color : COLOR;
};

vsoutput vsmain(vsinput input)
{
	vsoutput output;
	
    output.position = mul(input.position, mvp);
    output.uv = input.uv;
    output.color = input.color;

    return output;
}