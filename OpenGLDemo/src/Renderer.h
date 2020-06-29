#pragma once

#include <GL/glew.h>
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Program.h"

class Renderer
{
public:
    void draw(const VertexArray& vertexArray, const IndexBuffer& indexBuffer, const Program& program) const;
    void clear() const;

    static void initDebug();
};