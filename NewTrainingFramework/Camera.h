#pragma once

#include "../Utilities/Math.h"
#include "Globals.h"

class Camera {
public:
    int id;
    Vector3 position;
    Vector3 target;
    Vector3 up;

    GLfloat moveSpeed;
    GLfloat rotateSpeed;
    GLfloat nearPlane;
    GLfloat farPlane;
    GLfloat fov;
    GLfloat deltaTime;

    Vector3 xAxis;
    Vector3 yAxis;
    Vector3 zAxis;

    Matrix viewMatrix;
    Matrix worldMatrix;
    Matrix perspectiveMatrix;

    Camera();
    Camera(Vector3& position, Vector3& target, Vector3& up,
        GLfloat moveSpeed = 1.0f, GLfloat rotateSpeed = 0.5f,
        GLfloat nearPlane = 0.1f, GLfloat farPlane = 100.0f, GLfloat fov = 60.0f);

    void moveOx(int sens);
    void moveOy(int sens);
    void moveOz(int sens);

    void rotateOy(int sens);
    void rotateOx(int sens);
    void rotateOz(int sens);

    void updateAxes();
    void updatePerspective();
    void updateWorldView();
    void setDeltaTime(GLfloat dt);

};
