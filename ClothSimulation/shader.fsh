#version 330 core
out vec4 FragColor;  
in vec2 TexCoords;
uniform sampler2D text;
uniform int enable;
void main()
{
    if (enable==0)
     FragColor = vec4(0.5,0.5,0.5, 1.0);
    else 
     FragColor = texture(text, TexCoords) ;
}