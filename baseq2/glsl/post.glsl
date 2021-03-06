
uniform vec4 waterwarpParam;

// common
INOUTTYPE vec4 texcoords[2];

#ifdef VERTEXSHADER
uniform mat4 orthomatrix;

layout(location = 0) in vec4 position;
layout(location = 2) in vec4 texcoord;


void PostVS ()
{
	gl_Position = orthomatrix * position;
	texcoords[0] = texcoord;
	texcoords[1] = ((texcoord * waterwarpParam.y) + waterwarpParam.x) * waterwarpParam.z * waterwarpParam.w;
}
#endif


#ifdef FRAGMENTSHADER
uniform sampler2D diffuse;
uniform sampler2D precomposite;
uniform sampler2D warpgradient;
uniform sampler2D lumTex;
uniform sampler2D AOTex;

uniform float exposure;
uniform int waterwarppost;

uniform vec4 surfcolor;
uniform vec4 brightnessContrastBlurSSAOAmount;
uniform vec2 rescale;
uniform vec2 texScale;

#define USE_TONEMAP						1
float Uncharted2WhitePoint = 11.2; // linear white point
	
vec3 ACESFilmRec2020( vec3 x )
{
    float a = 15.8f;
    float b = 2.12f;
    float c = 1.2f;
    float d = 5.92f;
    float e = 1.9f;

    return ( x * ( a * x + b ) ) / ( x * ( c * x + d ) + e );
}

vec3 ToneMap(vec3 c, float avglum)
{
#if 1
	// calculate basic exposure value
	vec3 exposedColor = c * (exposure / avglum);
#else
	// calculation from: Perceptual Effects in Real-time Tone Mapping - Krawczyk et al.
	float hdrKey = 1.03 - (2.0 / (2.0 + (avglum + 1.0f)));

	// calculate exposure from geometric mean of luminance
	float avgLuminance = max( avglum, 0.001 );
	float linearExposure = ( hdrKey / avgLuminance );
	float newExposure = log2( max( linearExposure, 0.0001 ) );
	
	// exposure curves ranges from 0.0625 to 16.0
	vec3 exposedColor = exp2( newExposure ) * c.rgb;
#endif
	
	float exposureBias = 1.0;
	vec3 curr = ACESFilmRec2020( exposedColor * exposureBias );
	vec3 whiteScale = 1.0 / ACESFilmRec2020( vec3( Uncharted2WhitePoint ) );
	
	return curr * whiteScale;
}

#define USE_VIGNETTE						1

#define USE_FILMGRAIN						1
void FilmgrainPass( inout vec4 color )
{
	vec2 uv = gl_FragCoord.st * texScale;
    float strength = 16.0;
    
    float x = (uv.x + 4.0) * (uv.y + 4.0) * (waterwarpParam.x * 10.0);
	vec4 grain = vec4(mod((mod(x, 13.0) + 1.0) * (mod(x, 123.0) + 1.0), 0.01)-0.005) * strength;
    
   	grain = 1.0 - grain;
	color *= grain;
}

out vec4 fragColor;

void PostFS ()
{
	vec2 st = gl_FragCoord.st * texScale;
	vec4 scene;
	
	// mix in water warp post effect
	if (waterwarppost == 1)
	{
		vec4 distort1 = texture (warpgradient, texcoords[1].yx);
		vec4 distort2 = texture (warpgradient, texcoords[1].xy);
		vec2 warpcoords = texcoords[0].xy + (distort1.ba + distort2.ab);

		scene = texture(precomposite, warpcoords * rescale);
	}
	else
	{
		scene = texture(precomposite, st);
	}
	
	// multiply scene with ambient occlusion
	if (brightnessContrastBlurSSAOAmount.w > 0)
	{
		vec4 AOScene = texture(AOTex, st);
		scene *= AOScene;
	}
	
	// then mix in the previously generated bloom
	vec4 hdrScene = texture(diffuse, st);
	vec4 color = mix(scene, hdrScene, brightnessContrastBlurSSAOAmount.z);

	// vignette effect
#if USE_VIGNETTE
	vec2 vignetteST = st;
    vignetteST *= 1.0 - vignetteST.yx;   
    float vig = vignetteST.x * vignetteST.y * 15.0;
    vig = pow(vig, 0.25);
	color.rgb *= vig;
#endif

	// tonemap using filmic tonemapping curve
#if USE_TONEMAP
	vec3 luminance = texture(lumTex, vec2(0.0, 0.0)).rgb;
	color.rgb = ToneMap(color.rgb, luminance.r);
#endif

	// film grain effect
#if USE_FILMGRAIN
	FilmgrainPass(color);
#endif

	// mix scene with possible modulation (eg item pickups, getting shot, etc)
	color = mix(color, surfcolor, surfcolor.a);

	// brightness
	color.rgb = doBrightnessAndContrast(color.rgb, brightnessContrastBlurSSAOAmount.x, brightnessContrastBlurSSAOAmount.y);
	
	// send it out to the screen
	fragColor = color;
}
#endif
