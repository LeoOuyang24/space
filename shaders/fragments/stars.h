#version 330

//generates a starry background

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

uniform vec2 screenDimen;

// Output fragment color
out vec4 finalColor;

float PHI = 1.61803398874989484820459;  // Φ = Golden Ratio
float rand(vec2 xy,float seed){
    //shamelessly stolen from https://stackoverflow.com/a/28095165
       return fract(tan(distance(xy*PHI, xy)*seed)*xy.x);
}

float rand(vec2 xy)
{
    return rand(xy,0.12431234215);
}

float rand(float x)
{
    return rand(vec2(x,0));
}

void main()
{
    int starSpacing = 100;

    vec2 pos = vec2(fragTexCoord.x*screenDimen.x,(1 - fragTexCoord.y)*screenDimen.y);

    vec2 origin = vec2(floor(pos.x/starSpacing)*starSpacing,floor(pos.y/starSpacing)*starSpacing);
    vec2 center = vec2(origin.x + starSpacing/2,origin.y + starSpacing/2);

    float seed = rand(center);


    float isStar = min(1,floor(seed/0.95)); //0 if seed is less than 0.7; basically makes it so only 30% of potential stars are rendered

    float ratio = rand(origin);
    vec3 starColor = vec3(ratio,rand(origin.x),rand(origin.y));
    float starRadius = ratio*starSpacing/2;

    finalColor = isStar*mix(vec4(1,1,1,1),vec4(starColor,0),length(pos - center)/(starRadius));
}
