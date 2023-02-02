#version 430


const float c_E                     = 2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427; ///< Euler's number
const float c_Pi                    = 3.1415926535897932384626433832795028841971693993751;
const float c_2_Pi                  = 6.233185307179586476925286766559005768394338798750211641949889184615632812572417997256069650684234135;
const float c_Pi_Over_180           = 0.017453292519943295769236907684886;
const float c_Pi_Over_2             = 1.570796326794896619231321691639751442098584699687552910487472296153908203143104499314017412671058533;
const float c_Pi_Over_4             = 0.7853981633974483096156608458198757210492923498437764552437361480769541015715522496570087063355292669;
const float c_Pi_Over_360           = 0.00872664625997164788461845384244;
const float c_180_Over_Pi           = 57.295779513082320876798154814105;
const float c_Log_E                 = 0.4342944819032518276511289189166050822943970058036665661144537831658646492088707747292249493384317483;
const float c_Ln_10                 = 2.302585092994045684017991454684364207601101488628772976033327900967572609677352480235997205089598298;
const float c_Sqrt_2                = 1.4142135623730950488016887242096980785696718753769;
const float c_Sqrt_3                = 1.7320508075688772935274463415058723669428052538104;
const float c_Log_E_2               = 0.693147180559945309417232121458176568075500134360255;                                                 ///< log of 2 to basis e
const float c_Golden                = 1.618034;
const float c_Tiny                  = 1.e-8;            ///< Smallest float possible
const float c_Small                 = 1.e-6;            ///< Small enough float to use as delta value

float GetDiffAngle( in vec3 v1, in vec3 v2 ){
	return acos( dot( v1, v2 ) );
}

float Sqr( in float fX ){
	return fX * fX;
}



in vec3 normal;
flat in vec3 flat_normal;
in vec3 position;
in float visibility;

uniform vec3 u_color;
uniform vec3 u_ambient;
uniform vec3 u_diffuse;
uniform vec3 u_specular;
uniform float u_shininess;


uniform float u_alpha = 1.0;

uniform bool solid = false;

uniform bool u_flat_shading = false;

//Ligth integration
struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS_MAX 4
uniform PointLight pointLights[NR_POINT_LIGHTS_MAX];
uniform int nb_pointlight;


out vec4 FragColor;


vec3 CalcSpecularPhong(PointLight  vLightDef, vec3 vLightVec,vec3 vNormal, vec3 vVertex ){
	vec3 vReflected = reflect( vLightVec, vNormal );
	float fSpecularIntensity = pow( max( dot( vReflected, vVertex ), 0.0 ), u_shininess);
	return fSpecularIntensity * (vLightDef.specular * u_specular);
}

vec3 CalcSpecularBlinn(PointLight  vLightDef,  vec3 vLightVec,vec3 vNormal, vec3 vVertex )
{
	vec3 vHalf = normalize( ( vLightVec + -vVertex ) * 0.5 );
	float fSpec = pow( max( dot( vNormal, vHalf ), 0.0 ), u_shininess );
	return fSpec * ( vLightDef.specular * u_specular );
}

vec3 CalcSpecularGaussian( PointLight  vLightDef, vec3 vLightVec, vec3 vNormal, vec3 vVertex ){
	vec3 vHalf = normalize( ( vLightVec + -vVertex ) * 0.5 );
	//float fFraction = (GetDiffAngle(vNormal,vHalf)) / fSurfaceSmoothness;
	float fFraction = ( GetDiffAngle( vNormal, vHalf ) ) / ( u_shininess * 0.001f );
	float fSpec = max( pow( c_E, - Sqr( fFraction ) ), 0.0 );
	return fSpec * ( vLightDef.specular * u_specular );
}

vec3 CalcSpecularBeckmann(PointLight  vLightDef, vec3 vLightVec,vec3 vNormal,vec3 vVertex ){
	vec3 vHalf = normalize( ( vLightVec + -vVertex ) *  0.5 );
	float fCosTerm = dot( vNormal, vHalf );
	float fSurfaceSmoothness = ( u_shininess * 0.001f );
	float fDivisor = 4.0 * Sqr( fSurfaceSmoothness ) * pow( fCosTerm, 4.0 );
	float fTan = tan( GetDiffAngle( vNormal, vHalf ) );
	float fExponent = Sqr( fTan / fSurfaceSmoothness );
	float fSpec = max( ( ( pow( c_E, -fExponent ) ) / fDivisor ), 0.0 );
	return fSpec * ( vLightDef.specular * u_specular );
}

vec3 CalcSpecularCookTorrance(PointLight  vLightDef, vec3 vLightVec, vec3 vNormal, vec3 vVertex ){
	// Lambda for the Fresnel-Term
	float fLambda = 0.8;		// Kind of the offset for the size

	vec3 E = -vVertex;
	float fEDotN = dot( E, vNormal );
	vec3 vHalf = normalize( vLightVec - vVertex );
	float fNDotH = dot( vNormal, vHalf );
	float fSurfaceSmoothness = ( u_shininess * 0.0005 );
	
	// calculate D:
	float fDivisor = 4.0 * Sqr( fSurfaceSmoothness ) * Sqr( Sqr( fNDotH ) );
	float fExponent = tan( GetDiffAngle( vNormal, vHalf ) ) / fSurfaceSmoothness;
	fExponent *= fExponent;
	float fD = ( pow( c_E, -fExponent ) ) / fDivisor;
	
	// calculate F (using the Schlick-term), several differnt ones:
	float fF = fLambda + ( ( 1.0 - fLambda ) * max( pow( 1.0 - dot( E, vHalf ), 5.0 ), 0.0 ) );
	
	//fF = pow((1.0 + fEDotN), fLambda);
	//fF = fLambda;							// Simplest
	
	// calculate G:
	float fEDotH_Rec = 1.0 / dot( E, vHalf );
	float fG1 = 2.0 * fNDotH * fEDotN * fEDotH_Rec;
	float fG2 = 2.0 * fNDotH * ( max( dot( vLightVec, vNormal ), 0.0 ) ) * fEDotH_Rec;
	float fG = min( max( min( fG1, fG2 ), 0.0 ), 1.0);
	
	// all in all:
	float fSpec = ( fD * fF * fG ) / ( fEDotN );
	fSpec = max( fSpec, 0.f );
	return fSpec * ( vLightDef.specular * u_specular );
}

