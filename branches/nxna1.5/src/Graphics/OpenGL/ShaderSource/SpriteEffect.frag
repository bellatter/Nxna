uniform sampler2D Diffuse;
in HIGHP vec2 o_diffuseCoords;
in HIGHP vec4 o_color;
#if VERSION >= 130
out vec4 outputColor;
#endif

void main()
{
#if VERSION < 130
	gl_FragColor = texture2D(Diffuse, o_diffuseCoords) * o_color;
#else
	outputColor = texture(Diffuse, o_diffuseCoords) * o_color;
#endif
}