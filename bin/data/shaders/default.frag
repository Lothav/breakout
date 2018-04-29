#version 300 es

precision mediump float;
uniform sampler2D tex;
in vec2 fragTexCoord;
out vec4 finalColor;

void main() {
    finalColor = texture(tex, fragTexCoord);
}
