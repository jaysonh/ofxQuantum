// Uniforms sent from main openframworks app
// This shader was created by combining various examples from shadertoy together

uniform vec3  iResolution;              // Resolution of openframeworks app
uniform float iGlobalTime;              // Time the app has been running for
uniform vec4  iMouse;                   // Mouse positiom
uniform float iState;
uniform sampler2D iChannel0;            // Texture #1
uniform sampler2D iChannel1;            // Texture #2
uniform sampler2D iChannel2;            // Texture #3
 
vec4 spect;
vec4 spectPlane;

int spherePlane;

// Value Noise by IQ
#define USE_PROCEDURAL
#ifdef USE_PROCEDURAL
float hash( float n ) { return fract(sin(n)*43758.5453123); }


float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    
    float n = p.x + p.y*157.0 + 113.0*p.z;
    return mix(mix( mix( hash(n+  0.0), hash(n+  1.0),f.x),
                    mix( hash(n+157.0), hash(n+158.0),f.x),f.y),
               mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                   mix( hash(n+270.0), hash(n+271.0),f.x),f.y),f.z);
}

#else
float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);
    f = f*f*(3.0-2.0*f);
    
    vec2 uv = (p.xy+vec2(37.0,17.0)*p.z) + f.xy;
    vec2 rg = texture2D( iChannel1, (uv+0.5)/256.0, -100.0 ).yx;
    return mix( rg.x, rg.y, f.z );
}
#endif

float fbm(vec3 uv)
{
    float f;
    mat3 m = mat3( 0.00,  0.80,  0.60,
                    -0.80,  0.36, -0.48,
                    -0.60, -0.48,  0.64 );
    f  = 0.5000*noise( uv ); uv = m*uv*2.01;
    f += 0.2500*noise( uv ); uv = m*uv*2.02;
    f += 0.1250*noise( uv ); uv = m*uv*2.03;
    f += 0.0625*noise( uv ); uv = m*uv*2.01;
//  f = 0.5 + 0.5*f;
    return f;
}

// Distance Functions for Raymarching
float sdSphere( vec3 p, float s )
{
    return length(p)-s;
}

float sdPlane( vec3 p )
{
    return p.y;
}

float opU( float d1, float d2 )
{
    return (d1<d2) ? d1 : d2;
}

// 0 == none 1 == sphere 2 == plane
float map( in vec3 pos, in bool saveShape )
{
    
    float deformPlane  = spectPlane.x*fbm( pos + spectPlane.y * fbm(( spect.z+ 5.)*pos)) * 1.0;
    float deformSphere = spect.x*fbm( pos + spect.y * fbm(( spect.z+ 5.)*pos)) * 0.25;

   
    float planeHeight = 1.14;
    float sphereSize  = 0.25;

    float dPlane  = sdPlane(pos+vec3(0.,0.5,0.)) + planeHeight *deformPlane;
    float dSphere = sdSphere( pos, sphereSize ) - deformSphere;

    float a = dPlane;
    float b = dSphere;
    
    return min(dPlane,dSphere);
}

float mapShape(in vec3 pos, in bool saveShape )
{
    float deformPlane  = spectPlane.x*fbm( pos + spectPlane.y * fbm(( spect.z+ 5.)*pos)) * 1.0;
    float deformSphere = spect.x*fbm( pos + spect.y * fbm(( spect.z+ 5.)*pos)) * 0.25;

   
    float planeHeight = 1.14;
    float sphereSize  = 0.15;

    float dPlane  = sdPlane(pos+vec3(0.,0.5,0.)) + planeHeight *deformPlane;
    float dSphere = sdSphere( pos, sphereSize ) - deformSphere;

    if(dPlane < dSphere)
    {
        spherePlane =1;
        return dPlane;
    }
    else
    {
        spherePlane = 2;
       return dSphere;
    }
}

vec3 calcNormal( in vec3 pos )
{
    vec3 eps = vec3(0.001,0.0,0.0);
    return normalize( vec3(
        map(pos+eps.xyy,false) - map(pos-eps.xyy,false),
        map(pos+eps.yxy,false) - map(pos-eps.yxy,false),
        map(pos+eps.yyx,false) - map(pos-eps.yyx,false) ) );
}

// Raymarching
bool raymarch(vec3 origin, vec3 dir, out float dist, out vec3 norm)
{
    float epsilon = 0.003;
    float maxdist = 10.0;
    float marched = 0.0;
    float delta = 2.*epsilon;
    dist = -1.0;
    for (int steps=0; steps < 60; steps++)
    {
        if ( ( abs(delta) < epsilon ) || (marched > maxdist) ) continue;
        marched += delta;
        delta = mapShape(origin + marched * dir,true);
    }
    bool res = false;
    if (marched < maxdist)
    {
        norm = calcNormal(origin + marched * dir);
        dist = marched;
        res = true;
    }
    return res;
}

