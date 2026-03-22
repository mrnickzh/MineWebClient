#version 300 es

precision highp float;
precision highp sampler2D;

uniform sampler2D uFrameTex;
uniform vec2 uViewportSize;
uniform int uEnabled;

in vec2 fragCoord;

out vec4 fragColor;

#define FXAA_REDUCE_MIN   (1.0/ 128.0)
#define FXAA_REDUCE_MUL   (1.0 / 8.0)
#define FXAA_SPAN_MAX     8.0

void main(void)
{
	if (uEnabled == 0) {
		fragColor = texture(uFrameTex, fragCoord);
		return;
	}
	vec2 inverseVP = vec2(1.0 / uViewportSize.x, 1.0 / uViewportSize.y);
	vec3 rgbNW = texture(uFrameTex, fragCoord + (vec2(-1.0, -1.0) * inverseVP)).xyz;
	vec3 rgbNE = texture(uFrameTex, fragCoord + (vec2(1.0, -1.0) * inverseVP)).xyz;
	vec3 rgbSW = texture(uFrameTex, fragCoord + (vec2(-1.0, 1.0) * inverseVP)).xyz;
	vec3 rgbSE = texture(uFrameTex, fragCoord + (vec2(1.0, 1.0) * inverseVP)).xyz;
	vec3 rgbM  = texture(uFrameTex, fragCoord).xyz;
	vec3 luma = vec3(0.299, 0.587, 0.114);
	float lumaNW = dot(rgbNW, luma);
	float lumaNE = dot(rgbNE, luma);
	float lumaSW = dot(rgbSW, luma);
	float lumaSE = dot(rgbSE, luma);
	float lumaM  = dot(rgbM,  luma);
	float lumaMin = min(lumaM, min(min(lumaNW, lumaNE), min(lumaSW, lumaSE)));
	float lumaMax = max(lumaM, max(max(lumaNW, lumaNE), max(lumaSW, lumaSE)));

	vec2 dir;
	dir.x = -((lumaNW + lumaNE) - (lumaSW + lumaSE));
	dir.y =  ((lumaNW + lumaSW) - (lumaNE + lumaSE));

	float dirReduce = max((lumaNW + lumaNE + lumaSW + lumaSE) *(0.25 * FXAA_REDUCE_MUL), FXAA_REDUCE_MIN);

	float rcpDirMin = 1.0 / (min(abs(dir.x), abs(dir.y)) + dirReduce);
	dir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX), max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX), dir * rcpDirMin)) * inverseVP;

	vec3 rgbA = 0.5 * (
	texture(uFrameTex, fragCoord + dir * (1.0 / 3.0 - 0.5)).xyz +
	texture(uFrameTex, fragCoord + dir * (2.0 / 3.0 - 0.5)).xyz);
	vec3 rgbB = rgbA * 0.5 + 0.25 * (
	texture(uFrameTex, fragCoord + dir * -0.5).xyz +
	texture(uFrameTex, fragCoord + dir * 0.5).xyz);

	float lumaB = dot(rgbB, luma);
	if ((lumaB < lumaMin) || (lumaB > lumaMax))
		fragColor = vec4(rgbA, 1.0);
	else
		fragColor = vec4(rgbB, 1.0);
}