
// STLReflectView.cpp : implementation of the CSTLReflectView class
#include "pch.h"


#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "STLReflect.h"
#endif
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<stb\stb_image_write.h>
#include "STLReflectDoc.h"
#include "STLReflectView.h"
#include"STLLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "PropertiesWnd.h"
#include "MainFrm.h"


// CSTLReflectView

IMPLEMENT_DYNCREATE(CSTLReflectView, CView)

BEGIN_MESSAGE_MAP(CSTLReflectView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CSTLReflectView::OnFilePrintPreview)
	//********************************
	ON_COMMAND(ID_FILE_OPEN, &CSTLReflectView::OnOpenFile)
	//********************************
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_MODELVIEW_SOLID, &CSTLReflectView::OnModelviewSolid)
	ON_COMMAND(ID_MODELVIEW_WIREFRAME, &CSTLReflectView::OnModelviewWireframe)
	ON_UPDATE_COMMAND_UI(ID_MODELVIEW_SOLID, &CSTLReflectView::OnUpdateModelviewSolid)
	ON_UPDATE_COMMAND_UI(ID_MODELVIEW_WIREFRAME, &CSTLReflectView::OnUpdateModelviewWireframe)
	ON_COMMAND(ID_FILE_NEW, &CSTLReflectView::OnFileNew)
	ON_COMMAND(ID_ROTATION_ENABLE, &CSTLReflectView::OnRotationEnable)
	ON_COMMAND(ID_ROTATION_DISABLE, &CSTLReflectView::OnRotationDisable)
	ON_UPDATE_COMMAND_UI(ID_ROTATION_ENABLE, &CSTLReflectView::OnUpdateRotationEnable)
	ON_UPDATE_COMMAND_UI(ID_ROTATION_DISABLE, &CSTLReflectView::OnUpdateRotationDisable)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(ID_COLOR_BACKGROUNDCOLOR, &CSTLReflectView::OnBnClickedButton1)
	ON_BN_CLICKED(ID_COLOR_LIGHTCOLOR, &CSTLReflectView::OnChangeLightColor)
	ON_BN_CLICKED(ID_COLOR_MODELCOLOR, &CSTLReflectView::OnSelectModelColor)
	ON_COMMAND(ID_LIGHT_ENABLELIGHT1, &CSTLReflectView::OnLightEnablelight1)
	ON_COMMAND(ID_LIGHT_ENABLELIGHT2, &CSTLReflectView::OnLightEnablelight2)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_ENABLELIGHT1, &CSTLReflectView::OnUpdateLightEnablelight1)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_ENABLELIGHT2, &CSTLReflectView::OnUpdateLightEnablelight2)
	ON_COMMAND(ID_BUTTON32785, &CSTLReflectView::OnResetView)
	ON_COMMAND(ID_FILE_SAVE, &CSTLReflectView::OnSaveAsImage)
	ON_COMMAND(ID_VIEW_TOGGLE_PLANE, &CSTLReflectView::OnTogglePlane)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOGGLE_PLANE, &CSTLReflectView::OnUpdateTogglePlane)
	ON_COMMAND(ID_MODELVIEW_SOLIDWITHEDGEHIGHLIGHT, &CSTLReflectView::OnModelviewSolidwithedgehighlight)
	ON_UPDATE_COMMAND_UI(ID_MODELVIEW_SOLIDWITHEDGEHIGHLIGHT, &CSTLReflectView::OnUpdateModelviewSolidwithedgehighlight)
	ON_COMMAND(ID_LIGHT_CEILING, &CSTLReflectView::OnLightCeiling)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_CEILING, &CSTLReflectView::OnUpdateLightCeiling)
	ON_COMMAND(ID_LIGHT_SPOTLIGHT, &CSTLReflectView::OnLightSpotlight)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SPOTLIGHT, &CSTLReflectView::OnUpdateLightSpotlight)
END_MESSAGE_MAP()

// CSTLReflectView construction/destruction
CSTLReflectView::CSTLReflectView() noexcept

