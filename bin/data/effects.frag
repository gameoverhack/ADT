#extension GL_ARB_texture_rectangle : enable
// sampler2DRect because we're using NPOT texture sizes

uniform sampler2DRect videoTexture;
uniform sampler2DRect contourTexture;

uniform vec2 tcOffset[25]; // offsets for subsampling

uniform float fLevel; // fade
uniform float rLevel; // red
uniform float gLevel; // green
uniform float bLevel; // blue

uniform int effects[2]; // effects[0] = color/video, effects[1] = unusued

void main(){

	vec4 mask;
	vec4 video;
	vec4 output;

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

	if(effects[0] == 1){
		// color and fade video
		video *= vec4(fLevel * rLevel, fLevel * gLevel, fLevel * bLevel, 1.0);
		output = mask * video;
	}else{
		mask *= vec4(fLevel * rLevel, fLevel * gLevel, fLevel * bLevel, 1.0);
		output = mask;
	}

	// output
     gl_FragColor = output;

}
