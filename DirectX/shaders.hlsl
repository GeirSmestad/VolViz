struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture3D g_texture : register(t0);
SamplerState g_sampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float4 offset;

	float4 ProjectionDirection;
	float4 BottomLeft;
	float4 RightSpan;
	float4 UpSpan;

	float StepSize;
};

PSInput VSMain(float4 position : POSITION, float4 uv : TEXCOORD)
{
	PSInput result;

	result.position = position;
	result.uv = uv;

	return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	// First-hit rendering algorithm
	const float fixedStepSize = 0.01;
	const float threshold = 0.1;

	float rayLength = 0;
	float cutoffDistance = 2;

	float4 rayPosition = BottomLeft + RightSpan * input.uv.x + UpSpan * input.uv.y;

	while (rayLength < cutoffDistance)
	{
		float4 voxelValue = g_texture.Sample(g_sampler, rayPosition, 0, 1);

		if (voxelValue.x > threshold)
		{
			return voxelValue;
		}

		// Using dynamic step size screws up PS debugger. Use fixed for now.
		rayPosition += ProjectionDirection * fixedStepSize;
		rayLength += fixedStepSize;		
	}

	float4 zeroColor = { 0, 0, 0, 0 };

	return zeroColor;

}