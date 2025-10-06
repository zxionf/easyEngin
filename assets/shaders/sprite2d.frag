#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform vec4 spriteColor;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    FragColor = texColor * spriteColor;
    
    // 简单的透明度丢弃
    if (FragColor.a < 0.1)
        discard;
}