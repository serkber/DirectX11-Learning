cbuffer cbChangesEveryFrame : register(b0)
{
    matrix modelMatrix;
};

cbuffer cbNeverChanges : register(b1)
{
    matrix viewMatrix;
};

cbuffer cbChangeOnResize : register(b2)
{
    matrix projMatrix;
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
	
    output.position = mul(input.position, modelMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projMatrix);
    output.uv = input.uv;
    output.color = input.color;

    return output;
}