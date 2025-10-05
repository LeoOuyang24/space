#version 330

//generates a starry background

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio
float M_PI = 3.1415926;

uniform float time;

void main()
{

    float dist = (length(fragTexCoord - vec2(0.5,0.5))); //find the distance between interpolated pixel and center
    float angle = atan(fragTexCoord.y - 0.5, fragTexCoord.x - 0.5);
    float actualAngle = angle + time/180.0*M_PI;

    finalColor = mix(vec4(0,0,0,0),vec4(0,0,0,1), int(abs(dist - mod(actualAngle,M_PI/2))<=.1)); //create the spiral
    finalColor = finalColor + mix(vec4(1,1,1,0),vec4(1,1,1,1),int(dist <= .334)); //create the circle

    finalColor.a = min(1,max(0,finalColor.a)); //the alpha value needs to be either 0 or 1
    finalColor = finalColor*mix(vec4(1,1,1,1),vec4(1,1,0,0),dist*2); //blend the colors

    //finalColor += vec4(1,0,0,0.1);

}
