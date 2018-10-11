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

#ifndef GIZMOTRANSFORM_H__
#define GIZMOTRANSFORM_H__

#include "GizmoTransformRender.h"
#include "IGizmo.h"

#include "stdafx.h"

extern float zubbiX;
extern float zubbiY;
extern float zubbiZ;

class CGizmoTransform : public IGizmo , protected CGizmoTransformRender
{
public:
    CGizmoTransform()
        : m_pMatrix(0),
          m_Offset(0, 0, 0),
          m_ScreenFactor(1),
          m_bUseSnap(false),
          mDisplayScale(1),
          mLocation(LOCATE_LOCAL),
          mEditPos(0),
          mEditScale(0),
          mEditQT(0),
          mMask(AXIS_ALL),
          mScreenWidth(1),
          mScreenHeight(1),
          mRayScale(1.0f, 1.0f, 1.0f)
    {
    }
    ~CGizmoTransform()
    {
    }

    virtual void SetEditMatrix(float *pMatrix)
    {
        m_pMatrix = (tmatrix*)pMatrix;
        //mTransform = NULL;

        mEditPos = mEditScale = NULL;
        mEditQT = NULL;
    }
    virtual void SetDisplayScale( float aScale ) { mDisplayScale = aScale; }

	virtual void SetRayScale(float x, float y, float z)
	{
		mRayScale.x = x;
		mRayScale.y = y;
		mRayScale.z = z;
	}

	virtual void SetScreenDimension( int screenWidth, int screenHeight)
    {
        mScreenWidth = screenWidth;
        mScreenHeight = screenHeight;
    }
    virtual void SetCameraMatrix(const float *Model, const float *Proj)
    {
        m_Model = *(tmatrix*)Model;
        m_Proj = *(tmatrix*)Proj;

        m_invmodel=m_Model;
        m_invmodel.Inverse();

        m_invproj=m_Proj;
        m_invproj.Inverse();

        m_CamSrc = m_invmodel.V4.position;
        m_CamDir = m_invmodel.V4.dir;
        m_CamUp = m_invmodel.V4.up;
    }



    // tools

    void BuildRay(int x, int y, tvector3 &rayOrigin, tvector3 &rayDir)
    {
        float frameX = (float)mScreenWidth;
        float frameY = (float)mScreenHeight;
        tvector3 screen_space;

        // device space to normalized screen space
        screen_space.x = ( ( (2.f * (float)x) / frameX ) - 1 ) / m_Proj.m[0][0];//.right.x;
        screen_space.y = -( ( (2.f * (float)y) / frameY ) - 1 ) / m_Proj.m[1][1];
        screen_space.z = -1.f;

        // screen space to world space

        rayOrigin = m_invmodel.V4.position;
        rayDir.TransformVector(screen_space, m_invmodel);
        rayDir.Normalize();
    }


    tvector3 GetVector(int vtID)
    {
        switch (vtID)
        {
        case 0: return tvector3(1,0,0);
        case 1: return tvector3(0,1,0);
        case 2: return tvector3(0,0,1);
        }
        return tvector3(0,0,0);
    }

    tvector3 GetTransformedVector(int vtID)
    {
        tvector3 vt;
        switch (vtID)
        {
        case 0: vt = tvector3(1,0,0); break;
        case 1: vt = tvector3(0,1,0); break;
        case 2: vt = tvector3(0,0,1); break;
        }
        if (mLocation == LOCATE_LOCAL)
        {
            vt.TransformVector(*m_pMatrix);
            vt.Normalize();
        }
        return vt;
    }
    virtual void SetAxisMask(unsigned int mask)
    {
        mMask = mask;
    }
    void ComputeScreenFactor()
    {
        const tmatrix viewproj = m_Model * m_Proj;
        //tvector4 trf = vector4( m_pMatrix->V4.position.x, m_pMatrix->V4.position.y, m_pMatrix->V4.position.z, 1.f);
        const tvector3 pos = GetTranslation();
        tvector4       trf = vector4(pos.x, pos.y, pos.z, 1.0f);
        trf.Transform( viewproj );
        const float w = ((trf.w < 0.0f) ? (-trf.w) : (trf.w));
        m_ScreenFactor = mDisplayScale * 0.15f * w;
    }

    tplane m_plan;
    tvector3 m_LockVertex;
    float m_Lng;

    tvector3 RayTrace2(const tvector3& rayOrigin, const tvector3& rayDir, const tvector3& norm, const tmatrix& mt, tvector3 trss, bool lockVTNorm = true)
    {
        extern tvector3 ptd;

        tvector3 df,inters;

        m_plan=vector4(GetTranslation(), norm);
        m_plan.RayInter(inters,rayOrigin,rayDir);
        df.TransformPoint( inters, mt );

        df /= GetScreenFactor();
		df *= mRayScale;
        m_LockVertex = df;
        if (lockVTNorm)
        {
            m_LockVertex.Normalize();
        }
        else
        {
            m_LockVertex = inters;
        }
        m_Lng = df.Length();

        return df;
    }

    float GetScreenFactor()
    {
        return m_ScreenFactor;
    }


    // snap
    virtual void UseSnap(bool bUseSnap)
    {
        m_bUseSnap = bUseSnap;
    }

    virtual bool IsUsingSnap()
    {
        return m_bUseSnap;
    }
    //transform
    virtual void ApplyTransform(tvector3& trans, bool bAbsolute) = 0;

    void SetLocation(LOCATION aLocation) { mLocation = aLocation; }
    LOCATION GetLocation() { return mLocation; }

    void SetOffset(float x, float y, float z) { m_Offset = tvector3(x, y, z); }
    inline tvector3 GetTranslation() const { return m_pMatrix->GetTranslation() + m_Offset; }

protected:
    tmatrix *m_pMatrix;
    tmatrix m_invmodel,m_invproj;
    tvector3 m_CamSrc,m_CamDir,m_CamUp,m_Offset;

    tmatrix m_svgMatrix;
    float m_ScreenFactor;
    bool m_bUseSnap;
    float mDisplayScale;

    LOCATION mLocation;

    tmatrix mWorkingMatrix; // for dissociated components
    tvector3 *mEditPos, *mEditScale ;
    tquaternion *mEditQT;
    //draw helpers

    unsigned int mMask;
    void SnapIt(float &pos, float &snap)
    {
        float sn = (float)fmod(pos,snap);
        if (fabs(sn)< (snap*0.25f) ) pos-=sn;
        if (fabs(sn)> (snap*0.75f) ) pos=( (pos-sn) + ((sn>0)?snap:-snap) );
    }

    int mScreenWidth, mScreenHeight;

	tvector3 mRayScale;
};

#endif // !defined(AFX_GIZMOTRANSFORM_H__913D353E_E420_4B1C_95F3_5A0258161651__INCLUDED_)
