precision mediump float;

uniform lowp int u_enableTexture;                      
uniform lowp int u_enableVertexColor;                 
uniform sampler2D s_texture;
uniform lowp int u_enableLight;

varying vec4 v_color;                       
varying vec2 v_texCoord;                               
varying float v_colorfactor; 

void main() {                              
    if (u_enableTexture == 1) {
        vec4 s = texture2D(s_texture, v_texCoord);
        gl_FragColor = s;        
	}                                                 
	else if (u_enableVertexColor == 1) {
	    gl_FragColor = v_color ;
	}                                                  
	else {                                             
	    gl_FragColor = vec4(0.0, 0.0, 1.0, 0.0);      
	}                                                
}                                                  