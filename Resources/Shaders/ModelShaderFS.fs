precision mediump float;

varying vec2 v_uv; 
uniform sampler2D u_texture; 

void main()
{
    vec4 color = texture2D(u_texture, v_uv); 
    if (color.a<0.1) discard;
    gl_FragColor = color; 
    //gl_FragColor = vec4(1.0,1.0,1.0,1.0);
}