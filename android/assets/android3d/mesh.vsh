precision highp float;

uniform mat4 u_mvpMatrix;
uniform mat4 u_mvMatrix;
uniform lowp int u_enableTexture;               
uniform lowp int u_enableVertexColor;                       
uniform lowp int u_enableLight;  

attribute vec4 vPosition;                  
attribute vec2 a_texCoord;            
attribute vec3 a_normal;             
attribute vec4 a_vcolor;                        

varying vec4 v_color;              
varying vec2 v_texCoord;               
varying float v_colorfactor;
varying vec3 v_ecNormal;              

void main() {                               
    gl_Position = u_mvpMatrix * vPosition;       
	if(u_enableTexture == 1) {                  
	    v_texCoord = a_texCoord;                  
	}                                            
	if (u_enableVertexColor == 1){             
	    v_color = a_vcolor;                       
	}
	if (u_enableLight == 1){
	    vec3 mcNormal = a_normal;
        // Calculate and normalize eye space normal
        vec3 ecNormal = vec3(u_mvMatrix * vec4(mcNormal, 0.0));
        ecNormal = ecNormal / length(ecNormal);
        v_ecNormal = ecNormal;
	}                   
}                                             