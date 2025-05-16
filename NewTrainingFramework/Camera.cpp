#include "stdafx.h"
#include "Camera.h"
#include <cmath>

Camera::Camera()
    : position(Vector3(0.0f, 0.0f, -1.0f)),
    target(Vector3(0.0f, 0.0f, 0.0f)),
    up(Vector3(0.0f, 1.0f, 0.0f)),
    moveSpeed(0.5f),
    rotateSpeed(0.5f),
    nearPlane(0.2f),
    farPlane(10.0f),
    fov(45.0f),
    deltaTime(0.0f) {
    float aspect = (float)Globals::screenWidth / Globals::screenHeight;
    perspectiveMatrix.SetPerspective(fov, aspect, nearPlane, farPlane);
    updateWorldView();
}

Camera::Camera(Vector3& pos, Vector3& tgt, Vector3& u,
    GLfloat mSpeed, GLfloat rSpeed,
    GLfloat nPlane, GLfloat fPlane, GLfloat f)
    : position(pos), target(tgt), up(u),
    moveSpeed(mSpeed), rotateSpeed(rSpeed),
    nearPlane(nPlane), farPlane(fPlane), fov(f),
    deltaTime(0.0f) {
    float aspect = Globals::screenWidth/Globals::screenHeight;
    perspectiveMatrix.SetPerspective(fov, aspect, nearPlane, farPlane);
    updateWorldView();
}

void Camera::moveOx(int sens) {
    position +=xAxis * sens * moveSpeed * deltaTime;
    target += xAxis * sens * moveSpeed * deltaTime;
    updateWorldView();
}

void Camera::moveOy(int sens) {
    position += yAxis * sens * moveSpeed * deltaTime;
    target += yAxis * sens * moveSpeed * deltaTime;
    updateWorldView();
}

void Camera::moveOz(int sens) {
    position += zAxis * sens * moveSpeed * deltaTime;
    target += zAxis * sens * moveSpeed * deltaTime;
    updateWorldView();
}

void Camera::rotateOy(int sens) {
    float rotationAngle = sens * rotateSpeed * deltaTime;
    Matrix mRotateOY;
    mRotateOY.SetRotationY(rotationAngle);
    Vector4 localTarget(0.0f, 0.0f, -(target - position).Length(), 1.0f);
    Vector4 rotatedTarget = localTarget * mRotateOY;
    target = (rotatedTarget * worldMatrix).ToVector3();
    updateWorldView();
}

void Camera::rotateOx(int sens) {
    float rotationAngle = sens * rotateSpeed * deltaTime;
    Matrix mRotateOX;
    mRotateOX.SetRotationX(rotationAngle);
    Vector4 localUp(0.0f, 1.0f, 0.0f, 0.0f);
    Vector4 rotatedLocalUp = localUp * mRotateOX;
    up = (rotatedLocalUp * worldMatrix).ToVector3();
    up = up.Normalize();
    Vector4 localTarget(0.0f, 0.0f, -(target - position).Length(), 1.0f);
    Vector4 rotatedTarget = localTarget * mRotateOX;
    target = (rotatedTarget * worldMatrix).ToVector3();
    updateWorldView();
}

void Camera::rotateOz(int sens) {
    float rotationAngle = sens * rotateSpeed * deltaTime;
    Matrix mRotateOZ;
    mRotateOZ.SetRotationZ(rotationAngle);
    Vector4 localUp(0.0f, 1.0f, 0.0f, 0.0f);
    Vector4 rotatedLocalUp = localUp * mRotateOZ;
    up = (rotatedLocalUp * worldMatrix).ToVector3();
    up = up.Normalize();
    Vector4 localTarget(0.0f, 0.0f, -(target - position).Length(), 1.0f);
    Vector4 rotatedTarget = localTarget * mRotateOZ;
    target = (rotatedTarget * worldMatrix).ToVector3();
    updateWorldView();
}

void Camera::updateAxes() {
    zAxis = -(target - position).Normalize();
    yAxis = up.Normalize();
    xAxis = zAxis.Cross(yAxis).Normalize();
}

void Camera::updateWorldView() {
    updateAxes();

    Matrix R;
    R.m[0][0] = xAxis.x; R.m[0][1] = xAxis.y; R.m[0][2] = xAxis.z; R.m[0][3] = 0.0f;
    R.m[1][0] = yAxis.x; R.m[1][1] = yAxis.y; R.m[1][2] = yAxis.z; R.m[1][3] = 0.0f;
    R.m[2][0] = zAxis.x; R.m[2][1] = zAxis.y; R.m[2][2] = zAxis.z; R.m[2][3] = 0.0f;
    R.m[3][0] = 0.0f;    R.m[3][1] = 0.0f;    R.m[3][2] = 0.0f;    R.m[3][3] = 1.0f;

    Matrix T;
    T.SetTranslation(position);

    worldMatrix = R * T;

    Matrix T_inv;
    T_inv.SetTranslation(-position);

    Matrix R_transpose = R.Transpose();

    viewMatrix = T_inv * R_transpose;
}

void Camera::setDeltaTime(GLfloat dt) {
    deltaTime = dt;
}
