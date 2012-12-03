precision mediump float;                              
varying vec4 v_color;                                 
uniform lowp int u_enableTexture;                      
uniform lowp int u_enableVertexColor;                 
uniform sampler2D s_texture;                          
varying vec2 v_texCoord;                               
varying float v_colorfactor;                          

void main() {                                           
    if (u_enableTexture == 1) {                         
	    gl_FragColor = texture2D(s_texture, v_texCoord);
	}                                                 
	else if (u_enableVertexColor == 1) {              
	    gl_FragColor = v_color;                         
	}                                                  
	else {                                             
	    gl_FragColor = vec4(0.0, 0.0, 1.0, 0.0);      
	}                                                
}                                                  