: m_hDC(nullptr), m_hRC(nullptr), m_MouseX(0), m_MouseY(0), zoomFactor(3.0f), vertices(0), indices(0), m_IsWireframe(false), m_lightColor(1.0f, 1.0f, 1.0f), plane(nullptr)
{
	prevTime = glfwGetTime(); 
}
CSTLReflectView::~CSTLReflectView()
{
	if (m_hRC)
	{
		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
	if (m_hDC)
	{
		::ReleaseDC(m_hWnd, m_hDC);
		m_hDC = NULL;
	}
}
BOOL CSTLReflectView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}
// CSTLReflectView printing
void CSTLReflectView::InitializeOpenGL()
{
	m_hDC = ::GetDC(m_hWnd);
	if (!m_hDC)
	{
		AfxMessageBox(_T("Failed to get device context"));
		return;
	}

	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR), 1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA, 32,
		0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,
		24, 8, 0,
		PFD_MAIN_PLANE, 0, 0, 0, 0
	};

	int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
	if (pixelFormat == 0)
	{
		AfxMessageBox(_T("Failed to choose pixel format"));
		return;
	}
	SetPixelFormat(m_hDC, pixelFormat, &pfd);
	if (!SetPixelFormat(m_hDC, pixelFormat, &pfd))
	{
		AfxMessageBox(_T("Failed to set pixel format"));
		return;
	}

	m_hRC = wglCreateContext(m_hDC);
	if (!m_hRC)
	{
		AfxMessageBox(_T("Failed to create OpenGL context"));
		return;
	}
	wglMakeCurrent(m_hDC, m_hRC);
	if (!wglMakeCurrent(m_hDC, m_hRC))
	{
		AfxMessageBox(_T("Failed to activate OpenGL context"));
		return;
	}

	// Initialize GLAD
	if (!gladLoadGL()) {
		AfxMessageBox(_T("Failed to initialize GLAD"));
		return;
	}
	(GL_CULL_FACE);          // Disable face culling to see all faces
	glFrontFace(GL_CW);      // Clockwise winding as front face
	glEnable(GL_MULTISAMPLE); // Enable multisampling
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	wglMakeCurrent(NULL, NULL);

}
void CSTLReflectView::SetupVertices(const std::vector<Triangle>& triangles)
{
	vertices.clear();
	indices.clear();
	colors.clear();

	unsigned int index = 0;

	for (const auto& triangle : triangles)
	{
		vertices.insert(vertices.end(),
			{
				triangle.vertex1.x, triangle.vertex1.y, triangle.vertex1.z,
				triangle.normal1.x, triangle.normal1.y, triangle.normal1.z,

				triangle.vertex2.x, triangle.vertex2.y, triangle.vertex2.z,
				triangle.normal2.x, triangle.normal2.y, triangle.normal2.z,

				triangle.vertex3.x, triangle.vertex3.y, triangle.vertex3.z,
				triangle.normal3.x, triangle.normal3.y, triangle.normal3.z
			}
		);

		for (int i = 0; i < 3; ++i) 
		{
			colors.insert(colors.end(), { selectedRed, selectedGreen, selectedBlue });
		}
		indices.insert(indices.end(), { index, index + 1, index + 2 });
		index += 3;
	}

	// Create and bind VBOs and EBO
	VBO1.Delete();
	EBO1.Delete();
	VBO2.Delete();
	if (!shaderProgram)
	{
		shaderProgram = std::make_unique<Shader>("default.vert", "default.frag");
	}
	VAO1 = std::make_unique<VAO>();
	VAO1->Bind();

	// Position + Normal VBO
	VBO VBO1(vertices.data(), vertices.size() * sizeof(GLfloat));
	VAO1->LinkAttrib(VBO1, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);            // Position
	VAO1->LinkAttrib(VBO1, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float))); // Normal

	VBO2 = VBO(colors.data(), colors.size() * sizeof(GLfloat));
	VAO1->LinkAttrib(VBO2, 2, 3, GL_FLOAT, 3 * sizeof(float), (void*)0); // Color

	EBO EBO1(indices.data(), indices.size() * sizeof(GLuint));

	VAO1->Unbind();
	VBO1.Unbind();
	VBO2.Unbind();
	EBO1.Unbind();
}
void CSTLReflectView::SetupViewport(const CRect& rect)
{
	glViewport(0, 0, rect.Width(), rect.Height());
}
void CSTLReflectView::ClearBuffers()
{
	glEnable(GL_MULTISAMPLE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
glm::mat4 CSTLReflectView::SetupModelMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	glm::vec3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
	glm::vec3 maxBounds(FLT_MIN, FLT_MIN, FLT_MIN);

	for (const auto& triangle : triangles)
	{
		for (const glm::vec3& vertex : { triangle.vertex1, triangle.vertex2, triangle.vertex3 })
		{
			minBounds = glm::min(minBounds, vertex);
			maxBounds = glm::max(maxBounds, vertex);
		}
	}

	// Step 2: Calculate the center of the bounding box
	glm::vec3 center = (minBounds + maxBounds) / 2.0f;

	// Step 3: Calculate the maximum dimension
	glm::vec3 dimensions = maxBounds - minBounds;
	float maxDimension = glm::max(glm::max(dimensions.x, dimensions.y), dimensions.z);

	// Step 4: Translate the model to center it at the origin
	model = glm::translate(model, -center);

	// Step 5: Scale the model uniformly to fit exactly 2x2x2 cube
	if (maxDimension > 0.0f)
	{
		float scale = 1.0f / maxDimension; // Uniform scale factor
		model = glm::scale(model, glm::vec3(scale, scale, scale));

		// Adjust bounds to ensure exact fit
		glm::vec3 scaledDimensions = dimensions * scale;
		float minScaledDimension = glm::min(glm::min(scaledDimensions.x, scaledDimensions.y), scaledDimensions.z);

		if (fabs(minScaledDimension - 1.0f) > 0.001f)
		{
			// Fine-tune scaling to ensure all dimensions are exactly 2
			float adjustmentScale = 1.0f / minScaledDimension;
			model = glm::scale(model, glm::vec3(adjustmentScale, adjustmentScale, adjustmentScale));
		}
	}

	if (rotationEnabled)
	{
		float crntTime = static_cast<float>(GetTickCount64()) / 1000.0f; // Get current time in seconds
		float deltaTime = crntTime - prevTime;
		if (deltaTime > 1.0f / 60.0f) { // Target 60 FPS
			rotation += 50.0f * deltaTime; // Adjust rotation speed
			prevTime = crntTime;
		}

		model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
		InvalidateRect(NULL, FALSE); // Efficient redraw

	}
	// model = glm::rotate(model, glm::radians(rotation), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::translate(model, glm::vec3(modelPosX, modelPosY, 0));
	view = glm::translate(view, glm::vec3(0.0f, -0.1f, -zoomFactor));

	return model;
}
glm::mat4 CSTLReflectView::SetupViewMatrix()
{
	glm::mat4 view = glm::mat4(1.0f);
	view = glm::translate(view, glm::vec3(0.0f, -0.1f, -zoomFactor));
	return view;
}
glm::mat4 CSTLReflectView::SetupProjectionMatrix(const CRect& rect)
{
	float aspectRatio = static_cast<float>(rect.Width()) / rect.Height();
	return glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);
}
void CSTLReflectView::SetShaderUniforms(const glm::mat4& model, const glm::mat4& view, const glm::mat4& proj)
{
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram->ID, "proj"), 1, GL_FALSE, glm::value_ptr(proj));
}
void CSTLReflectView::SetupLighting()
{
	glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos(1.0f, 0.5f, 0.5f);

	if (light1Enabled)
	{
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "lightColor"), 1, glm::value_ptr(m_lightColor));
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "lightPos"), 1, glm::value_ptr(lightPos));
	}
	else
	{
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "lightColor"), 1, glm::value_ptr(glm::vec3(0.0f)));
	}

	if (light2Enabled)
	{
		glm::vec3 oppositeLightPos(-1.0f, 0.5f, 0.5f);
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "oppositeLightColor"), 1, glm::value_ptr(glm::vec3(1.0f)));
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "oppositeLightPos"), 1, glm::value_ptr(oppositeLightPos));
	}
	else
	{
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "oppositeLightColor"), 1, glm::value_ptr(glm::vec3(0.0f)));
	}

	if (toplightEnabled)
	{ 
	glm::vec3 toptLightColor(1.0f, 1.0f, 1.0f);
	glm::vec3 topLightPos(0.0f, 10.0f, 0.0f);
	glUniform3fv(glGetUniformLocation(shaderProgram->ID, "topLightColor"), 1, glm::value_ptr(toptLightColor));
	glUniform3fv(glGetUniformLocation(shaderProgram->ID, "topLightPos"), 1, glm::value_ptr(topLightPos));
	}
	else
	{
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "topLightColor"), 1, glm::value_ptr(glm::vec3(0.0f)));
	}

	if(spotlightEnabled)

	{
		glm::vec3 spotLightColor(1.0f, 1.0f, 1.0f);      // Neutral white light
		glm::vec3 spotLightPos(0.0f, 5.0f, 3.0f);        // Slightly above and in front
		glm::vec3 spotLightDir(0.0f, 0.0f, 0.0f);      // Pointing down and slightly forward
		constexpr float spotLightInnerAngle = glm::radians(20.0f); // Focused inner cone
		constexpr float spotLightOuterAngle = glm::radians(30.0f); // Smooth falloff

		// Transform the spotlight's position based on the model's transformations
		glm::mat4 modelMatrix = SetupModelMatrix();
		glm::vec4 spotlightPos = modelMatrix * glm::vec4(spotLightPos, 1.0f);
		glm::vec3 transformedSpotlightPos = glm::vec3(spotlightPos);
		glm::vec4 transformedSpotlightDir = modelMatrix * glm::vec4(spotLightDir, 0.0f);
		glm::vec3 transformedSpotlightDirVec = glm::normalize(glm::vec3(transformedSpotlightDir));

		// Pass the transformed spotlight position and direction to the shader
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "spotlightColor"), 1, glm::value_ptr(spotLightColor));
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "spotLightPos"), 1, glm::value_ptr(transformedSpotlightPos));
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "spotLightDir"), 1, glm::value_ptr(transformedSpotlightDirVec));
		glUniform1f(glGetUniformLocation(shaderProgram->ID, "spotLightInnerAngle"), spotLightInnerAngle);
		glUniform1f(glGetUniformLocation(shaderProgram->ID, "spotLightOuterAngle"), spotLightOuterAngle);
	}
	else
	{
		glUniform3fv(glGetUniformLocation(shaderProgram->ID, "spotlightColor"), 1, glm::value_ptr(glm::vec3(0.0f)));

	}
}
void CSTLReflectView::RenderModel()
{
	if (!VAO1)
		return;

	VAO1->Bind();

	// Solid Rendering (No wireframe, no silhouette)
	if (!m_IsWireframe && !m_IsSilhouetteView)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Fill polygons
		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isWireframe"), GL_FALSE);
		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isSilhouette"), GL_FALSE);
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
	}

	// Wireframe Rendering (For wireframe mode)
	if (m_IsWireframe && !m_IsSilhouetteView)
	{

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw lines (wireframe)
		//glLineWidth(1.5f); // 2.0f is an example thickness value (change as needed)

		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -1.0f);
        glUniform1i(glGetUniformLocation(shaderProgram->ID, "isWireframe"), GL_TRUE);
		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isSilhouette"), GL_FALSE);
		glUniform3f(glGetUniformLocation(shaderProgram->ID, "overrideColor"), 1.0f, 1.0f, 1.0f); // Wireframe color (White)
		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
		glDisable(GL_POLYGON_OFFSET_LINE);
	}
	// Silhouette Rendering (For silhouette edges)
	if (m_IsSilhouetteView)
	{
		// First, render the silhouette edges with a small offset to avoid z-fighting
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Draw the wireframe (edge highlights)

		glEnable(GL_POLYGON_OFFSET_LINE);
		glPolygonOffset(-1.0f, -1.0f); // Offset for silhouette lines to prevent z-fighting

		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isWireframe"), GL_TRUE);
		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isSilhouette"), GL_TRUE);
		glUniform3f(glGetUniformLocation(shaderProgram->ID, "overrideColor"), 0.0f, 0.0f, 0.0f); // Silhouette color (Black)

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

		glDisable(GL_POLYGON_OFFSET_LINE);

		// Then, render the solid model (infill) to avoid black patches, in fill mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Render solid model underneath

		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0f, 1.0f); // Offsets to avoid Z-fighting with filled polygons

		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isWireframe"), GL_FALSE);
		glUniform1i(glGetUniformLocation(shaderProgram->ID, "isSilhouette"), GL_FALSE);
		glUniform3f(glGetUniformLocation(shaderProgram->ID, "overrideColor"), 1.0f, 1.0f, 1.0f); // Default color for solid model

		glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);

		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	// Reset polygon mode to default (solid fill) after rendering
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	VAO1->Unbind();
}
void CSTLReflectView::OnDraw(CDC* /*pDC*/)
{
	CSTLReflectDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	wglMakeCurrent(m_hDC, m_hRC);
	if (!m_hDC || !m_hRC)
		return;

	CRect rect;
	GetClientRect(&rect);
	SetupViewport(rect);
	ClearBuffers();

	if (shaderProgram)
	{
		shaderProgram->Activate();
		if (m_planeEnabled)
		{
			plane = new Plane();
			if (plane)
			{
				plane->Render(*shaderProgram, glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(5.0f, 1.0f, 5.0f));
			}
		}
		glm::mat4 model = SetupModelMatrix();
		model = glm::translate(model, glm::vec3(0.0f, 0.5f, 0.0f));
		glm::mat4 view = SetupViewMatrix();
		glm::mat4 proj = SetupProjectionMatrix(rect);

		SetShaderUniforms(model, view, proj);
		SetupLighting();

		RenderModel();

		SwapBuffers(m_hDC);
	}

	if (rotationEnabled)
	{
		InvalidateRect(NULL, FALSE);
	}
}
void CSTLReflectView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	InitializeOpenGL(); 
	DragAcceptFiles(TRUE);
}
void CSTLReflectView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}
void CSTLReflectView::OnOpenFile()
{
	// Create an Open File Dialog to select the STL file
	CFileDialog fileDlg(TRUE, _T(".stl"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("STL Files (*.stl)|*.stl||"), this);

	// Show the dialog and check if the user selected a file
	if (fileDlg.DoModal() == IDOK)
	{
		CWaitCursor waitCursor;

		ResetViewParameters();
	
		// Get the full path of the selected file
		CString filePath = fileDlg.GetPathName();

		// Extract the file name and remove the ".stl" extension
		CString fileName = fileDlg.GetFileName();

		wchar_t titleString[255] = { '\0' };
		::LoadString(GetModuleHandle(NULL), AFX_IDS_APP_TITLE, titleString, 255);
		CString newTitle = titleString;
		newTitle.Append(L" - ");
		newTitle.Append(fileName);
		AfxGetMainWnd()->SetWindowTextW(newTitle);

		int dotIndex = fileName.ReverseFind('.');
		if (dotIndex != -1) {
			fileName = fileName.Left(dotIndex);  // Keep only the part before ".stl"
		}
		// Convert CString to std::string for the STL loader
		std::string stdFilePath(CT2A(filePath.GetString())); // Use GetString() for better clarity

		// Load the STL file using the STLLoader
		STLLoader load;
		triangles = load.loadSTL(stdFilePath);
		SetupVertices(triangles);
		
		if (triangles.empty())
		{
			AfxMessageBox(_T("Failed to load STL file."), MB_OK | MB_ICONERROR);
			return;
		}
		// Calculate total surface area and volume
		float totalArea = 0.0f;
		float totalVolume = 0.0f;
		
		load.calculateSurfaceAreaAndVolume(triangles, totalArea, totalVolume);
		// Display the number of triangles loaded

		CPropertiesWnd* pPropertiesWnd = ((CMainFrame*)AfxGetMainWnd())->GetPropertiesWnd();

		if (pPropertiesWnd != nullptr) {

			pPropertiesWnd->UpdateSTLProperties(fileName, filePath, static_cast<int>(triangles.size()), totalArea, totalVolume);

			
		}
		else {
			AfxMessageBox(_T("Failed to access the Properties Pane."), MB_OK | MB_ICONERROR);
		}
	}
	Invalidate();
}
BOOL CSTLReflectView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}
void CSTLReflectView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}
void CSTLReflectView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}
void CSTLReflectView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}
void CSTLReflectView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	//theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

