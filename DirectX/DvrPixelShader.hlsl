struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture3D volumeTexture : register(t0);
Texture1D transferFunctionTexture: register(t1);

SamplerState g_sampler : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	float4 offset;

	float4 ProjectionDirection;
	float4 BottomLeft;
	float4 RightSpan;
	float4 UpSpan;

	float4 DimensionSizeFactors;
	float StepSize;
};

float4 PSMain(PSInput input) : SV_TARGET
{
	// Direct Volume Rendering algorithm
	const float fixedStepSize = 0.002;
	const float threshold = 0.1;
	const float modelSpaceCenterOfDimension = 0.5f; // Sampling is always from [0,1]

	float4 rayPosition = BottomLeft + RightSpan * input.uv.x + UpSpan * input.uv.y;

	float rayLength = 0;
	float cutoffDistance = 5;
	float4 outputColor = {0,0,0,0};

	// Debug only. Test that transfer function is correctly loaded.
	if (input.uv.y < 0.1) 
	{
		return transferFunctionTexture.Sample(g_sampler, input.uv.x, 0, 1);
	}

	while (rayLength < cutoffDistance)
	{
		// Ray position is in intermediate space, must be translated to model space for sampling
		// This should ideally happen outside of the loop, but decent proof of concept.
		float4 samplingLocation = float4(
			modelSpaceCenterOfDimension + rayPosition.x / DimensionSizeFactors.x,
			modelSpaceCenterOfDimension + rayPosition.y / DimensionSizeFactors.y,
			modelSpaceCenterOfDimension + rayPosition.z / DimensionSizeFactors.z,
			0);

		// TODO: Modify by transfer function
		float4 colorAtThisVoxel = volumeTexture.Sample(g_sampler, samplingLocation, 0, 1);

		// TODO: Modify by transfer function
		float opacityAtThisVoxel = colorAtThisVoxel.x;


		//if (UsePhongShading)
		//{
		//	// TODO: Modify intensity at this voxel by shading
		//}

		//if (ModifyOpacityByGradientMagnitude) 
		//{
		//	// TODO: Modify opacity by gradient magnitude in this location
		//}

		// DVR compositing
		outputColor = colorAtThisVoxel * opacityAtThisVoxel + (1 - opacityAtThisVoxel) * outputColor;

		// Using dynamic step size screws up pixel shader debugger. Use fixed for now.
		rayPosition += ProjectionDirection * fixedStepSize;
		rayLength += fixedStepSize;		
	}

	return outputColor;
}