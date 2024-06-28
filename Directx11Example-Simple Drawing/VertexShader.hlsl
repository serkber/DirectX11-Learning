//Not used right now
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
    uint instanceID : SV_InstanceID;    
    matrix mvp : INSTANCE_MVP;
    float4 instColor : INSTANCE_COLOR;
};

static matrix Identity =
{
    { 1, 0, 0, 0 },
    { 0, 1, 0, 0 },
    { 0, 0, 1, 0 },
    { 0, 0, 0, 1 }
};

vsoutput vsmain(vsinput input)
{
	vsoutput output;
	
    output.position = mul(input.position, input.mvp);
    output.uv = input.uv;
    output.color = input.color * input.instColor;

    return output;
}