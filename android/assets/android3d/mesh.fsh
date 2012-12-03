precision mediump float;

struct DirectionalLight {
    vec3 direction;
    vec3 halfplane;
    vec4 ambientColor;
    vec4 diffuseColor;
    vec4 specularColor;
};

struct Material {
    vec4 ambientFactor;
    vec4 diffuseFactor;
    vec4 specularFactor;
    float shininess;
};                   
                                
uniform lowp int u_enableTexture;                      
uniform lowp int u_enableVertexColor;                 
uniform sampler2D s_texture;
uniform lowp int u_enableLight;
// Light
uniform DirectionalLight u_directionalLight;
// Material
uniform Material u_material;

varying vec4 v_color;                       
varying vec2 v_texCoord;                               
varying float v_colorfactor; 
varying vec3 v_ecNormal;                         

vec4 compute_light(vec3 normal, DirectionalLight dl, Material material)
{
    // Normalize v_ecNormal
    vec3 ecNormal = normal / length(normal);
    float ecNormalDotLightDirection = max(0.0, dot(ecNormal, dl.direction));
    float ecNormalDotLightHalfplane = max(0.0, dot(ecNormal, dl.halfplane));
    // Calculate ambient light
    vec4 ambientLight = dl.ambientColor * material.ambientFactor;
    // Calculate diffuse light
    vec4 diffuseLight = ecNormalDotLightDirection * dl.diffuseColor * material.diffuseFactor;
    // Calculate specular light
    vec4 specularLight = vec4(0.0);
    if (ecNormalDotLightHalfplane > 0.0) {
        specularLight = pow(ecNormalDotLightHalfplane, material.shininess) * dl.specularColor * material.specularFactor;
    } 
    vec4 light = ambientLight + diffuseLight + specularLight;
     
    return light;
}

void main() {
    vec4 light = vec4(1.0, 1.0, 1.0, 1.0);
    if (u_enableLight == 1){
        light = compute_light(v_ecNormal, u_directionalLight, u_material);
    }
    //light = vec4(1.2, 3.4, 1.0, 0.9);                                
    if (u_enableTexture == 1) {
        vec4 s = texture2D(s_texture, v_texCoord);
        gl_FragColor = s * light;        
	}                                                 
	else if (u_enableVertexColor == 1) {
	    gl_FragColor = v_color * light;
	}                                                  
	else {                                             
	    gl_FragColor = vec4(0.0, 0.0, 1.0, 0.0) * light;      
	}                                                
}                                                  