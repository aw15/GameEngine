#include "BasicHeader.hlsl"

//////////////
// TYPEDEFS //
//////////////
struct VertexInputType
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 Normal : NORMAL;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
PixelInputType FireVertexShader(VertexInputType input)
{
    PixelInputType output;
    


	// Change the position vector to be 4 units for proper matrix calculations.
    input.position.w = 1.0f;

	// Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, gWorld);
    output.position = mul(output.position, gView);
    output.position = mul(output.position, gProj);
    
	// Store the texture coordinates for the pixel shader.
	output.tex = input.tex;
 

	
    return output;
}


////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
////////////////////////////////////////////////////////////////////////////////
float4 FirePixelShader(PixelInputType input) : SV_TARGET
{
    float4 noise1;
    float4 noise2;
    float4 noise3;
    float4 finalNoise;
    float perturb;
    float2 noiseCoords;
    float4 fireColor;
    float4 alphaColor;

    float2 distortion1 = {0.1f, 0.2f};
    float2 distortion2 = {0.1f, 0.3f};
    float2 distortion3 = {0.1f, 0.1f};
    float distortionScale = 0.8f;
    float distortionBias = 0.5f;


    float3 gScrollSpeeds = { 1.3f, 2.1f, 2.3f };
    float3 gScales = { 1.0f, 2.0f, 3.0f };

    // Compute texture coordinates for first noise texture using the first scale and upward scrolling speed values.
    float2 texCoords1 = (input.tex * gScales.x);
    texCoords1.y = texCoords1.y + (gTotalTime * gScrollSpeeds.x);

    // Compute texture coordinates for second noise texture using the second scale and upward scrolling speed values.
    float2 texCoords2 = (input.tex * gScales.y);
    texCoords2.y = texCoords2.y + (gTotalTime * gScrollSpeeds.y);

    // Compute texture coordinates for third noise texture using the third scale and upward scrolling speed values.
    float2 texCoords3 = (input.tex * gScales.z);
    texCoords3.y = texCoords3.y + (gTotalTime * gScrollSpeeds.z);

	// Sample the same noise texture using the three different texture coordinates to get three different noise scales.
    noise1 = gDiffuseMap[FIRE_NOISE].Sample(gsamLinearWrap, texCoords1);
    noise2 = gDiffuseMap[FIRE_NOISE].Sample(gsamLinearWrap, texCoords2);
    noise3 = gDiffuseMap[FIRE_NOISE].Sample(gsamLinearWrap, texCoords3);

	// Move the noise from the (0, 1) range to the (-1, +1) range.
    noise1 = (noise1 - 0.5f) * 2.0f;
    noise2 = (noise2 - 0.5f) * 2.0f;
    noise3 = (noise3 - 0.5f) * 2.0f;

	// Distort the three noise x and y coordinates by the three different distortion x and y values.
    noise1.xy = noise1.xy * distortion1.xy;
    noise2.xy = noise2.xy * distortion2.xy;
    noise3.xy = noise3.xy * distortion3.xy;

	// Combine all three distorted noise results into a single noise result.
    finalNoise = noise1 + noise2 + noise3;

	// Perturb the input texture Y coordinates by the distortion scale and bias values.  
	// The perturbation gets stronger as you move up the texture which creates the flame flickering at the top effect.
    perturb = ((1.0f - input.tex.y) * distortionScale) + distortionBias;

	// Now create the perturbed and distorted texture sampling coordinates that will be used to sample the fire color texture.
    noiseCoords.xy = (finalNoise.xy * perturb) + input.tex.xy;

	// Sample the color from the fire texture using the perturbed and distorted texture sampling coordinates.
	// Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
    fireColor = gDiffuseMap[FIRE].Sample(gsamLinearClamp, noiseCoords.xy);

	// Sample the alpha value from the alpha texture using the perturbed and distorted texture sampling coordinates.
	// This will be used for transparency of the fire.
	// Use the clamping sample state instead of the wrap sample state to prevent flames wrapping around.
    alphaColor = gDiffuseMap[FIRE_ALPHA].Sample(gsamLinearClamp, noiseCoords.xy);

	// Set the alpha blending of the fire to the perturbed and distored alpha texture value.
    fireColor.a = alphaColor;
	
    return fireColor;
}
