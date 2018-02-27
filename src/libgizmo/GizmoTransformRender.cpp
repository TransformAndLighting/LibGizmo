///////////////////////////////////////////////////////////////////////////////////////////////////
// LibGizmo
// File Name :
// Creation : 10/01/2012
// Author : Cedric Guillemet
// Description : LibGizmo
//
///Copyright (C) 2012 Cedric Guillemet
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
//of the Software, and to permit persons to whom the Software is furnished to do
///so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
///FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef NDEBUG
#include <stdio.h>
#endif

#include "GizmoTransformRender.h"
#include "IGizmo.h"

#if GIZMO_USE_GL_GIZMO
#include "gl_gizmo.c"
#else
#define gizmo_ogl_LoadFunctions()
#endif

int    CGizmoTransformRender::m_Count                   =  0;
GLuint CGizmoTransformRender::m_VertexBuffer            =  0;
GLuint CGizmoTransformRender::m_Program                 =  0;
GLint  CGizmoTransformRender::m_ColorUniform            = -1;
GLint  CGizmoTransformRender::m_ModelviewMatrixUniform  = -1;
GLint  CGizmoTransformRender::m_ProjectionMatrixUniform = -1;

static void AttachShader(const char *source, GLenum type, GLuint program)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, 0);
    glCompileShader(shader);
#ifndef NDEBUG
    {
        GLint compiled, len;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
            if (len > 0) {
                char *message = new char[len];
                glGetShaderInfoLog(shader, len, &len, message);
                fprintf(stderr, "shader: %s", message);
                delete[] message;
            }
        }
    }
#endif
    glAttachShader(program, shader);
    glDeleteShader(shader);
}

CGizmoTransformRender::CGizmoTransformRender()
{
    Initialize();
}

CGizmoTransformRender::~CGizmoTransformRender()
{
    Terminate();
}

