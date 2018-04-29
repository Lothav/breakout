#version 300 es

uniform mat4 view;

in vec3 vert;
in vec2 vertTexCoord;
out vec2 fragTexCoord;

void main() {
    // Pass the tex coord straight through to the fragment shader
    fragTexCoord = vertTexCoord;
    gl_Position = view * vec4(vert, 1.0f);
}
