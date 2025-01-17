#version 330 core

// function to generate cycling color palettes
// https://iquilezles.org/articles/palettes/
// http://dev.thi.ng/gradients/
vec3 palette(float t)
{
	vec3 a = vec3(0.5, 0.5, 0.5);
	vec3 b = vec3(0.5, 0.5, 0.5); 
	vec3 c = vec3(1.0, 1.0, 1.0);
	vec3 d = vec3(0.263, 0.416, 0.557);
	return a + b*cos( 6.28318*(c*t+d));
}

// Outputs colors in RGBA
out vec4 FragColor;

// Uniforms
uniform float iTime;
uniform vec2  iResolution;

void main()
{
	// uv coordinates originally go from 0 to 1.
	// with the following operations, we transform the coordinates so they go from -1 to 1
	// and the center of the canvas is now (0, 0).
	vec2 uv0 = gl_FragCoord.xy / iResolution.xy * 2.0 - 1.0;

	// Scale the u coordinate so that it's interval from -1 to 1 spans the same number of pixels as the y coordinate.
	uv0.x *= iResolution.x / iResolution.y;

	// Time varying pixel color
    vec3 col = 0.5 + 0.5*cos(iTime+uv0.xyx+vec3(0,2,4));

	FragColor = vec4(col, 1.0f);
}