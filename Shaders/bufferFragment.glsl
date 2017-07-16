# version 150 core

uniform sampler2D diffuseTex ; // Diffuse texture map
uniform sampler2D bumpTex ; // Bump map
uniform vec3 cameraPos;
uniform sampler2DShadow shadowTex;
uniform vec4 lightColour;
uniform vec3 lightPos;
uniform float lightRadius;

in Vertex {
vec4 colour ;
vec2 texCoord ;
vec3 normal ;
vec3 worldPos ;
vec3 tangent ; // New !
vec3 binormal ; // New !
vec4 shadowProj;
 } IN ;

out vec4 gl_FragColor [2]; // Our final outputted colours !
//const vec3 shadow=vec3(0,0,0);

 void main ( void ) {

	vec4 diffuse = texture(diffuseTex,IN.texCoord);
 
	mat3 TBN = mat3 ( IN . tangent , IN . binormal , IN . normal );
 
	vec3 normal = normalize ( TBN *( texture2D ( bumpTex , IN . texCoord ). rgb )* 2.0 - 1.0);

	/*vec3 incident = normalize(lightPos - IN.worldPos);

	float lambert = max(0.0,dot(incident,IN.normal));
	
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist/lightRadius,0.2,1.0);

	vec3 viewDir = normalize(cameraPos-IN.normal);
	vec3 halfDir = normalize(incident+viewDir);

	float rFactor = max(0.0,dot(halfDir,IN.normal));
	float sFactor = pow(rFactor,50.0);*/

	float shadow = 1.0;
	int test = 0;
	if (IN.shadowProj.w>0.0){
			shadow = textureProj(shadowTex,IN.shadowProj)*0.5+0.5;
			test = 1;
	}
	//lambert *= shadow;
	
	//vec3 colour = (diffuse.rgb*lightColour.rgb);
	
	//colour += (lightColour.rgb*sFactor)*0.33;//*/
	
	gl_FragColor [0] =vec4(diffuse.rgb*shadow,diffuse.a);

	//float mix1 = clamp((IN.worldPos.y-50.0f)/100.0f,0.0f,1.0f);

	//gl_FragColor [0].rgb+=(diffuse.rgb*lightColour.rgb)*0.1;


	//gl_FragColor [0] = texture2D ( diffuseTex , IN . texCoord );
	gl_FragColor [1] = vec4 ( (normal . xyz*shadow) * 0.5 + 0.5 ,1.0);
	
	//gl_FragColor [0].rgb = lightColour.rgb;
	/*if (test == 1) {
		gl_FragColor [0] =vec4(1, 0, 0, 1);
		gl_FragColor [1] = vec4 ( (normal . xyz) * 0.5 + 0.5 ,1.0);
	}*/
 }