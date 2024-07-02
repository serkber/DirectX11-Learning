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
};

struct vsinput {
    float4 position : POSITION;
};

vsoutput vsmain(vsinput input)
{
	vsoutput output;
	
    output.position = mul(input.position, modelMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projMatrix);

    return output;
}