float calcAO( in vec3 pos, in vec3 nor )
{
    float totao = 0.0;
    float sca = 1.0;
    for( int aoi=0; aoi<5; aoi++ )
    {
        float hr = 0.01 + 0.05*float(aoi);
        vec3 aopos =  nor * hr + pos;
        float dd = map( aopos ,false);
        totao += -(dd-hr)*sca;
        sca *= 0.75;
    }
    return clamp( 1.0 - 4.0*totao, 0.0, 1.0 );
}

vec3 render( in vec3 ro, in vec3 rd )
{ 
    vec3 col = vec3(0.0);
    float dist = 0.;
    vec3 nor = vec3(0.,0.,0.);
    //
    vec3 lig = normalize( vec3(0.6, 0.7, -0.5) );

    vec3 sky = vec3(0.32,0.36,0.4) - rd.y*0.4;
    float sun = clamp( dot(rd,lig), 0.0, 1.0 );

    sky += vec3(1.0,0.8,0.4)*0.5*pow( sun, 10.0 );
    sky *= 0.9;
    //
    if ( raymarch(ro, rd, dist, nor) )
    {
        vec3 pos = ro + dist*rd;

        col = vec3(0.75);
        
        float ao = 1.0;//calcAO( pos, nor );

        float amb = clamp( 0.5+0.5*nor.y, 0.0, 1.0 );
        float dif = clamp( dot( nor, lig ), 0.0, 1.0 );
        float bac = clamp( dot( nor, normalize(vec3(-lig.x,0.0,-lig.z))), 0.0, 1.0 )*clamp( 1.0-pos.y,0.0,1.0);

        // need to decide if it is plane or sphere here...
        vec3 materialCol = vec3(1.0, 0.0, 1.0);

        if(spherePlane == 1)
            materialCol = vec3(0.0,1.0,0.0);
        else if(spherePlane == 2)
            materialCol = vec3(1.0 * iState,1.0 * iState,1.0 * iState);

        vec3 brdf = vec3(0.0);
        brdf += 1.20*amb*vec3(0.20,0.22,0.26)*ao;
        brdf += 0.20*bac*vec3(0.15,0.15,0.15)*ao;
        brdf += 1.00*dif*materialCol;

        float pp = clamp( dot( reflect(rd,nor), lig ), 0.0, 1.0 );
        float spe = pow(pp,16.0);
        float fre = ao*pow( clamp(1.0+dot(nor,rd),0.0,1.0), 2.0 );

        vec3 radiationCol = vec3(1.0, 1.0, 1.0);
        col = col*brdf + vec3(0.8) * radiationCol * spe + 0.2*fre*(0.5+0.5*col);
        col = mix( col, sky, 1.0-exp(-0.0025*dist*dist*dist) );
    }
    else
    {
        col = sky;
    }

    return vec3( clamp(col,0.0,1.0) );
}

void main ( void )
{   
  

    spect.x = 1.5 * noise(vec3(0.0,0.0,1.0));
    spect.y = 1.  * noise(vec3(1.0,0.0,0.0));
    spect.z = 1.5 * noise(vec3(0.0,1.0,0.0));
    spect.w = noise(vec3(0.0,0.0,0.0));
   
    spectPlane = vec4(1.0 * noise(vec3( iGlobalTime*0.015)), 0.0,0.0,0.0);

    vec2 q = gl_FragCoord.xy/iResolution.xy;
    vec2 p = -1.0+2.0*q;
    p.x *= iResolution.x/iResolution.y;
         
    float Time = 0.05*(15.0 + iGlobalTime) - 2.*spect.w;

    // camera   
    //vec3 ro = vec3( 1.0 + 1.0* sin(Time*0.5) , 0.5, 1.0 + 1.0* (sin(Time) ));
    vec3 ro = vec3( 1.0, -0.15, 1.0*sin(Time*0.25));
    vec3 ta = vec3( 0.0, 0.0, 0.0 );
    
    // camera tx
    vec3 cw = normalize( ta-ro );
    vec3 cp = vec3( 0.0, 1.0, 0.0 );
    vec3 cu = normalize( cross(cw,cp) );
    vec3 cv = normalize( cross(cu,cw) );
    vec3 rd = normalize( p.x*cu + p.y*cv + 2.5*cw );

    vec3 col = render( ro, rd );
    
    gl_FragColor=vec4( clamp(col,0.,1.), 1.0 );
}