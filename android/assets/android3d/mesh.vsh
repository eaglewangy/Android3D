precision highp float;

uniform mat4 u_mvpMatrix;
uniform mat3 u_normalMatrix;
uniform lowp int u_enableTexture;               
uniform lowp int u_enableVertexColor;  
uniform vec4 u_lightDir;                     
uniform lowp int u_enableLight;  

attribute vec4 vPosition;                  
attribute vec2 a_texCoord;            
attribute vec3 a_normal;             
attribute vec4 a_vcolor;                        

varying vec4 v_color;              
varying vec2 v_texCoord;               
varying float v_colorfactor;

void main() {                               
    gl_Position = u_mvpMatrix * vPosition;       
	
	if (u_enableLight == 1){
	    vec3 N = u_normalMatrix * a_normal; 
	    vec3 L = u_lightDir.xyz;
	    v_colorfactor = max(dot(N, L), 0.0);
	}else{
	    v_colorfactor = 1.0;
	}
	
	if(u_enableTexture == 1) {                  
	    v_texCoord = a_texCoord;             
	}                                            
	else if (u_enableVertexColor == 1){             
	    v_color = v_colorfactor * a_vcolor;                       
	}
	else{
	    v_color = v_colorfactor * vec4(0.0, 0.0, 1.0, 0.0); 
	}
	
	//v_color = v_colorfactor * a_vcolor;
	v_color.a = a_vcolor.a; 
}                                             