// CSTLReflectView diagnostics

#ifdef _DEBUG
void CSTLReflectView::AssertValid() const
{
	CView::AssertValid();
}
void CSTLReflectView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
CSTLReflectDoc* CSTLReflectView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSTLReflectDoc)));
	return (CSTLReflectDoc*)m_pDocument;
}
#endif //_DEBUG


void CSTLReflectView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_hDC || m_hRC) // Ensure OpenGL context exists
	{
		// Update the OpenGL viewport with the new width and height
		glViewport(0, 0, cx, cy);
		Invalidate();  // Request window repaint to apply changes
	}
}
void CSTLReflectView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
	case VK_LEFT:
		rotation -= 1.0f; // Rotate left
		break;
	case VK_RIGHT:
		rotation += 1.0f; // Rotate right
		break;
	case VK_UP:
		zoomFactor *= 1.1f; // Zoom in (10% increase per press)
		if (zoomFactor > 90.0f) zoomFactor = 90.0f; // Set an upper limit
		break;
	case VK_DOWN:
		zoomFactor *= 0.9f; // Zoom out (10% decrease per press)
		if (zoomFactor < 0.001f) zoomFactor = 0.001f; // Prevent excessive zoom out
		break;
	default:
		break;
	}

	Invalidate(FALSE); // Redraw the view without erasing background
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
void CSTLReflectView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (nFlags & MK_LBUTTON) // Left mouse button held down for rotation
	{
		// Calculate the change in position from the previous mouse position
		float deltaX = (point.x - m_MouseX) * 0.2f; // Change in X for rotation around Y axis
		float deltaY = (point.y - m_MouseY) * 0.2f; // Change in Y for rotation around X axis

		// Update the rotation angles
		rotationX += deltaY;
		rotationY += deltaX;

		// Optional: Limit rotation angles to avoid flipping
		if (rotationX > 89.0f) rotationX = 89.0f;
		if (rotationX < -89.0f) rotationX = -89.0f;
	}
	else if (nFlags & MK_RBUTTON) // Right mouse button held down for dragging
	{
		// Calculate the change in position for model translation
		float deltaX = (point.x - m_MouseX) * 0.01f; // Adjust the sensitivity as needed
		float deltaY = (point.y - m_MouseY) * 0.01f;

		// Update the model position
		modelPosX += deltaX;
		modelPosY -= deltaY; // Invert Y direction for correct dragging

		// Store updated position for further mouse moves
		m_MouseX = point.x;
		m_MouseY = point.y;
	}

	// Update the mouse position for the next move
	m_MouseX = point.x;
	m_MouseY = point.y;


	Invalidate(FALSE);
	CView::OnMouseMove(nFlags, point);
}
BOOL CSTLReflectView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (zDelta > 0)
	{
		zoomFactor += 1.1f; // Zoom in
	}
	else
	{
		zoomFactor -= 0.9f; // Zoom out
		if (zoomFactor < 0.001f) 
			zoomFactor = 0.001f; // Prevent negative zoom
	}

	
	Invalidate(FALSE);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}
