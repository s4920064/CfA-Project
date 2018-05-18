/// code originally written by Richard Southern  *

#version 430

// The output colour. At location 0 it will be sent to the screen.
layout (location=0) out vec4 fragColor;

// The texture to be mapped
uniform sampler2D gameTex;
uniform sampler2D depthTex;
uniform sampler2D textTex;
uniform sampler2D maskTex;
uniform sampler2D numTex;

// The depth at which we want to focus
uniform float focalDepth = 0.93;

// A scale factor for the radius of blur
uniform float blurRadius = 0.03;

// We pass the window size to the shader.
uniform vec2 windowSize;

// We pass the window size to the shader.
uniform int score[5];
uniform int scoreLength;
uniform vec2 scorePlacement;
uniform vec2 numDimensions;

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
    float blurredB = PoissonFilter(tex, vec2(UVpos.x+offset,UVpos.y),sigma).b;

    // final color
    return vec4(blurredR, blurredG, blurredB, 1);
}

bool isInside(vec2 pixelCoord, vec2 rectCoord, vec2 rectDimensions)
{
    if(pixelCoord.x>rectCoord.x &&
       pixelCoord.x<rectCoord.x+rectDimensions.x &&
       pixelCoord.y>rectCoord.y &&
       pixelCoord.y<rectCoord.y+rectDimensions.y)
    {
        return true;
    }
    else {return false;}
}

void main() {


    // Determine the texture coordinate from the window size
    vec2 texPos = gl_FragCoord.xy / windowSize;

    // the new scaled size of the game
    float gameScale = 0.9;
    // The desired viewport ratio * the scale
    vec2 gameSize = vec2(0.6,1)*gameScale;

    //-------------------Game Viewport------------------
    // we want a bit of a border between the edge of the screen
    // and the game viewport
    vec2 gameOffset = vec2(0.15,-0.05);

    // texture coordinates for drawing within the game viewport
    vec2 gamePos = texPos*(1+(1-gameScale))+gameOffset;

    // Determine sigma, the blur radius of this pixel
    float sigma = abs(focalDepth - texture(depthTex, gamePos).x) * blurRadius;

    // chromatic abberation offset
    // (weighted for depth, otherwise it looks like there's
    // two projectiles in the distance where there's only one)
    float gameCAOffset = 0.001*(pow(texture2D(depthTex, gamePos).r,3));

    // final color of the game viewport
    vec4 gameColor = ChromaticAbberationBlur(gameTex, gamePos, sigma, gameCAOffset);
    //--------------------------------------------------

    //----------------------Game UI---------------------
    // the chromatic abberation offset for our in-game UI
    float flatCAOffset = 0.001;

    // because the origin on SDL_Surfaces is the top-left corner
    // and without this everything is upside down
    vec2 SDL_SurfaceSize = windowSize;
    SDL_SurfaceSize.y = 1-windowSize.y;

    // the coordinates of our text surface
    vec2 textPos = gl_FragCoord.xy / SDL_SurfaceSize;

    vec4 textColor = ChromaticAbberationBlur(textTex, textPos, 0.001, flatCAOffset);
    vec4 maskColor = ChromaticAbberationBlur(maskTex, textPos, 0.005, flatCAOffset);

    //vec4 numColor = vec4(vec3(texture2D(numTex, textPos).rgb),1);

    gameColor *= maskColor;
    vec4 outColor = gameColor+textColor;

    vec4 numColor = ChromaticAbberationBlur(numTex, textPos, 0.001, flatCAOffset);

    vec2 _scorePlacement = scorePlacement/windowSize;
    vec2 _numDimensions = numDimensions/windowSize;
    _scorePlacement.y = 1-_scorePlacement.y-_numDimensions.y;
    _scorePlacement.x -= 0.04;

    for(int i; i<scoreLength; i++)
    {
        vec2 numPlacement = vec2(_scorePlacement.x+_numDimensions.x*i,_scorePlacement.y);

        if(isInside(texPos, numPlacement, _numDimensions))
        {
            vec2 translation = textPos - vec2(numPlacement.x,(1-_numDimensions.y)+numPlacement.y);
            vec2 numPos = vec2(translation.x+_numDimensions.x*score[i],(translation.y)+2*numPlacement.y);
            outColor = ChromaticAbberationBlur( numTex, vec2(numPos.x,numPos.y), 0.001, flatCAOffset );
        }
    }
    //--------------------------------------------------

    fragColor = outColor;

}