vec3 CalcSpecularSchlick( PointLight  vLightDef,vec3 vLightVec,vec3 vNormal,vec3 vVertex ){
	vec3 vRefl = reflect( vLightVec, vNormal );
	float fT = max( dot( vRefl, vVertex ), 0.0 );
	float fSpec = max( fT / ( u_shininess - u_shininess * fT + fT ), 0.0 );
	return fSpec * ( vLightDef.specular * u_specular );
}

void main (void) {

    vec3 m_normal = u_flat_shading ? flat_normal : normal;

    float alpha = 1.;
    if(visibility > 0.) discard;
    
    //Avec le tri des éléments
    //if(visibility > 0.) alpha = u_alpha;

	
    vec3 color = vec3(0,0,0);
    
    if(solid) {
	color = vec3(u_color);	
    } else {
	
	//vec3 N_Normed = normalize(normal); // Normal
	//vec3 V_Normed = normalize(position); //Position
	
	//for(int i = 0; i<nb_pointlight; i++){
	//	PointLight cur_light = pointLights[i];
	//	vec3 vLightDir 	= cur_light.position.xyz - position;
	//	vec3 vLightDir_Normed = normalize(vLightDir);
   				
//LIGHT ATTENUATION
	//	float distance    = length(vLightDir);
	//	float fAttenuation = 1.0 / (cur_light.constant + cur_light.linear * distance + cur_light.quadratic * (distance * distance));		
		
//AMBIENT PART
	//	vec3 vAmbient = cur_light.ambient * u_ambient;
		
//DIFFUSE PART
	//	float fLambert = max( dot( vLightDir_Normed , N_Normed), 0.0 );
	//	vec3 vDiffuse = fLambert * (cur_light.diffuse * u_diffuse );
		
//SPECULAR PART
	//	vec3 vSpecular = vec3(0.0, 0.0, 0.0);
	//	if (u_shininess > 0.00001) {	
			//vSpecular = CalcSpecular( cur_light, vLightDir_Normed, N_Normed, V_Normed );
			//vSpecular = CalcSpecularCookTorrance( cur_light, vLightDir_Normed, N_Normed, V_Normed );
		
			//vSpecular = CalcSpecularBlinn( cur_light, vLightDir_Normed, N_Normed, V_Normed );
			//vSpecular = CalcSpecularWarren( cur_light, vLightDir_Normed, N_Normed, V_Normed );
	//		vSpecular = CalcSpecularSchlick( cur_light, vLightDir_Normed, N_Normed, V_Normed );
	//	}
	//	color += (fAttenuation * (vAmbient + vDiffuse + vSpecular));
	//}

	//color = vec4(position,1.0);

        /*
        PointLight cur_light = pointLights[0];
	
	vec3 N_Normed = normalize(normal); // Normal
	vec3 V_Normed = normalize(position); //Position

	vec3 vLightDir = cur_light.position - position;
	vec3 vLightDir_Normed = normalize(vLightDir);
	//color = vLightDir;
	
	//if(vLightDir_Normed.y>0){
	//	color = vec3(vLightDir_Normed.y,0,0);
	//}else{
	//	color = vec3(0,1,0);
	//}

	//float distance = length(vLightDir);
	//float fAttenuation = 1.0 / (cur_light.constant + cur_light.linear * distance + cur_light.quadratic * (distance * distance));		

        vec3 vAmbient = u_color * u_ambient;
	float fLambert = max( dot( vLightDir_Normed , N_Normed), 0.0 );
        vec3 vDiffuse = fLambert * (cur_light.diffuse * u_diffuse * u_color);
	
	vec3 vSpecular = CalcSpecularBlinn( cur_light, vLightDir_Normed, N_Normed, V_Normed );
	
	//color = cur_light.position.xyz;
	//color = vSpecular;
	//color = vec3(fAttenuation,0,0);
        //color += (vAmbient + vDiffuse + vSpecular);
        color += (vAmbient + vDiffuse);

        */

        for(int i = 0; i<nb_pointlight; i++) {
            PointLight cur_light = pointLights[i];

            vec3 N_Normed = normalize(m_normal); // Normal
            vec3 V_Normed = normalize(position); //Position

            vec3 vLightDir = cur_light.position - position;

            vec3 vLightDir_Normed = normalize(vLightDir);

            vec3 vAmbient = u_color * u_ambient * cur_light.ambient;
            float fLambert = max(dot( vLightDir_Normed , N_Normed), 0.0f);
            vec3 vDiffuse = fLambert * (cur_light.diffuse * u_diffuse * u_color);

            color += vAmbient + vDiffuse;
        }

        //color = color / (color + vec3(1.0));
        color = vec3(1.0) - exp(-color * 1.5);
        color = pow(color, vec3(2.2));
    }
	
    FragColor = vec4(color, 1.0);
} 
