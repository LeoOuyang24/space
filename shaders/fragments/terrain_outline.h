#version 330


//shader for rendering gravity fields, incomplete

uniform vec2 finalColorSizes;
uniform sampler2D sprite;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

uniform float outline_thickness = 2;
uniform vec3 outline_colour = vec3(0, 0, 1);
uniform float outline_threshold = .0;

void main()
{
    finalColor = texture(sprite, fragTexCoord);

    if (finalColor.a <= outline_threshold) {
        ivec2 size = textureSize(sprite, 0);

        vec4 sum = vec4(0,0,0,0);
        for (int n = 0; n < 4; ++n) {
            //vec4 h_sum = texelFetch(sprite,ivec2(fragTexCoord*size + vec2((n%3-1),(n/3-1))*outline_thickness),0);
            vec4 h_sum = texelFetch(sprite,ivec2(fragTexCoord*size + vec2((n%2*2 - 1)*int(n >= 2),(n%2*2 - 1)*int(n<2))*outline_thickness),0);

            //sum += h_sum / 9.0;
            sum = mix(sum,h_sum,int(length(h_sum) > 0));
        } 

        /*for (int i = 0; i < 4; i ++)
        {
            sum += texelFetch(sprite,ivec2(fragTexCoord*size + vec2((i%2*2 - 1)*int(i > 2),(i%2*2 - 1)*int(i<2))*outline_thickness),0);
            max_a = max(sum.a,max_a);

        }*/


        //sum *= 1.0/9.0;

        finalColor = vec4(sum.xyz*0.5,int(sum.a > 0));
    }
}