void CGizmoTransformRender::Initialize()
{
    if (m_Count == 0) {
        gizmo_ogl_LoadFunctions();
        m_Program = glCreateProgram();
        static const char vertexShader[] = ""
                "precision highp float;\n"
                "#if __VERSION__ < 130\n"
                "#define in attribute\n"
                "#define out varying\n"
                "#endif\n"
                "uniform mat4 modelViewMatrix;\n"
                "uniform mat4 projectionMatrix;\n"
                "uniform vec4 color;\n"
                "in vec3 inPosition;\n"
                "out vec4 outColor;\n"
                "void main() {\n"
                "  outColor = color;\n"
                "  gl_Position = (projectionMatrix * modelViewMatrix) * vec4(inPosition, 1.0);\n"
                "}\n";
        AttachShader(vertexShader, GL_VERTEX_SHADER, m_Program);
        static const char fragmentShader[] = ""
                "precision highp float;\n"
                "#if __VERSION__ < 130\n"
                "#define in varying\n"
                "#define outPixelColor gl_FragColor\n"
                "#else\n"
                "out vec4 outPixelColor;\n"
                "#endif\n"
                "in vec4 outColor;\n"
                "void main() {\n"
                "  outPixelColor = outColor;\n"
                "}\n"
                ;
        AttachShader(fragmentShader, GL_FRAGMENT_SHADER, m_Program);
        glBindAttribLocation(m_Program, 0, "inPosition");
        glLinkProgram(m_Program);
#ifndef NDEBUG
        {
            GLint compiled, len;
            glGetProgramiv(m_Program, GL_LINK_STATUS, &compiled);
            if (!compiled) {
                glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &len);
                if (len > 0) {
                    char *message = new char[len];
                    glGetProgramInfoLog(m_Program, len, &len, message);
                    fprintf(stderr, "shader: %s", message);
                    delete[] message;
                }
            }
        }
#endif
        m_ColorUniform = glGetUniformLocation(m_Program, "color");
        m_ModelviewMatrixUniform = glGetUniformLocation(m_Program, "modelViewMatrix");
        m_ProjectionMatrixUniform = glGetUniformLocation(m_Program, "projectionMatrix");

        glGenBuffers(1, &m_VertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(128 * sizeof(tvector3)), 0, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    ++m_Count;
}

void CGizmoTransformRender::Terminate()
{
    --m_Count;
    if (m_Count != 0) return;
    glDeleteBuffers(1, &m_VertexBuffer);
    glDeleteProgram(m_Program);
    m_VertexBuffer            =  0;
    m_Program                 =  0;
    m_ColorUniform            = -1;
    m_ModelviewMatrixUniform  = -1;
    m_ProjectionMatrixUniform = -1;
}

void CGizmoTransformRender::DrawCircle(const tvector3 &orig, const tvector3 &color, const tvector3 &vtx, const tvector3 &vty)
{
    static const int size = 50;
    tvector3 vertices[size];
    for (int i = 0; i < size; i++) {
        tvector3 vt;
        vt  = vtx * cos((2 * ZPI / size) * i);
        vt += vty * sin((2 * ZPI / size) * i);
        vt += orig;
        vertices[i] = vt;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glDisable(GL_DEPTH_TEST);
    ActivateProgram();
    glUniform4f(m_ColorUniform, color.x, color.y, color.z, 1);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(vertices)), vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
    glDrawArrays(GL_LINE_LOOP, 0, size);
    DeactivateProgram();
    glEnable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void CGizmoTransformRender::DrawCircleHalf(const tvector3 &orig, const tvector3 &color, const tvector3 &vtx, const tvector3 &vty, tplane &camPlan)
{
    static const int size = 30;
    tvector3 vertices[size];
    int j = 0;
    for (int i = 0; i < size; i++) {
        tvector3 vt;
        vt  = vtx * cos((ZPI / size) * i);
        vt += vty * sin((ZPI / size) * i);
        vt += orig;
        if (camPlan.DotNormal(vt)) {
            vertices[j++] = vt;
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glDisable(GL_DEPTH_TEST);
    ActivateProgram();
    glUniform4f(m_ColorUniform, color.x, color.y, color.z, 1);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(vertices)), vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), vertices);
    glDrawArrays(GL_LINE_STRIP, 0, j);
    DeactivateProgram();
    glEnable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGizmoTransformRender::DrawAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx, const tvector3 &vty, float fct, float fct2, const tvector3 &col)
{
    tvector3 vertices1[2] = { orig, orig + axis };
    tvector3 vertices2[62];
    int j = 0;
    for (int i = 0; i <= 30; i++) {
        tvector3 pt;
        pt  = vtx * cos(((2 * ZPI) / 30.0f) * i) * fct;
        pt += vty * sin(((2 * ZPI) / 30.0f) * i) * fct;
        pt += axis * fct2;
        pt += orig;
        pt  = vtx * cos(((2 * ZPI) / 30.0f) * (i + 1)) * fct;
        pt += vty * sin(((2 * ZPI) / 30.0f) * (i + 1)) * fct;
        pt += axis * fct2;
        pt += orig;
        vertices2[j++] = pt;
        vertices2[j++] = orig + axis;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glDisable(GL_DEPTH_TEST);
    ActivateProgram();
    glUniform4f(m_ColorUniform, col.x, col.y, col.z, 1);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(vertices1)), vertices1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices1[0]), 0);
    glDrawArrays(GL_LINES, 0, 2);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(vertices2)), vertices2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices2[0]), 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, j);
    DeactivateProgram();
    glEnable(GL_DEPTH_TEST);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGizmoTransformRender::DrawCamem(const tvector3 &orig, const tvector3 &vtx, const tvector3 &vty, float ng)
{
    tvector3 vertices[52];
    int j = 0;
    vertices[j++] = orig;
    for (int i = 0 ; i <= 50 ; i++) {
        tvector3 vt;
        vt = vtx * cos((ng / 50) * i);
        vt += vty * sin((ng / 50) * i);
        vt += orig;
        vertices[j++] = vt;
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    ActivateProgram();
    glUniform4f(m_ColorUniform, float(0xf0) / 255.0f, float(0x12) / 255.0f, float(0xbe) / 255.0f, 0.5f);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(vertices)), vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, j);
    j = 0;
    vertices[j++] = &orig.x;
    for (int i = 0 ; i <= 50 ; i++) {
        tvector3 vt;
        vt  = vtx * cos(((ng)/50)*i);
        vt += vty * sin(((ng)/50)*i);
        vt += orig;
        vertices[j++] = vt;
    }
    glDisable(GL_BLEND);
    glUniform4f(m_Program, 1.0f, 1.0f, 0.2f, 1.0f);
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(vertices)), vertices);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertices[0]), 0);
    glDrawArrays(GL_LINE_LOOP, 0, j);
    DeactivateProgram();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGizmoTransformRender::DrawQuad(const tvector3 &orig, float size, bool bSelected, const tvector3 &axisU, const tvector3 &axisV)
{
    tvector3 pts[4];
	tvector3 tmp;
	pts[0] = orig;
    pts[1] = orig + (axisU * size);
	pts[2] = orig + (axisV * size);
	pts[3] = orig + (axisU + axisV)*size;
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    ActivateProgram();
    if (!bSelected) {
        glUniform4f(m_ColorUniform, 1, 1, 0, 0.5f);
    }
    else {
        glUniform4f(m_ColorUniform, 1, 1, 1, 0.6f);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(pts)), pts);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pts[0]), pts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, sizeof(pts) / sizeof(pts[0]));
    if (!bSelected) {
        glUniform4f(m_ColorUniform, 1, 1, 0.2f, 1);
    }
    else {
        glUniform4f(m_ColorUniform, 1, 1, 1, 0.6f);
    }
    tmp    = pts[2];
    pts[2] = pts[3];
    pts[3] = tmp;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pts[0]), pts);
    glDrawArrays(GL_LINE_STRIP, 0, sizeof(pts) / sizeof(pts[0]));
    DeactivateProgram();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void CGizmoTransformRender::DrawTri(const tvector3 &orig, float size, bool bSelected, const tvector3 &axisU, const tvector3 &axisV)
{
    tvector3 pts[3];
    pts[0] = orig;
    pts[1] = axisU;
    pts[2] = axisV;
    pts[1] *= size;
    pts[2] *= size;
    pts[1] += orig;
    pts[2] += orig;
    glBindBuffer(GL_ARRAY_BUFFER, m_VertexBuffer);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    ActivateProgram();
    if (!bSelected) {
        glUniform4f(m_ColorUniform, 1, 1, 0, 0.5f);
    }
    else {
        glUniform4f(m_ColorUniform, 1, 1, 1, 0.6f);
    }
    glBufferSubData(GL_ARRAY_BUFFER, 0, GLsizeiptr(sizeof(pts)), pts);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pts[0]), pts);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(pts) / sizeof(pts[0]));
    if (!bSelected) {
        glUniform4f(m_ColorUniform, 1, 1, 0.2f, 1);
    }
    else {
        glUniform4f(m_ColorUniform, 1, 1, 1, 0.6f);
    }
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(pts[0]), pts);
    glDrawArrays(GL_LINE_STRIP, 0, sizeof(pts) / sizeof(pts[0]));
    DeactivateProgram();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void CGizmoTransformRender::ActivateProgram()
{
    glUseProgram(m_Program);
    glUniformMatrix4fv(m_ModelviewMatrixUniform, 1, GL_FALSE, m_Model);
    glUniformMatrix4fv(m_ProjectionMatrixUniform, 1, GL_FALSE, m_Proj);
    glEnableVertexAttribArray(0);
}

void CGizmoTransformRender::DeactivateProgram()
{
    glDisableVertexAttribArray(0);
    glUseProgram(0);
}

bool DestroyGizmo(IGizmo *gizmo)
{
    if (gizmo == 0) return false;
    delete gizmo;
    return true;
}