void CSTLReflectView::OnModelviewSolid()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	// Set Solid Mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	m_IsWireframe = false;
	m_IsSilhouetteView = false; // Ensure silhouette mode is off
	Invalidate(FALSE); // Force a redraw
}
void CSTLReflectView::OnModelviewWireframe()
{
	// Set Wireframe Mode
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	m_IsWireframe = true;
	m_IsSilhouetteView = false; // Ensure silhouette mode is off
	Invalidate(FALSE); // Force a redraw
}
void CSTLReflectView::OnModelviewSolidwithedgehighlight()
{
	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
	// Toggle Silhouette Mode
	m_IsSilhouetteView = !m_IsSilhouetteView;

	// Silhouette mode overrides wireframe but keeps the polygon mode as GL_FILL
	if (m_IsSilhouetteView)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		m_IsWireframe = false;
	}

	Invalidate(FALSE); // Force a redraw
}
void CSTLReflectView::OnUpdateModelviewSolid(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(!m_IsWireframe && !m_IsSilhouetteView);
}
void CSTLReflectView::OnUpdateModelviewWireframe(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_IsWireframe);
}
void CSTLReflectView::OnUpdateModelviewSolidwithedgehighlight(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_IsSilhouetteView);
}
void CSTLReflectView::OnFileNew()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	OnOpenFile();
}
void CSTLReflectView::OnRotationEnable()
{
	// TODO: Add your command handler code here
	
	rotationEnabled = true;
	
}
void CSTLReflectView::OnRotationDisable()
{
	// TODO: Add your command handler code here
	
	rotationEnabled = false;
	
}
void CSTLReflectView::OnUpdateRotationEnable(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!rotationEnabled);
}
void CSTLReflectView::OnUpdateRotationDisable(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(rotationEnabled);
}
void CSTLReflectView::OnDropFiles(HDROP hDropInfo)
{
	// Get the number of files dropped
	UINT fileCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);

	// Loop through the dropped files
	for (UINT i = 0; i < fileCount; ++i)
	{
		// Get the length of the file path
		UINT length = ::DragQueryFile(hDropInfo, i, NULL, 0);

		// Allocate a buffer for the file path
		CString filePath;
		::DragQueryFile(hDropInfo, i, filePath.GetBufferSetLength(length + 1), length + 1);
		filePath.ReleaseBuffer();


		CString fileExtension = filePath.Right(4).MakeLower();
		if (fileExtension != _T(".stl"))
		{
			AfxMessageBox(_T("Only .stl files are supported."), MB_OK | MB_ICONWARNING);
			continue;
		}
		// Load the file or perform necessary action
		AfxMessageBox(_T("File dropped: ") + filePath);

		// Example: Call a function to load the model
		LoadNewModel(filePath);
	}

	// Free the memory used by the drag-and-drop operation
	::DragFinish(hDropInfo);

	CView::OnDropFiles(hDropInfo);

}
void CSTLReflectView::LoadNewModel(const CString& filePath)
{
	// Extract the file name and remove the ".stl" extension
	CString fileName = filePath.Mid(filePath.ReverseFind('\\') + 1);
	int dotIndex = fileName.ReverseFind('.');
	if (dotIndex != -1) {
		fileName = fileName.Left(dotIndex);  // Keep only the part before ".stl"
	}
	wchar_t titleString[255] = { '\0' };
	::LoadString(GetModuleHandle(NULL), AFX_IDS_APP_TITLE, titleString, 255);
	CString newTitle = titleString;
	newTitle.Append(L" - ");
	newTitle.Append(fileName);
	AfxGetMainWnd()->SetWindowTextW(newTitle);
	// Convert CString to std::string for the STL loader
	std::string stdFilePath(CT2A(filePath.GetString())); // Use GetString() for better clarity
	ResetViewParameters();
	// Load the STL file using the STLLoader
	STLLoader load;
	triangles = load.loadSTL(stdFilePath);
	SetupVertices(triangles);

	// Check if the STL file was loaded successfully
	if (triangles.empty())
	{
		AfxMessageBox(_T("Failed to load STL file."), MB_OK | MB_ICONERROR);
		return;
	}

	// Calculate total surface area and volume
	float totalArea = 0.0f;
	float totalVolume = 0.0f;
	load.calculateSurfaceAreaAndVolume(triangles, totalArea, totalVolume);

	// Update the Properties Window
	CPropertiesWnd* pPropertiesWnd = ((CMainFrame*)AfxGetMainWnd())->GetPropertiesWnd();
	if (pPropertiesWnd != nullptr) {
		pPropertiesWnd->UpdateSTLProperties(fileName, filePath, static_cast<int>(triangles.size()), totalArea, totalVolume);

	}
	else {
		AfxMessageBox(_T("Failed to access the Properties Pane."), MB_OK | MB_ICONERROR);
	}

	// Redraw the view
	Invalidate();
}
void CSTLReflectView::OnBnClickedButton1()
{
	bool checkState;
	checkState = rotationEnabled;
	rotationEnabled = false;
	// TODO: Add your control notification handler code here
	CColorDialog colorDlg; // MFC color dialog
	if (colorDlg.DoModal() == IDOK)
	{
		COLORREF color = colorDlg.GetColor();
		float red = GetRValue(color) / 255.0f;
		float green = GetGValue(color) / 255.0f;
		float blue = GetBValue(color) / 255.0f;

		// Set OpenGL clear color
		glClearColor(red, green, blue, 1.0f);
		Invalidate(); // Redraw the scene to apply the new color
	}
	rotationEnabled = checkState;
}
void CSTLReflectView::OnChangeLightColor()
{
	bool checkState;
	checkState = rotationEnabled;
	rotationEnabled = false;
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK)
	{
		COLORREF color = colorDlg.GetColor();

		// Convert COLORREF to RGB values in the range 0 to 1
		float r = GetRValue(color) / 255.0f;
		float g = GetGValue(color) / 255.0f;
		float b = GetBValue(color) / 255.0f;

		// Update light color
		m_lightColor = glm::vec3(r, g, b);

		// Update the shader with the new light color
		if (shaderProgram)
		{
			shaderProgram->Activate();
			int lightColorLoc = glGetUniformLocation(shaderProgram->ID, "lightColor");
			glUniform3fv(lightColorLoc, 1, glm::value_ptr(m_lightColor));
		}

		// Redraw the scene
		Invalidate();
	}
	rotationEnabled = checkState;
}
void CSTLReflectView::UpdateColors(float red, float green, float blue)
{
	selectedRed = red;
	selectedGreen = green;
	selectedBlue = blue;

	for (size_t i = 0; i < colors.size(); i += 3)
	{
		colors[i] = red;
		colors[i + 1] = green;
		colors[i + 2] = blue;
	}
	VBO2.Bind();
	glBufferSubData(GL_ARRAY_BUFFER, 0, colors.size() * sizeof(GLfloat), colors.data());
	VBO2.Unbind();

	Invalidate();
}
void CSTLReflectView::OnSelectModelColor()
{
	bool checkState;
	checkState = rotationEnabled;
	rotationEnabled = false;
	CColorDialog colorDlg;
	if (colorDlg.DoModal() == IDOK)
	{
		COLORREF selectedColor = colorDlg.GetColor();
		float red = GetRValue(selectedColor) / 255.0f;
		float green = GetGValue(selectedColor) / 255.0f;
		float blue = GetBValue(selectedColor) / 255.0f;

		UpdateColors(red, green, blue); 
	}
	rotationEnabled = checkState;
}
void CSTLReflectView::OnLightEnablelight1()
{
	light1Enabled = !light1Enabled;
	
	Invalidate(); 
}
void CSTLReflectView::OnLightEnablelight2()
{
	light2Enabled = !light2Enabled;
	
	Invalidate();
}
void CSTLReflectView::OnLightCeiling()
{
	// TODO: Add your command handler code here
	toplightEnabled = !toplightEnabled;

	Invalidate();
}
void CSTLReflectView::OnLightSpotlight()
{
	// TODO: Add your command handler code here
	spotlightEnabled = !spotlightEnabled;

	Invalidate();
}
void CSTLReflectView::OnUpdateLightEnablelight1(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(light1Enabled);
}
void CSTLReflectView::OnUpdateLightEnablelight2(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(light2Enabled);
}
void CSTLReflectView::OnUpdateLightCeiling(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(toplightEnabled);
}
void CSTLReflectView::OnUpdateLightSpotlight(CCmdUI* pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(spotlightEnabled);
}
void CSTLReflectView::ResetViewParameters()
{
	rotation = 0.0f;
	rotationX = 0.0f;
	rotationY = 0.0f;
	modelPosX = 0.0f;
	modelPosY = 0.0f;
	zoomFactor =3.0f; 
	selectedRed = 0.5f;
	selectedGreen = 0.5f;
	selectedBlue = 0.5f;
	Invalidate(FALSE);
}
void CSTLReflectView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	modelPosX = 0.0f;
	modelPosY = 0.0f;
	zoomFactor = 3.0f; 
	rotationX = 0.0f;
	rotationY = 0.0f;

	Invalidate(FALSE);
	CView::OnLButtonDblClk(nFlags, point);
}
void CSTLReflectView::OnResetView()
{
	bool checkState;
	checkState = rotationEnabled;
	rotationEnabled = false;
	// Show a confirmation message box
	int result = AfxMessageBox(
		_T("Are you sure you want to reset the view?"),
		MB_YESNO | MB_ICONQUESTION | MB_TOPMOST
	);

	// If the user clicks "No", do not reset the view
	if (result != IDYES)
	{
		rotationEnabled = checkState;
		return;
	}
	ResetViewParameters();
	m_IsWireframe = false;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	rotationEnabled = false;
	light1Enabled = true;
	light2Enabled = true;
	toplightEnabled = true;
	spotlightEnabled = true;
	m_IsWireframe = false;
	m_IsSilhouetteView = false;
	UpdateColors(0.5f, 0.5f, 0.5f);

	glClearColor(0.8f, 0.8f, 0.8f, 1.0f);

	m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	modelPosX = 0.0f;
	modelPosY = 0.0f;
	zoomFactor = 3.0f;

	Invalidate(FALSE);
}
void CSTLReflectView::SaveViewportAsImage(const CString& filePath)
{
	// Get the viewport dimensions
	CRect rect;
	GetClientRect(&rect);
	int width = rect.Width();
	int height = rect.Height();

	// Allocate memory for pixel data (RGBA format)
	std::vector<GLubyte> pixels(width * height * 4);

	// Bind the OpenGL context
	wglMakeCurrent(m_hDC, m_hRC);

	// Read the pixels from the frame buffer
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	// Unbind the OpenGL context
	wglMakeCurrent(nullptr, nullptr);

	// Flip the image vertically (OpenGL origin is bottom-left, while images expect top-left)
	for (int row = 0; row < height / 2; ++row)
	{
		int oppositeRow = height - row - 1;
		for (int col = 0; col < width * 4; ++col)
		{
			std::swap(pixels[row * width * 4 + col], pixels[oppositeRow * width * 4 + col]);
		}
	}

	// Convert CString to std::string
	std::string stdFilePath(CT2A(filePath.GetString()));

	// Save the image as a PNG file
	if (!stbi_write_png(stdFilePath.c_str(), width, height, 4, pixels.data(), width * 4))
	{
		AfxMessageBox(_T("Failed to save image."), MB_OK | MB_ICONERROR);
	}
	else
	{
		AfxMessageBox(_T("Image saved successfully."), MB_OK | MB_ICONINFORMATION);
	}
}
void CSTLReflectView::OnSaveAsImage()
{
	CFileDialog saveDlg(FALSE, _T(".png"), _T(""), OFN_OVERWRITEPROMPT, _T("PNG Files (*.png)|*.png||"));
	if (saveDlg.DoModal() == IDOK)
	{
		CString filePath = saveDlg.GetPathName();
		SaveViewportAsImage(filePath);
	}
}
void CSTLReflectView::OnTogglePlane()
{
	m_planeEnabled = !m_planeEnabled; // Toggle the plane's visibility
	Invalidate(); // Force redraw
}
void CSTLReflectView::OnUpdateTogglePlane(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_planeEnabled); // Reflect the current state in the menu
}