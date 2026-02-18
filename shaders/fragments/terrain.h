#version 330


//shader for rendering gravity fields, incomplete

uniform vec2 finalColorSizes;
uniform sampler2D sprite;

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Output fragment color
out vec4 finalColor;

uniform float outline_thickness = 10;
uniform vec3 outline_colour = vec3(0, 0, 1);
uniform float outline_threshold = .5;

void main()
{
    finalColor = texture(sprite, fragTexCoord);

    if (finalColor.a <= outline_threshold) {
        ivec2 size = textureSize(sprite, 0);

        float uv_x = fragTexCoord.x * size.x;
        float uv_y = fragTexCoord.y * size.y;

        float sum = 0.0;
        for (int n = 0; n < 9; ++n) {
            uv_y = (fragTexCoord.y * size.y) + (outline_thickness * float(n - 4.5));
            float h_sum = 0.0;
            h_sum += texelFetch(sprite, ivec2(uv_x - (4.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x - (3.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x - (2.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x - outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x, uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x + outline_thickness, uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x + (2.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x + (3.0 * outline_thickness), uv_y), 0).a;
            h_sum += texelFetch(sprite, ivec2(uv_x + (4.0 * outline_thickness), uv_y), 0).a;
            sum += h_sum / 9.0;
        }

        if (sum / 9.0 >= 0.0001) {
            finalColor = vec4(outline_colour, 1);
        }
    }
}
