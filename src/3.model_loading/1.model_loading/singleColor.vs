#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 NormalVec;

out int isEdge;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform int shaderMode;
 
float LineThickness = .03f;

void main(void)
{
    NormalVec = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords; 
    if(shaderMode == 0)
    {
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
    else if(shaderMode == 1 || shaderMode == 2)
    {
        vec4 original = projection * view * model * vec4(aPos, 1.0);
        vec4 normal = projection * view * model * vec4(aNormal, 1.0);
        gl_Position = original + (LineThickness * normal);
    }
}