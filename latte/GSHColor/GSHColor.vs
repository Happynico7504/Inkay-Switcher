#version 150

in vec3 aPosition;
in vec4 aColour;

out vec4 vColor;

void main() {
    gl_Position = vec4(aPosition, 1.0);
    vColor = aColour;
}