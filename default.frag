#version 460 core

out vec4 FragColor;
uniform sampler2D screen;
uniform float iTime;
in vec2 texCoord;

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

vec4 gaussianBlur()
{
    // Define the 3x3 kernel for blur (simple average for example)
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

float random(vec2 p) {
    return fract(sin(dot(p, vec2(12.9898, 78.233))) * 43758.5453);
}

void main()
{
	// Sample the color from the texture at the given texture coordinates
    vec4 texColor = texture(screen, texCoord);

    //vec4 texColor = gaussianBlur();//texture(screen, texCoord);


	if (texColor.r > 0.00001f) {

		texColor.rgb = palette(texColor.r);
	}	


	FragColor = texColor;

    /*

    vec2 p = texCoord.xy + iTime * 0.1; // Adding time-based shift to coords
    float randomValue = random(p);
    FragColor = vec4(randomValue, randomValue, randomValue, 1.0);

    */

	//FragColor = texture(screen, texCoord);
}