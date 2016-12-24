uniform HIGHP mat4 ModelViewProjection;

in vec4 position : POSITION0;
#if defined TEXTUREENABLED
in vec2 texCoords : TEXCOORD0;
out vec2 o_diffuseCoords;
#endif
#if defined VERTEXCOLORENABLED
in vec4 color : COLOR0;
out vec4 o_color;
#endif
		
void main()
{
	gl_Position = ModelViewProjection * position;
#if defined TEXTUREENABLED
	o_diffuseCoords = texCoords;
#endif
#if defined VERTEXCOLORENABLED
	o_color = color;
#endif
}