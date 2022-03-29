#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 NormalVec;

uniform sampler2D texture_diffuse1;

uniform vec3 LightPosition;// = vec3(10.0, 10.0, 20.0);

uniform int shaderMode;

in int isEdge;
vec4 LineColor = vec4(0, 0, 0, 1);

void main()
{    
    vec3 n = normalize(NormalVec);
   if(shaderMode == 0)
   {
       FragColor = texture(texture_diffuse1, TexCoords);
   }
   else if (shaderMode == 1)
   {
       vec4 color2;
       

       float intensity = dot(normalize(LightPosition),NormalVec);
       //float intensity = dot(vec3(LightPosition),n);
       
           if (intensity > 0.95)      color2 = vec4(1.0, 1.0, 1.0, 1.0);
           else if (intensity > 0.75) color2 =  vec4(0.8, 0.8, 0.8, 1.0);
           else if (intensity > 0.50) color2 =  vec4(0.6, 0.6, 0.6, 1.0);
           else if (intensity > 0.25) color2 =  vec4(0.4, 0.4, 0.4, 1.0);
           else                       color2 =  vec4(0.2, 0.2, 0.2, 1.0);
       
           FragColor = color2;
   }
   else if (shaderMode == 2)
   {
         vec4 color1 = texture(texture_diffuse1, TexCoords);
         vec4 color2;
          

          float intensity = dot(normalize(LightPosition),NormalVec);
          //float intensity = dot(vec3(LightPosition),n);
          
          if (intensity > 0.95)      color2 = texture(texture_diffuse1, TexCoords) + vec4(1.0, 1.0, 1.0, 1.0);
          else if (intensity > 0.75) color2 = texture(texture_diffuse1, TexCoords) + vec4(0.8, 0.8, 0.8, 1.0);
          else if (intensity > 0.50) color2 = texture(texture_diffuse1, TexCoords) + vec4(0.6, 0.6, 0.6, 1.0);
          else if (intensity > 0.25) color2 = texture(texture_diffuse1, TexCoords) + vec4(0.4, 0.4, 0.4, 1.0);
          else                       color2 = texture(texture_diffuse1, TexCoords) + vec4(0.2, 0.2, 0.2, 1.0);
          
          FragColor = color1 * color2;
    }
}   