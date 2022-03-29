#ifndef MESH_H
#define MESH_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <learnopengl/shader.h>

#include <string>
#include <vector>
using namespace std;

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    string type;
    string path;
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;

        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    void Draw(Shader &shader) 
    {
        // bind appropriate textures
        unsigned int diffuseNr  = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr   = 1;
        unsigned int heightNr   = 1;
        for(unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            string number;
            string name = textures[i].type;
            if(name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if(name == "texture_specular")
                number = std::to_string(specularNr++); // transfer unsigned int to stream
            else if(name == "texture_normal")
                number = std::to_string(normalNr++); // transfer unsigned int to stream
             else if(name == "texture_height")
                number = std::to_string(heightNr++); // transfer unsigned int to stream

            // now set the sampler to the correct texture unit
            glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id);
        }
        
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

    void convertFacesToAdjancencyFormat()
    {
        // Elements with adjacency info
        std::vector<GLuint> elAdj(indices.size() * 2);

        // Copy and make room for adjacency info
        for (GLuint i = 0; i < indices.size(); i += 3)
        {
            elAdj[i * 2 + 0] = indices[i];
            elAdj[i * 2 + 1] = std::numeric_limits<GLuint>::max();
            elAdj[i * 2 + 2] = indices[i + 1];
            elAdj[i * 2 + 3] = std::numeric_limits<GLuint>::max();
            elAdj[i * 2 + 4] = indices[i + 2];
            elAdj[i * 2 + 5] = std::numeric_limits<GLuint>::max();
        }

        // Find matching edges
        for (GLuint i = 0; i < elAdj.size(); i += 6)
        {
            // A triangle
            GLuint a1 = elAdj[i];
            GLuint b1 = elAdj[i + 2];
            GLuint c1 = elAdj[i + 4];

            // Scan subsequent triangles
            for (GLuint j = i + 6; j < elAdj.size(); j += 6)
            {
                GLuint a2 = elAdj[j];
                GLuint b2 = elAdj[j + 2];
                GLuint c2 = elAdj[j + 4];

                // Edge 1 == Edge 1
                if ((a1 == a2 && b1 == b2) || (a1 == b2 && b1 == a2))
                {
                    elAdj[i + 1] = c2;
                    elAdj[j + 1] = c1;
                }
                // Edge 1 == Edge 2
                if ((a1 == b2 && b1 == c2) || (a1 == c2 && b1 == b2))
                {
                    elAdj[i + 1] = a2;
                    elAdj[j + 3] = c1;
                }
                // Edge 1 == Edge 3
                if ((a1 == c2 && b1 == a2) || (a1 == a2 && b1 == c2))
                {
                    elAdj[i + 1] = b2;
                    elAdj[j + 5] = c1;
                }
                // Edge 2 == Edge 1
                if ((b1 == a2 && c1 == b2) || (b1 == b2 && c1 == a2))
                {
                    elAdj[i + 3] = c2;
                    elAdj[j + 1] = a1;
                }
                // Edge 2 == Edge 2
                if ((b1 == b2 && c1 == c2) || (b1 == c2 && c1 == b2))
                {
                    elAdj[i + 3] = a2;
                    elAdj[j + 3] = a1;
                }
                // Edge 2 == Edge 3
                if ((b1 == c2 && c1 == a2) || (b1 == a2 && c1 == c2))
                {
                    elAdj[i + 3] = b2;
                    elAdj[j + 5] = a1;
                }
                // Edge 3 == Edge 1
                if ((c1 == a2 && a1 == b2) || (c1 == b2 && a1 == a2))
                {
                    elAdj[i + 5] = c2;
                    elAdj[j + 1] = b1;
                }
                // Edge 3 == Edge 2
                if ((c1 == b2 && a1 == c2) || (c1 == c2 && a1 == b2))
                {
                    elAdj[i + 5] = a2;
                    elAdj[j + 3] = b1;
                }
                // Edge 3 == Edge 3
                if ((c1 == c2 && a1 == a2) || (c1 == a2 && a1 == c2))
                {
                    elAdj[i + 5] = b2;
                    elAdj[j + 5] = b1;
                }
            }
        }

        // Look for any outside edges
        for (GLuint i = 0; i < elAdj.size(); i += 6)
        {
            if (elAdj[i + 1] == std::numeric_limits<GLuint>::max()) elAdj[i + 1] = elAdj[i + 4];
            if (elAdj[i + 3] == std::numeric_limits<GLuint>::max()) elAdj[i + 3] = elAdj[i];
            if (elAdj[i + 5] == std::numeric_limits<GLuint>::max()) elAdj[i + 5] = elAdj[i + 2];
        }

        // Copy all data back into el
        indices = elAdj;
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);  

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);	
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);	
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);	
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));

        glBindVertexArray(0);
    }
};
#endif
