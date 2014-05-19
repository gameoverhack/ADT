#extension GL_ARB_texture_rectangle : enable
// sampler2DRect because we're using NPOT texture sizes

uniform sampler2DRect videoTexture;
uniform sampler2DRect contourTexture;
//uniform float level;

void main(){

	vec4 mask;
	vec4 video;

	// Get texels from textures
	mask	= texture2DRect(contourTexture, gl_TexCoord[0].xy);
	video	= texture2DRect(videoTexture, gl_TexCoord[0].xy);

     gl_FragColor = mask * video;

}
