uniform HIGHP mat4 ModelViewProjection;

in vec4 position : POSITION0;
in vec2 texCoords : TEXCOORD0;
#if defined VERTEXCOLORENABLED
in vec4 color : COLOR0;
out vec4 o_color;
#endif
out vec2 o_diffuseCoords;

void main()
{
	gl_Position = ModelViewProjection * position;
	o_diffuseCoords = texCoords;
#if defined VERTEXCOLORENABLED
	o_color = color;
#endif
}