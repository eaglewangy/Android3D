uniform mat4 u_MVPMatrix;
attribute vec4 vPosition;                  
attribute vec2 a_texCoord;            
attribute vec3 a_normal;                        
varying vec2 v_texCoord;                        
attribute vec4 a_vcolor;                        
varying vec4 v_color;                          
uniform lowp int u_enableTexture;               
uniform lowp int u_enableVertexColor;           
uniform mat3 u_normalMatrix;                   
uniform vec4 u_lightDir;                       
uniform lowp int u_enableLight;                
varying float v_colorfactor;                    

void main() {                               
    gl_Position = u_MVPMatrix * vPosition;       
	if(u_enableTexture == 1) {                  
	    v_texCoord = a_texCoord;                  
	}                                            
	if (u_enableVertexColor == 1){             
	    v_color = a_vcolor;                       
	}                                            

    vec3 N = u_normalMatrix * a_normal;
	vec3 L = u_lightDir.xyz;                    
	if(u_enableLight == 1) {                    
	    v_colorfactor = max(dot(N, L), 0.0);
	} else {        
	    v_colorfactor = 1.0;                       
	}                                            
	v_color = v_colorfactor * a_vcolor;           
    v_color.a = a_vcolor.a;                       
}                                             