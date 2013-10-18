uniform highp vec4 AlphaTest;
uniform sampler2D Diffuse;

in HIGHP vec2 o_diffuseCoords;
#if defined VERTEXCOLORENABLED
in HIGHP vec4 o_color;
#endif
#if __VERSION__ >= 130
out vec4 outputColor;
#endif

void clip(HIGHP float a)
{
	if (a < 0.0) discard;
}

void main()
{
	HIGHP vec4 finalColor;
#if __VERSION__ < 130
	finalColor = texture2D(Diffuse, o_diffuseCoords);
#else
	finalColor = texture(Diffuse, o_diffuseCoords);
#endif
#if defined VERTEXCOLORENABLED
	finalColor *= o_color;
#endif

#if defined LESSGREATER
	clip((finalColor.a < AlphaTest.x) ? AlphaTest.z : AlphaTest.w);
#else
	clip((abs(finalColor.a - AlphaTest.x) < AlphaTest.y) ? AlphaTest.z : AlphaTest.w);
#endif
		
#if __VERSION__ < 130
	gl_FragColor = finalColor;
#else
	outputColor = finalColor;
#endif

}