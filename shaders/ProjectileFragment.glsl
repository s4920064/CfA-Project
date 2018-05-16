#version 330 core

uniform bool state;
uniform bool damaged;

// the vertex UV
in vec2 vertUV;
out vec4 outColour;
void main()
{
    if(damaged) { outColour = vec4(1,0,0,1); }
    else
    {
        if(state)
        { outColour = vec4(1,0,1,1); }
        else { outColour = vec4(0,1,1,1); }
    }
}
