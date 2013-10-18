uniform HIGHP mat4 ModelViewProjection;

in vec4 position : POSITION0;
in vec2 texCoords1 : TEXCOORD0;
in vec2 texCoords2 : TEXCOORD1;
out vec2 o_diffuseCoords1;
out vec2 o_diffuseCoords2;
#if defined VERTEXCOLORENABLED
in vec4 color : COLOR0;
out vec4 o_color;
#endif

void main()
{
	gl_Position = ModelViewProjection * position;
	o_diffuseCoords1 = texCoords1;
	o_diffuseCoords2 = texCoords2;
#if defined VERTEXCOLORENABLED
	o_color = color;
#endif
}