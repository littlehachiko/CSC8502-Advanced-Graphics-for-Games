#version 150 core
uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
    vec3 position;
} IN;

out vec4 gl_FragColor;

const vec3 shadow = vec3(0,0,0);
const vec3 fluid = vec3(0,0,0.8);
const vec3 snow = vec3(1,1,1);

void main(void){
        float gundam = clamp((IN.position.y/400.0f),0.0f,1.0f);
        float gundam1 = clamp((IN.position.y-100.0f)/100.0f,0.0f,1.0f);
        float gundam2 = clamp((IN.position.y/80.0f),0.0f,1.0f);
        vec4 miu=texture(diffuseTex,IN.texCoord);
	    //gl_FragColor = texture(diffuseTex,IN.texCoord);
        vec3 sazabi= mix(shadow,miu.rgb,gundam);
        vec3 sazabi1= mix(sazabi,snow,gundam1);
        vec3 sazabi2= mix(fluid,sazabi1,gundam2);
        //gl_FragColor.rgb = sazabi2;
        gl_FragColor.rgb= sazabi2 *snow;
	
	//float height = IN.position.y/500.0f;
	//float shadow = clamp(height,0.0f,1.0f);
}
