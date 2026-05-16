#version 330


//shader for rendering gravity fields, incomplete

uniform sampler2D sprite;
uniform float outline_thickness = 3;
uniform float shade = 1;
// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;


void main()
{
    finalColor = texture(sprite, fragTexCoord);
    
    if (finalColor.a == 0)
    {
        ivec2 size = textureSize(sprite, 0);

        vec4 sum = vec4(0,0,0,0);
        for (int n = 0; n < 9; ++n) {
            vec4 h_sum = texelFetch(sprite,ivec2(fragTexCoord*size + vec2((n%3-1),(n/3-1))*outline_thickness),0);
            //vec4 h_sum = texelFetch(sprite,ivec2(fragTexCoord*size + vec2((n%2*2 - 1)*int(n >= 2),(n%2*2 - 1)*int(n<2))*outline_thickness),0);

            //sum += h_sum / 9.0;
            sum = mix(sum,h_sum,int(length(h_sum) > 0));
        } 

        finalColor = vec4(sum.xyz*0.5,int(sum.a > 0));
    }
    finalColor.xyz *= shade;
}
