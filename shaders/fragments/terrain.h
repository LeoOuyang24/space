#version 330


//shader for rendering gravity fields, incomplete

uniform vec2 pixelSizes;
uniform sampler2D sprite;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;


void main()
{

    int outline = 20;


    vec4 texColor = texture(sprite,fragTexCoord);
    float maxA = texColor.a;
    float minA = texColor.a;

    float minDist = outline;

    for (int i = 0; i < 4; i ++)
    {
        vec2 dir = vec2((i%2*2- 1)*int(i<2),(i%2*2 - 1)*int(i>1));

        for (int j = 1; j < outline - 1; j ++)
        {
            minDist = mix(minDist,min(minDist,j),float(texture(sprite,fragTexCoord + dir*j*pixelSizes).a > 0));
        }

        float trans = texture(sprite,fragTexCoord + dir*outline*pixelSizes).a;

        maxA = max(maxA,trans);
        minA = min(minA,trans);
    }

    finalColor = mix(texColor,vec4(0,1,1,minDist/outline),min(maxA - minA,1 - texColor.a));

}
