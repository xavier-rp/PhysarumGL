#version 460 core

out vec4 FragColor;
uniform sampler2D screen;
uniform float iTime;
in vec2 texCoord;

const float kernel3x3[9] = float[9](
    1.0, 2.0, 1.0,
    2.0, 4.0, 2.0,
    1.0, 2.0, 1.0
);

// 4x4 Gaussian Kernel (normalized)
const float kernel4x4[16] = float[16](
    1.0,  4.0,  6.0,  4.0,
    4.0, 16.0, 24.0, 16.0,
    6.0, 24.0, 36.0, 24.0,
    4.0, 16.0, 24.0, 16.0
);

// 5x5 Gaussian Kernel (normalized)
const float kernel5x5[25] = float[25](
    1.0,  4.0,  6.0,  4.0,  1.0,
    4.0, 16.0, 24.0, 16.0,  4.0,
    6.0, 24.0, 36.0, 24.0,  6.0,
    4.0, 16.0, 24.0, 16.0,  4.0,
    1.0,  4.0,  6.0,  4.0,  1.0
);

// function to generate cycling color palettes
// https://iquilezles.org/articles/palettes/
// http://dev.thi.ng/gradients/
vec3 palette(float t)
{
	vec3 a = vec3(0.731, 1.098, 0.192);
	vec3 b = vec3(0.358, 1.090, 0.657); 
	vec3 c = vec3(1.077, 0.360, 0.328);
	vec3 d = vec3(0.965, 2.265, 0.837);
	return a + b*cos( 6.28318*(c*t+d));
}

vec4 averageBlur()
{
    vec2 texOffset = 1.0 / textureSize(screen, 0); // Gets the texture size (width, height)
    
    // Sample the 9 surrounding texels
    vec4 color = vec4(0.0);
    color += texture(screen, texCoord + texOffset * vec2(-1.0,  1.0));  // top-left
    color += texture(screen, texCoord + texOffset * vec2( 0.0,  1.0));  // top
    color += texture(screen, texCoord + texOffset * vec2( 1.0,  1.0));  // top-right
    
    color += texture(screen, texCoord + texOffset * vec2(-1.0,  0.0));  // left
    color += texture(screen, texCoord);  // center (current pixel)
    color += texture(screen, texCoord + texOffset * vec2( 1.0,  0.0));  // right
    
    color += texture(screen, texCoord + texOffset * vec2(-1.0, -1.0));  // bottom-left
    color += texture(screen, texCoord + texOffset * vec2( 0.0, -1.0));  // bottom
    color += texture(screen, texCoord + texOffset * vec2( 1.0, -1.0));  // bottom-right
    
    // Normalize and output the result
    color /= 9.0;  // Average the 9 samples

    return color;

}

vec4 gaussianBlur3x3()
{
    vec2 texOffset = 1.0 / textureSize(screen, 0);  // Calculates the offset for sampling neighboring texels

    // Initialize the result color

    vec4 result = vec4(0.0);
    // Loop over the 3x3 kernel and apply the blur
    int index = 0; // Index for accessing the kernel array
    for (int x = -1; x <= 1; ++x) {
        for (int y = -1; y <= 1; ++y) {
            vec2 offset = vec2(x, y) * texOffset;      // Offset for the neighboring texel
            float weight = kernel3x3[index] / 16.0; // Get the weight from the kernel (normalized)

            // Add the weighted color value to the result
            result += texture(screen, texCoord + offset) * weight;

            index++; // Move to the next element in the kernel array
        }
    }

    return result;

}

vec4 gaussianBlur5x5()
{
    vec2 texOffset = 1.0 / textureSize(screen, 0);  // Calculates the offset for sampling neighboring texels

    // Initialize the result color

    vec4 result = vec4(0.0);
    // Loop over the 3x3 kernel and apply the blur
    int index = 0; // Index for accessing the kernel array
    for (int x = -2; x <= 2; ++x) {
        for (int y = -2; y <= 2; ++y) {
            vec2 offset = vec2(x, y) * texOffset;      // Offset for the neighboring texel
            float weight = kernel5x5[index] / 256.0; // Get the weight from the kernel (normalized)

            // Add the weighted color value to the result
            result += texture(screen, texCoord + offset) * weight;

            index++; // Move to the next element in the kernel array
        }
    }

    return result;

}

float random(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	// Sample the color from the texture at the given texture coordinates
    vec4 texColor = texture(screen, texCoord);

    //Allows for a smooth gradient between the color of the tails and the background color(black)
    //The alpha channel is used to store the actual Trail value. Small values indicate that the trail at this position has evaporated a lot
    //hence why it is related to the tails of the agents
    if (texColor.a < 0.10){

        texColor.rgb = mix(vec3(0.0f, 0.0f, 0.0f), texColor.rgb, smoothstep(0.0f, 1.0f, 10.0*texColor.a));
        
    }

    //Activate to blur the image with a gaussian mask. See functions above for 3x3, 4x4, 5x5 and average blur
    //vec4 texColor = gaussianBlur3x3();//texture(screen, texCoord);

	FragColor = texColor;


}