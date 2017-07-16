#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D secondTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void){
	vec4 tex2 = texture(secondTex, IN.texCoord); //second texture (pup)
	gl_FragColor = texture(diffuseTex,IN.texCoord);  //first texture (glass)
	gl_FragColor = gl_FragColor *  tex2 * IN.colour;

	//gl_FragColor = texture(secondTex, IN.texCoord);
}