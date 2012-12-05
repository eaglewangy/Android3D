precision highp float;

uniform mat4 u_mvpMatrix;
uniform mat4 u_mvMatrix;
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
	if(u_enableTexture == 1) {                  
	    v_texCoord = a_texCoord;                  
	}                                            
	if (u_enableVertexColor == 1){             
	    v_color = a_vcolor;                       
	}
	float factor = 1.0;
	if (u_enableLight == 1){
	    //vec3 N = u_mvMatrix * a_normal; 
	    //vec3 L = u_lightDir.xyz;
	    //factor = max(dot(N, L), 0.0);
	}else{
	    factor = 1.0;
	}
	
	v_color = factor * a_vcolor;
	v_color.a = a_vcolor.a; 
}                                             