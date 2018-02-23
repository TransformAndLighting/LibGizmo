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



#ifndef GIZMOTRANSFORMRENDER_H__
#define GIZMOTRANSFORMRENDER_H__

#ifdef GIZMO_ENABLE_QT
#include <QOpenGLContext>
#elif GIZMO_PLATFORM_APPLE
	#include "TargetConditionals.h"
	#if GIZMO_PLATFORM_IOS_SIMULATOR
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
	#elif GIZMO_PLATFORM_IOS
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
	#elif GIZMO_PLATFORM_MAC_OS
		#include <OpenGL/gl3.h>
		#include <OpenGL/gl3ext.h>
	#else
		#error "Unknown Apple platform"
	#endif
#elif GIZMO_PLATFORM_ANDROID
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
#elif GIZMO_PLATFORM_EMSCRIPTEN
	#include <emscripten.h>
	#include <SDL.h>
	#define GL_GLEXT_PROTOTYPES 1
	#include <SDL_opengles2.h>
#else
#include "gl_gizmo.h"
#endif

typedef tvector4 tplane;
struct tmatrix4;

class CGizmoTransformRender
{
public:
    CGizmoTransformRender();
    virtual ~CGizmoTransformRender();

    void DrawCircle(const tvector3 &orig,const tvector3 &color,const tvector3 &vtx,const tvector3 &vty);
    void DrawCircleHalf(const tvector3 &orig,const tvector3 &color,const tvector3 &vtx,const tvector3 &vty,tplane &camPlan);
    void DrawAxis(const tvector3 &orig, const tvector3 &axis, const tvector3 &vtx,const tvector3 &vty, float fct,float fct2,const tvector3 &col);
    void DrawCamem(const tvector3& orig,const tvector3& vtx,const tvector3& vty,float ng);
    void DrawQuad(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3 &axisV);
    void DrawTri(const tvector3& orig, float size, bool bSelected, const tvector3& axisU, const tvector3& axisV);

protected:
    tmatrix m_Model,m_Proj;

private:
    void ActivateProgram();
    void DeactivateProgram();

    static void Initialize();
    static void Terminate();

    static GLuint m_Count;
    static GLuint m_VertexBuffer;
    static GLuint m_Program;
    static GLuint m_ColorUniform;
    static GLuint m_ModelviewMatrixUniform;
    static GLuint m_ProjectionMatrixUniform;
    static GLuint m_ProjectionMatrixUniform;
};

#endif // !defined(AFX_GIZMOTRANSFORMRENDER_H__549F6E7A_D46D_4B18_9E74_76B7E43A3841__INCLUDED_)
