#if defined TEXTUREENABLED
uniform sampler2D Diffuse;
in HIGHP vec2 o_diffuseCoords;
#endif

#if defined VERTEXCOLORENABLED
in HIGHP vec4 o_color;
#endif

#if __VERSION__ >= 130
out HIGHP vec4 outputColor;
#endif
void main()
{
	HIGHP vec4 finalColor;
#if defined TEXTUREENABLED

#if __VERSION__ < 130
	finalColor = texture2D(Diffuse, o_diffuseCoords);
#else
	finalColor = texture(Diffuse, o_diffuseCoords);
#endif
#if defined VERTEXCOLORENABLED
	finalColor *= o_color;
#endif
#elif defined VERTEXCOLORENABLED
	finalColor = o_color;
#endif
	
#if __VERSION__ < 130
	gl_FragColor = finalColor;
#else
	outputColor = finalColor;
#endif

}