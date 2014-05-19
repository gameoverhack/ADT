#extension GL_ARB_texture_rectangle : enable
// sampler2DRect because we're using NPOT texture sizes

uniform sampler2DRect videoTexture;
uniform sampler2DRect contourTexture;
uniform vec2 tcOffset[25];
//uniform float level;

void main(){

	vec4 mask;
	vec4 video;

	// Get texels from textures
	video = texture2DRect(videoTexture, gl_TexCoord[0].xy);


	vec4 sample[25];
	vec4 minValue = vec4(1.0);
 
	for (int i = 0; i < 25; i++){
		// Sample a grid around and including our texel
		sample[i] = texture2DRect(contourTexture, gl_TexCoord[0].xy + tcOffset[i]);
 		
		// Keep the minimum value		
		minValue = min(sample[i], minValue);
		
	}
 	
	mask = minValue;

     gl_FragColor = mask * video;

}
