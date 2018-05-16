#version 330 core

#extension GL_ARB_separate_shader_objects : enable

// this is a pointer to the current 2D texture object
//uniform sampler2D tex;
// the vertex UV

uniform bool isGrid;

in vec2 vertUV;
out vec4 outColour;
void main()
{
 if(isGrid) { outColour = vec4(0.0,1,0,1); }
 // set the fragment colour to the current texture
 else { outColour = vec4(0.0,0.0,0.0,1); }
}
