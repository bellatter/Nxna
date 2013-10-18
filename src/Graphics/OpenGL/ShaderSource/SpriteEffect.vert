uniform HIGHP mat4 ModelViewProjection;
in vec3 position : POSITION0;
in vec2 texCoords : TEXCOORD0;
in vec4 color: COLOR0;
out vec2 o_diffuseCoords;
out vec4 o_color;
void main()
{
	gl_Position = ModelViewProjection * vec4(position, 1.0);
	o_diffuseCoords = texCoords;
	o_color = color;
}