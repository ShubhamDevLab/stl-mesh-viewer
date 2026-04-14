#include "pch.h"
#include "MyDockablePane.h"

// CMyDockablePane

IMPLEMENT_DYNAMIC(CMyDockablePane, CDockablePane)

CMyDockablePane::CMyDockablePane() {}
CMyDockablePane::~CMyDockablePane() {}

BEGIN_MESSAGE_MAP(CMyDockablePane, CDockablePane)
    ON_WM_CREATE()
END_MESSAGE_MAP()

// CMyDockablePane message handlers

int CMyDockablePane::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CDockablePane::OnCreate(lpCreateStruct) == -1)
        return -1;

    // Create a CStatic control inside the dockable pane to display the message
    m_staticMessage.Create(_T("Information goes here"), WS_CHILD | WS_VISIBLE | SS_LEFT, CRect(10, 10, 200, 50), this);

    return 0;
}

void CMyDockablePane::SetMessage(const CString& message)
{
    m_staticMessage.SetWindowText(message);
}
