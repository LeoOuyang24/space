#version 330 core

//shader for the gravity field of planets and star background
//used with gravityVertexShader

#define M_PI 3.1415926535897932384626433832795

out vec4 FragColor;
// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec4 centerColor;
uniform vec4 borderColor;

uniform int time;


void main()
{


    float sunRadius = 0.1;



    float dist = (length(fragTexCoord - vec2(0.5,0.5)) - 0.05)/(0.5); //find the distance between interpolated pixel and center
    //float angle = atan(fragTexCoord.y - 0.5, fragTexCoord.x - 0.5) / M_PI * 180;

    FragColor = mix(centerColor,borderColor,min(1.0,max(0.0,dist)));
    //FragColor = mix(centerColor,borderColor,dist);



   /* if (angle >= 0 && angle < 5)
    {
        FragColor.a *= 0.99;
    }*/

}
