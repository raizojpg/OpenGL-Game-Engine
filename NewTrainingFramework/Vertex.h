#pragma once
#include "Math.h"

class Vertex {
public:
    Vector3 pos;
    Vector3 normal;
    Vector3 binormal;
    Vector3 tangent;
    Vector2 uv;
    Vector3 color;

    Vertex() = default;

    Vertex(const Vertex& other) :
        pos{ other.pos }, normal{ other.normal }, binormal{ other.binormal },
        tangent{ other.tangent }, uv{ other.uv }, color{ other.color } {}

};