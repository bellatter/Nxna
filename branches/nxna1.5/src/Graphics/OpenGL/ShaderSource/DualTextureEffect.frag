uniform sampler2D Diffuse;
uniform sampler2D Diffuse2;

in HIGHP vec2 o_diffuseCoords1;
in HIGHP vec2 o_diffuseCoords2;

#if defined VERTEXCOLORENABLED
in HIGHP vec4 o_color;
#endif

#if __VERSION__ >= 130
out HIGHP vec4 outputColor;
#endif
void main()
{
	HIGHP vec4 finalColor;
#if __VERSION__ < 130
	HIGHP vec4 color1 = texture2D(Diffuse, o_diffuseCoords1);
	HIGHP vec4 color2 = texture2D(Diffuse2, o_diffuseCoords2);
#else
	vec4 color1 = texture(Diffuse, o_diffuseCoords1);
	vec4 color2 = texture(Diffuse2, o_diffuseCoords2);
#endif
#if defined VERTEXCOLORENABLED
	finalColor = vec4(color1.rgb * color2.rgb * 2.0, color1.a * color2.a) * o_color;
#else
	finalColor = vec4(color1.rgb * color2.rgb * 2.0, color1.a * color2.a);
#endif
#if __VERSION__ < 130
	gl_FragColor = finalColor;
#else
	outputColor = finalColor;
#endif
}