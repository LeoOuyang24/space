#version 330 core

out vec4 FragColor;
// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;


void main()
{
    float dist = length(fragTexCoord - vec2(0.5,0.5));

    FragColor = mix(vec4(1,1,1,1),vec4(0,0,0,0),min(1.0,max(0.0,dist)));
}
