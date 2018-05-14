#version 330 core

//#extension GL_ARB_separate_shader_objects : enable

uniform bool state;

// the vertex UV
in vec2 vertUV;
out vec4 outColour;
void main()
{
    if(state) { outColour = vec4(1,0,0,1); }
    else { outColour = vec4(0,0,1,1); }
}
