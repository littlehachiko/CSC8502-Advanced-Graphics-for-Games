#version 150 core

uniform sampler2D diffuseTex;

in Vertex	{
	vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void)	{
	vec4 output = texture(diffuseTex, IN.texCoord);
	
	vec2 tsin = sin(IN.texCoord * 3.14);
	
	float sinamt = clamp(tsin.x+tsin.y-1.0, 0, 0.5);
	
	vec2 lTex = IN.texCoord - vec2(0.1,0);
	vec2 rTex = IN.texCoord + vec2(0.1,0);
	
	lTex = sin(lTex* 3.14);
	rTex = sin(rTex* 3.14);
	
	float lblend = clamp(lTex.x+lTex.y-1.0, 0, 0.5);
	float rblend = clamp(rTex.x+rTex.y-1.0, 0, 0.5);
	
	vec4 outputLeft 	= texture(diffuseTex, lTex);
	vec4 outputRight 	= texture(diffuseTex, rTex);
		
	gl_FragColor = (output * (1.0 - (lblend + rblend)) + (outputLeft * lblend) + (outputRight * rblend));
	
	//gl_FragColor = (outputLeft * 0.5) + (outputRight * 0.5);
	
	//gl_FragColor = vec4(sinamt,sinamt,sinamt,1);
	
	
	//gl_FragColor = output;
		
	gl_FragColor.a = 1.0;
	//gl_FragColor = vec4(1);
}