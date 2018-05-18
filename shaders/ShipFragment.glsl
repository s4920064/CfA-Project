#version 330 core

uniform vec4 color;

// the vertex UV
in vec2 vertUV;
out vec4 outColour;
void main()
{
    outColour = color;
}
