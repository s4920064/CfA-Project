/// code originally written by Richard Southern  *

#version 430

// The texture to be mapped
uniform sampler2D gameTex;
uniform sampler2D depthTex;
uniform sampler2D textTex;
uniform sampler2D maskTex;

// The depth at which we want to focus
uniform float focalDepth = 0.93;

// A scale factor for the radius of blur
uniform float blurRadius = 0.03;

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

// We pass the window size to the shader.
uniform vec2 windowSize;
uniform vec2 textSize;

/***************************************************************************************************
 * Gaussian Blur functions and constants
 ***************************************************************************************************/
// Gaussian coefficients
const float G5x5[25] = {0.0035,    0.0123,    0.0210,    0.0123,    0.0035,
                        0.0123,    0.0543,    0.0911,    0.0543,    0.0123,
                        0.0210,    0.0911,    0.2224,    0.0911,    0.0210,
                        0.0123,    0.0543,    0.0911,    0.0543,    0.0123,
                        0.0035,    0.0123,    0.0210,    0.0123,    0.0035};

const float G9x9[81] = {0,         0,    0.0039,    0.0039,    0.0039,    0.0039,    0.0039,         0,         0,
                        0,    0.0039,    0.0078,    0.0117,    0.0117,    0.0117,    0.0078,    0.0039,         0,
                        0.0039,    0.0078,    0.0117,    0.0234,    0.0273,    0.0234,    0.0117,    0.0078,    0.0039,
                        0.0039,    0.0117,    0.02343,    0.0352,    0.0430,    0.0352,    0.0234,    0.0117,    0.0039,
                        0.0039,    0.0117,    0.0273,    0.0430,    0.0469,    0.0430,    0.0273,    0.0117,    0.0039,
                        0.0039,    0.0117,    0.0234,    0.0352,    0.0430,    0.0352,    0.0234,    0.0117,    0.0039,
                        0.0039,    0.0078,    0.0117,    0.0234,    0.0273,    0.0234,    0.0117,    0.0078,    0.0039,
                        0,    0.0039,    0.0078,    0.0117,    0.0117,    0.0117,    0.0078,    0.0039,         0,
                        0,         0,    0.0039,    0.0039,    0.0039,    0.0039,    0.0039,         0,         0};

// These define which Gaussian kernel and the size to use (G5x5 and 5 also possible)
#define SZ 9
#define G  G9x9

// Gaussian filter for regular smooth blur
vec4 GaussianFilter(vec2 texpos, float sigma) {
    // We need to know the size of half the window
    int HSZ = int(floor(SZ / 2));

    int i,j;
    vec4 colour = vec4(0.0);
    vec2 samplepos;

    // Note that this loops over n^2 values. Is there a more efficient way to do this?
    for (i=0; i<SZ; ++i) {
        for (j=0; j<SZ; ++j) {
            samplepos = texpos + sigma*vec2(float(i)-HSZ, float(j)-HSZ);
            colour += G[i*SZ+j] * texture(gameTex, samplepos);
        }
    }
    return colour;
}

/***************************************************************************************************
 * Poisson Blur functions and constants
 ***************************************************************************************************/
// Tap locations for 13 sample Poisson unit disc
const vec2 PoissonDisc[12] = {vec2(-0.326212,-0.40581),vec2(-0.840144,-0.07358),
                              vec2(-0.695914,0.457137),vec2(-0.203345,0.620716),
                              vec2(0.96234,-0.194983),vec2(0.473434,-0.480026),
                              vec2(0.519456,0.767022),vec2(0.185461,-0.893124),
                              vec2(0.507431,0.064425),vec2(0.89642,0.412458),
                              vec2(-0.32194,-0.932615),vec2(-0.791559,-0.59771)};

// A pseudorandom generator for the Poisson disk rotation
float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// The Poisson filter for irregular blurring
vec4 PoissonFilter(sampler2D tex, vec2 texpos, float sigma) {
    int i;
    vec4 colour = texture(tex, texpos);
    float angle = rand(texpos);
    mat2 rot = mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
    for (i = 0; i < 12; ++i) {
        vec2 samplepos = texpos + 2 * sigma * rot * PoissonDisc[i];
        colour += texture(tex, samplepos);
    }
    return colour * 0.076923077; // Same as "/ 13.0"
}

/// end of code  *

vec4 ChromaticAbberationBlur(sampler2D tex, vec2 UVpos, float sigma, float offset)
{
    // the three offsetted color channels to create the chromatic abberation effect
    float blurredR = PoissonFilter(tex, vec2(UVpos.x-offset,UVpos.y),sigma).r;
    float blurredG = PoissonFilter(tex, UVpos, sigma).g;
    vec2 blurredBA = PoissonFilter(tex, vec2(UVpos.x+offset,UVpos.y),sigma).ba;

    // final color
    return vec4(blurredR, blurredG, blurredBA.x, blurredBA.y);
}

void main() {


    // Determine the texture coordinate from the window size
    vec2 texPos = gl_FragCoord.xy / windowSize;

    vec2 gameSize = vec2(0.6,1)*0.9;

    //-------------------Game Viewport------------------
    // texture coordinates for drawing in the game viewport
    vec2 gameOffset = vec2(0.15,-0.05);
    vec2 gamePos = texPos*1.1+gameOffset;//vec2(gameOffset.x,-gameOffset.y);
    //vec2 gamePos = texPos;

    // Determine sigma, the blur radius of this pixel
    float sigma = abs(focalDepth - texture(depthTex, gamePos).x) * blurRadius;

    // chromatic abberation offset (weighted for depth, otherwise it looks like there's
    //                              two projectiles in the distance when there's only one)
    float gameCAOffset = 0.001*(pow(texture2D(depthTex, gamePos).r,3));

    // final color of the game viewport
    vec4 gameColor = ChromaticAbberationBlur(gameTex, gamePos, sigma, gameCAOffset);
    //--------------------------------------------------

    //----------------------Game UI---------------------

    float flatCAOffset = 0;

    //vec4 backColor = ChromaticAbberationBlur(backTex, textPos, sigma, flatCAOffset);
//    vec4 backColor = vec4(0.05,0.05,0.05,1);
//    float border = 0.05;

    vec2 _textSize = windowSize;

    _textSize.y = -windowSize.y+1;

//    float textOffset = 0.5;

//    // the coordinates of our text surface (the y is flipped to put it right side up)
    vec2 textPos = gl_FragCoord.xy / _textSize;

    vec4 textColor = ChromaticAbberationBlur(textTex, textPos, sigma, flatCAOffset);

    vec4 maskColor = ChromaticAbberationBlur(maskTex, textPos, sigma, flatCAOffset);

//    // if the alpha for the text is black then so is the color
//    if(textColor.a==0 ) {textColor=vec4(0);}

    // if the pixel is not within the text texture bounds, then make it black
    // (otherwise it wraps)
//    if(texPos.x>textSize.x/windowSize.x*(6+textOffset) ||
//       texPos.x<textSize.x/windowSize.x*(5+textOffset) ||
//       texPos.y>textSize.y/windowSize.y*(9+textOffset))
//    {
//        textColor=vec4(0);
//    }

//    // a mask for the game viewport
//    float mask = 1;
//    // if the pixel is within the game viewport
//    if( texPos.x > border &&
//        texPos.y > border &&
//        texPos.x < gameSize.x+border &&
//        texPos.y < gameSize.y+border
//       )
//    {
//        // the mask is black
//        mask = 0;
//    }
    //--------------------------------------------------

    gameColor *= maskColor;

    //fragColor = textColor;
    fragColor = textColor+gameColor;
    //fragColor = gameColor;

}

