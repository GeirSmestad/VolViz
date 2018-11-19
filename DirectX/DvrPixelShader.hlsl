struct PSInput
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

Texture3D volumeTexture : register(t0);
Texture1D transferFunctionTexture: register(t1);
Texture3D gradientsTexture : register(t2);

SamplerState volumeSampler : register(s0);

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

	// Debug only. Display transfer function, to see that it's is correctly loaded.
	if (input.uv.y < 0.025) 
	{
		return transferFunctionTexture.Sample(volumeSampler, input.uv.x, 0, 1);
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

		// X-ray absorption at this point, range [0,1]. (x, y, z and w components all have the same value).
		float datasetValueAtThisVoxel = volumeTexture.Sample(volumeSampler, samplingLocation, 0, 1).x;

		// Get color and opacity of this dataset value by sampling the transfer function at the given absorption.
		// TODO: I would expect the following line to work, but it leads to problem unrolling the loop. Alternative method yields no interpolation.
		// float4 transferFunctionValueAtThisVoxel = transferFunctionTexture.Sample(volumeSampler, datasetValueAtThisVoxel, 0, 1);*/
		float4 transferFunctionValuesAtThisVoxel = transferFunctionTexture.SampleLevel(volumeSampler, datasetValueAtThisVoxel, 0);

		float4 colorAtThisVoxel = float4(
			transferFunctionValuesAtThisVoxel.x,
			transferFunctionValuesAtThisVoxel.y,
			transferFunctionValuesAtThisVoxel.z,
			0);

		float opacityAtThisVoxel = transferFunctionValuesAtThisVoxel.w;

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