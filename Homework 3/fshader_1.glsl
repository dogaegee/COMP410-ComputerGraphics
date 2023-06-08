#version 410

in  vec4 color;
in  vec2 texCoord;
uniform int TextureFlag;
out vec4 fcolor;

uniform sampler2D tex;

void main() 
{ 
    if (TextureFlag == 1)
        //sample a texture color from texture object
        fcolor = texture(tex, texCoord);

    else fcolor = color;
}