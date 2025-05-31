#include "stdafx.h"
#include "SceneObject.h"
#include "Model.h"
#include "Shader.h"
#include "Texture.h"

SceneObject::SceneObject()
    : id(0),name(""), position(0, 0, 0), rotation(0, 0, 0), scale(1, 1, 1),
    model(nullptr), shader(nullptr), depthTest(true), wiredFormat(false) {}

void SceneObject::Draw(Camera* activeCamera) {
    if (!shader || !model) {
        return; 
    }
    int err = glGetError();
    glUseProgram(shader->programId);
   
    //for (size_t i = 0; i < textures.size(); ++i) {
    //    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i)); // Activate texture unit
    //    glBindTexture(GL_TEXTURE_2D, textures[i]->textureId); // Bind the texture
    //}

    (depthTest) ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

    if (wiredFormat) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->wiredIboId); 
        glDrawElements(GL_LINES, model->nrIndexesWired, GL_UNSIGNED_SHORT, 0); 
    }
    else {
        glBindBuffer(GL_ARRAY_BUFFER, model->vboId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, model->iboId); 

        sendCommonData(activeCamera);
        sendSpecificData();

        glDrawElements(GL_TRIANGLES, model->nrIndexes, GL_UNSIGNED_SHORT, 0);
        
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //for (size_t i = 0; i < textures.size(); ++i) {
    //    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i));
    //    glBindTexture(GL_TEXTURE_2D, 0);
    //}

}

void SceneObject::Update(float deltaTime) {
    // Placeholder for per-frame updates (e.g., movement, animations)
}

void SceneObject::sendCommonData(Camera* activeCamera) {
    if (shader->matrixUniform != -1) {
        Matrix translationMatrix;
        translationMatrix.SetTranslation(position);

        Matrix rotationXMatrix, rotationYMatrix, rotationZMatrix;
        rotationXMatrix.SetRotationX(rotation.x);
        rotationYMatrix.SetRotationY(rotation.y);
        rotationZMatrix.SetRotationZ(rotation.z);

        Matrix scaleMatrix;
        scaleMatrix.SetScale(scale);

        
        Matrix modelMatrix = scaleMatrix * rotationXMatrix * rotationYMatrix * rotationZMatrix * translationMatrix;
        //Matrix modelMatrix;
        //modelMatrix.SetIdentity();
        Matrix MVP = modelMatrix * activeCamera->viewMatrix * activeCamera->perspectiveMatrix;

        glUniformMatrix4fv(shader->matrixUniform, 1, GL_FALSE, (GLfloat*)MVP.m);
    }

    if (!textures.empty() && shader->textureUniform != -1) {
        glActiveTexture(GL_TEXTURE0); 
        glBindTexture(GL_TEXTURE_2D, textures[0]->textureId); 
        glUniform1i(shader->textureUniform, 0); 
    }

    if (shader->positionAttribute != -1) {
        glEnableVertexAttribArray(shader->positionAttribute);
        glVertexAttribPointer(shader->positionAttribute, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
    }

    if (shader->uvAttribute != -1) {
        glEnableVertexAttribArray(shader->uvAttribute);
        glVertexAttribPointer(shader->uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3) * 4));
    }
}

void SceneObject::sendSpecificData() {
    // Placeholder for derived classes to send specific data
}

SceneObject::~SceneObject() {
    // Cleanup resources if necessary
}