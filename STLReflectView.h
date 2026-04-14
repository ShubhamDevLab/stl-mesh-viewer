
// STLReflectView.h : interface of the CSTLReflectView class
//

#pragma once
#include <glad/glad.h>
#include<GLFW/glfw3.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include"STLLoader.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"shaderClass.h"
#include "Plane.h"

class CSTLReflectView : public CView
{
protected: // create from serialization only
	CSTLReflectView() noexcept;
	DECLARE_DYNCREATE(CSTLReflectView)

// Attributes
public:
	CSTLReflectDoc* GetDocument() const;

	
	
// Operations
public:

// Overrides
public:
	
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual void OnInitialUpdate() override; // Called first time after construct
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSTLReflectView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	HDC m_hDC;     // Handle to device context
	HGLRC m_hRC;   // Handle to OpenGL rendering context
	std::unique_ptr<Shader> shaderProgram;
	std::unique_ptr<VAO> VAO1;
	VBO VBO1, VBO2;
	EBO EBO1;
	
	Plane* plane;


	int m_MouseX;
	int m_MouseY;
	
	float selectedRed = 0.5f;
	float selectedGreen = 0.5f;
	float selectedBlue = 0.5f;

	float modelPosX = 0.0f;
	float modelPosY = 0.0f;

	float zoomFactor;

	float rotationX = 0.0f; 
	float rotationY = 0.0f;
	

	bool m_IsWireframe;
	bool rotationEnabled = false;
	bool light1Enabled = true;
	bool light2Enabled = true;
	bool toplightEnabled = true;
	bool spotlightEnabled = true;

	double prevTime;
	float rotation = 0.2f;
	bool m_IsSilhouetteView = false; // Initially disabled
	BOOL m_planeEnabled = true;
	glm::vec3 m_lightColor;



	std::vector<Triangle> triangles;
	std::vector<GLfloat> vertices;
	std::vector<GLuint>  indices;
	std::vector<GLfloat> colors;

	void InitializeOpenGL(); 
	void SetupVertices(const std::vector<Triangle>& triangles);
	void SetupViewport(const CRect& rect);
	void ClearBuffers();
	glm::mat4 SetupModelMatrix();
	glm::mat4 SetupViewMatrix();
	glm::mat4 SetupProjectionMatrix(const CRect& rect);
	void SetShaderUniforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj);
	void SetupLighting();
	void RenderModel();
	void LoadNewModel(const CString& filePath);
	void ResetViewParameters();
	void SaveViewportAsImage(const CString& filePath);
// Generated message map functions
protected:
	afx_msg

	
	//********************************************************************
	afx_msg void OnOpenFile(); // Declare the OnOpenFile function
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnResetView();
	afx_msg void OnSaveAsImage();
	afx_msg void OnTogglePlane();
	afx_msg void OnUpdateTogglePlane(CCmdUI* pCmdUI);
	//********************************************************************
	void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnModelviewSolid();
	afx_msg void OnModelviewWireframe();
	afx_msg void OnUpdateModelviewSolid(CCmdUI* pCmdUI);
	afx_msg void OnUpdateModelviewWireframe(CCmdUI* pCmdUI);
	afx_msg void OnFileNew();
	afx_msg void OnRotationEnable();
	afx_msg void OnRotationDisable();
	afx_msg void OnUpdateRotationEnable(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRotationDisable(CCmdUI* pCmdUI);
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButton1();
	afx_msg void OnChangeLightColor();
	afx_msg void UpdateColors(float red, float green, float blue);
	afx_msg void OnSelectModelColor();
	afx_msg void OnLightEnablelight1();
	afx_msg void OnLightEnablelight2();
	afx_msg void OnUpdateLightEnablelight1(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLightEnablelight2(CCmdUI* pCmdUI);
	afx_msg void OnModelviewSolidwithedgehighlight();
	afx_msg void OnUpdateModelviewSolidwithedgehighlight(CCmdUI* pCmdUI);
	afx_msg void OnLightCeiling();
	afx_msg void OnUpdateLightCeiling(CCmdUI* pCmdUI);
	afx_msg void OnLightSpotlight();
	afx_msg void OnUpdateLightSpotlight(CCmdUI* pCmdUI);
};

#ifndef _DEBUG  // debug version in STLReflectView.cpp
inline CSTLReflectDoc* CSTLReflectView::GetDocument() const
   { return reinterpret_cast<CSTLReflectDoc*>(m_pDocument); }
#endif

