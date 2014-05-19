// fragment
/*
uniform sampler2DRect yuyv_tex;
void main(){
    float g = gl_FragCoord.x;//texture2DRect(bars, texcoord.xy).r; // we need just one channel
    vec4 rgba = texture2DRect(yuyv_tex, gl_TexCoord[0].xy);
    vec3 finalcol;
    if (g == 0.0) finalcol = convertYUVtoRGB(rgba.rga);
    else  finalcol = convertYUVtoRGB(rgba.bga);
    gl_FragColor = vec4(finalcol,1.0);
}
*/

//uniform sampler2DRect yuvTex;
//
//void main(){
//    vec4 yuvcolor; // f(xyz) -> yvu
//    float y, u , v;
//    vec4 rgbcolor;
//
//    y = texture2DRect(yuvTex, gl_TexCoord[0].xy).g;
//    u = texture2DRect(yuvTex, gl_TexCoord[0].xy).r-0.5;
//    v = texture2DRect(yuvTex, gl_TexCoord[0].xy).b-0.5;
//
//    rgbcolor.r = 2.0 * (y/2.0 + 1.402/2.0 * u);
//    rgbcolor.g = 2.0 * (y/2.0 - 0.344136 * v/2.0 - 0.714136 * u/2.0);
//    rgbcolor.b = 2.0 * (y/2.0 + 1.773/2.0 * v);
//
//    gl_FragColor = rgbcolor;
//}

// modified from: http://www.fourcc.org/fccyvrgb.php and http://www.fourcc.org/source/YUV420P-OpenGL-GLSLang.c
uniform sampler2DRect yuvTex;

void main(void){

    float nx, ny, r, g, b, a, y, u, v;

    vec4 txl,ux,vx;

    //float g = gl_TexCoord[0].x*1080 % 2;
//    vec4 rgba = texture2DRect(yuyv_tex, texcoord.xy);
//    vec3 finalcol;
//    if (g == 0.0) finalcol = convertYUVtoRGB(rgba.rga);
//    else  finalcol = convertYUVtoRGB(rgba.bga);
//    gl_FragColor = vec4(finalcol,1.0);

    nx = gl_TexCoord[0].x/2.0;
    ny = gl_TexCoord[0].y;

    y = texture2DRect(yuvTex, vec2(nx,ny)).g;
    u = texture2DRect(yuvTex, vec2(nx,ny)).b;
    v = texture2DRect(yuvTex, vec2(nx,ny)).r;

    y = 1.1643 * (y - 0.0625);
    u = u - 0.5;
    v = v - 0.5;

    r = y + 1.5958 * v;
    g = y - 0.39173 * u - 0.81290 * v;
    b = y + 2.017 * u;

    gl_FragColor = vec4(r, g, b, 1.0);
}
/*
uniform sampler2DRect rgbsrc;

vec4 y_const = vec4( 0.257,  0.504,  0.098, 1);
vec4 v_const = vec4( 0.439, -0.368, -0.071, 1);
vec4 u_const = vec4(-0.148, -0.291, +0.439, 1);

void main(){
    // read two rgb pixels
    vec4 rgb1 = texture2DRect(rgbsrc, gl_TexCoord[0].xy);
    vec4 rgb2 = texture2DRect(rgbsrc, gl_TexCoord[1].xy);
    vec4 rgb12 = (rgb1 + rgb2) * 0.5;

    vec4 yuyv; // yuyv -> rgba

    yuyv.b = dot(rgb1,  y_const) +  16.0/256.0; // Y1
    yuyv.g = dot(rgb12, u_const) + 128.0/256.0; // v
    yuyv.r = dot(rgb2,  y_const) +  16.0/256.0; // Y2
    yuyv.a = dot(rgb12, v_const) + 128.0/256.0; // u

    gl_FragColor = yuyv;
}*/
