
#include "pch.h"
#include "framework.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "STLReflect.h"




#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}



BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_EXPAND_ALL, OnExpandAllProperties)
	ON_UPDATE_COMMAND_UI(ID_EXPAND_ALL, OnUpdateExpandAllProperties)
	ON_COMMAND(ID_SORTPROPERTIES, OnSortProperties)
	ON_UPDATE_COMMAND_UI(ID_SORTPROPERTIES, OnUpdateSortProperties)
	ON_COMMAND(ID_PROPERTIES1, OnProperties1)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES1, OnUpdateProperties1)
	ON_COMMAND(ID_PROPERTIES2, OnProperties2)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES2, OnUpdateProperties2)
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar message handlers

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd () == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() -(m_nComboHeight+cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create combo:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("Failed to create Properties Combo \n");
		return -1;      // fail to create
	}

	m_wndObjectCombo.AddString(_T("Application"));
	m_wndObjectCombo.AddString(_T("Properties Window"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("Failed to create Properties Grid \n");
		return -1;      // fail to create
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* Is locked */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* Locked */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	AdjustLayout();
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CPropertiesWnd::OnExpandAllProperties()
{
	m_wndPropList.ExpandAll();
}

void CPropertiesWnd::OnUpdateExpandAllProperties(CCmdUI* /* pCmdUI */)
{
}

void CPropertiesWnd::OnSortProperties()
{
	m_wndPropList.SetAlphabeticMode(!m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnUpdateSortProperties(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndPropList.IsAlphabeticMode());
}

void CPropertiesWnd::OnProperties1()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties1(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::OnProperties2()
{
	// TODO: Add your command handler code here
}

void CPropertiesWnd::OnUpdateProperties2(CCmdUI* /*pCmdUI*/)
{
	// TODO: Add your command update UI handler code here
}

void CPropertiesWnd::InitPropList()
{
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();


	// Add an empty group for STL File Properties initially
	CMFCPropertyGridProperty* pGroupSTL = new CMFCPropertyGridProperty(_T("STL File Properties"));
	pGroupSTL->AddSubItem(new CMFCPropertyGridProperty(_T("File Name"), _T("No File Loaded"), _T("Name of STL file")));
	pGroupSTL->AddSubItem(new CMFCPropertyGridProperty(_T("File Path"), _T("No File Loaded"), _T("The path to the STL file")));
	pGroupSTL->AddSubItem(new CMFCPropertyGridProperty(_T("Number of Triangles"),(long) 0, _T("The number of triangles in the STL model")));
	pGroupSTL->AddSubItem(new CMFCPropertyGridProperty(_T("Surface Area"), 0.0f, _T("The surface area of the STL model")));
	pGroupSTL->AddSubItem(new CMFCPropertyGridProperty(_T("Volume"), 0.0f, _T("The volume of the STL model")));
	m_wndPropList.AddProperty(pGroupSTL);


	//CMFCPropertyGridProperty* pGroup1 = new CMFCPropertyGridProperty(_T("Appearance"));

	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("3D Look"), (_variant_t) false, _T("Specifies the window's font will be non-bold and controls will have a 3D border")));

	//CMFCPropertyGridProperty* pProp = new CMFCPropertyGridProperty(_T("Border"), _T("Dialog Frame"), _T("One of: None, Thin, Resizable, or Dialog Frame"));
	//pProp->AddOption(_T("None"));
	//pProp->AddOption(_T("Thin"));
	//pProp->AddOption(_T("Resizable"));
	//pProp->AddOption(_T("Dialog Frame"));
	//pProp->AllowEdit(FALSE);

	//pGroup1->AddSubItem(pProp);
	//pGroup1->AddSubItem(new CMFCPropertyGridProperty(_T("Caption"), (_variant_t) _T("About"), _T("Specifies the text that will be displayed in the window's title bar")));

	//m_wndPropList.AddProperty(pGroup1);

	//CMFCPropertyGridProperty* pSize = new CMFCPropertyGridProperty(_T("Window Size"), 0, TRUE);

	//pProp = new CMFCPropertyGridProperty(_T("Height"), (_variant_t) 250l, _T("Specifies the window's height"));
	//pProp->EnableSpinControl(TRUE, 50, 300);
	//pSize->AddSubItem(pProp);

	//pProp = new CMFCPropertyGridProperty( _T("Width"), (_variant_t) 150l, _T("Specifies the window's width"));
	//pProp->EnableSpinControl(TRUE, 50, 200);
	//pSize->AddSubItem(pProp);

	//m_wndPropList.AddProperty(pSize);

	//CMFCPropertyGridProperty* pGroup2 = new CMFCPropertyGridProperty(_T("Font"));

	//LOGFONT lf;
	//CFont* font = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	//font->GetLogFont(&lf);

	//_tcscpy_s(lf.lfFaceName, _T("Arial"));

	//pGroup2->AddSubItem(new CMFCPropertyGridFontProperty(_T("Font"), lf, CF_EFFECTS | CF_SCREENFONTS, _T("Specifies the default font for the window")));
	//pGroup2->AddSubItem(new CMFCPropertyGridProperty(_T("Use System Font"), (_variant_t) true, _T("Specifies that the window uses MS Shell Dlg font")));

	//m_wndPropList.AddProperty(pGroup2);

	//CMFCPropertyGridProperty* pGroup3 = new CMFCPropertyGridProperty(_T("Misc"));
	//pProp = new CMFCPropertyGridProperty(_T("(Name)"), _T("Application"));
	//pProp->Enable(FALSE);
	//pGroup3->AddSubItem(pProp);

	//CMFCPropertyGridColorProperty* pColorProp = new CMFCPropertyGridColorProperty(_T("Window Color"), RGB(210, 192, 254), nullptr, _T("Specifies the default window color"));
	//pColorProp->EnableOtherButton(_T("Other..."));
	//pColorProp->EnableAutomaticButton(_T("Default"), ::GetSysColor(COLOR_3DFACE));
	//pGroup3->AddSubItem(pColorProp);

	//static const TCHAR szFilter[] = _T("Icon Files(*.ico)|*.ico|All Files(*.*)|*.*||");
	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Icon"), TRUE, _T(""), _T("ico"), 0, szFilter, _T("Specifies the window icon")));

	//pGroup3->AddSubItem(new CMFCPropertyGridFileProperty(_T("Folder"), _T("c:\\")));

	//m_wndPropList.AddProperty(pGroup3);

	/*CMFCPropertyGridProperty* pGroup4 = new CMFCPropertyGridProperty(_T("Hierarchy"));

	CMFCPropertyGridProperty* pGroup41 = new CMFCPropertyGridProperty(_T("First sub-level"));
	pGroup4->AddSubItem(pGroup41);

	CMFCPropertyGridProperty* pGroup411 = new CMFCPropertyGridProperty(_T("Second sub-level"));
	pGroup41->AddSubItem(pGroup411);

	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 1"), (_variant_t) _T("Value 1"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 2"), (_variant_t) _T("Value 2"), _T("This is a description")));
	pGroup411->AddSubItem(new CMFCPropertyGridProperty(_T("Item 3"), (_variant_t) _T("Value 3"), _T("This is a description")));

	pGroup4->Expand(FALSE);
	m_wndPropList.AddProperty(pGroup4);
	m_wndPropList.MarkModifiedProperties();*/
}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}
void CPropertiesWnd::UpdateSTLProperties(const CString& fileName, const CString& filePath,
	int numTriangles, float surfaceArea, float volume)
{
	
	// Find or create the "STL File Properties" group
	CMFCPropertyGridProperty* pGroupSTL = nullptr;
	pGroupSTL = m_wndPropList.GetProperty(NULL); 

	if (pGroupSTL == nullptr) {
		// Create the group if it doesn't exist
		pGroupSTL = new CMFCPropertyGridProperty(_T("STL File Properties"));
		m_wndPropList.AddProperty(pGroupSTL);
	}
	CMFCPropertyGridProperty* pFileNameProp = pGroupSTL->GetSubItem(0);
	if (pFileNameProp == nullptr) {
		pFileNameProp = new CMFCPropertyGridProperty(_T("File Name"), COleVariant(fileName), _T("The name of the STL file (without extension)"));
		pFileNameProp->AllowEdit(FALSE); // Make the "File Name" property read-only
		pGroupSTL->AddSubItem(pFileNameProp);
	}
	else {
		pFileNameProp->SetValue(COleVariant(fileName));
	}
	// Add or update properties
	// File path
	CMFCPropertyGridProperty* pFilePathProp = pGroupSTL->GetSubItem(1);
	if (pFilePathProp == nullptr) {
		pFilePathProp = new CMFCPropertyGridProperty(_T("File Path"), COleVariant(filePath), _T("The path to the STL file"));
		pFilePathProp->AllowEdit(FALSE);
		pGroupSTL->AddSubItem(pFilePathProp);
	}
	else {
		pFilePathProp->SetValue(COleVariant(filePath));
	}

	// Number of triangles
	CMFCPropertyGridProperty* pNumTrianglesProp = pGroupSTL->GetSubItem(2);
	if (pNumTrianglesProp == nullptr) {
		pNumTrianglesProp = new CMFCPropertyGridProperty(_T("Number of Triangles"), COleVariant((long)numTriangles), _T("Number of triangles in the STL file"));
		pNumTrianglesProp->AllowEdit(FALSE);
		pGroupSTL->AddSubItem(pNumTrianglesProp);
	}
	else {
		pNumTrianglesProp->SetValue(COleVariant((long)numTriangles));
	}

	// Surface area
	CMFCPropertyGridProperty* pSurfaceAreaProp = pGroupSTL->GetSubItem(3);
	if (pSurfaceAreaProp == nullptr) {
		pSurfaceAreaProp = new CMFCPropertyGridProperty(_T("Surface Area"), COleVariant((double)surfaceArea), _T("Total surface area of the STL model"));
		pSurfaceAreaProp->AllowEdit(FALSE);
		pGroupSTL->AddSubItem(pSurfaceAreaProp);
	}
	else {
		pSurfaceAreaProp->SetValue(COleVariant((float)surfaceArea));
	}

	// Volume
	CMFCPropertyGridProperty* pVolumeProp = pGroupSTL->GetSubItem(4);
	if (pVolumeProp == nullptr) {
		pVolumeProp = new CMFCPropertyGridProperty(_T("Volume"), COleVariant((double)volume), _T("Total volume of the STL model"));
		pGroupSTL->AddSubItem(pVolumeProp);
		pVolumeProp->AllowEdit(FALSE);
		pGroupSTL->AllowEdit(FALSE);

	}
	else {
		pVolumeProp->SetValue(COleVariant((float)volume));
	}
	
	m_wndPropList.EnableWindow(FALSE);
}
