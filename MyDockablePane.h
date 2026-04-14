#pragma once
#include "afxwin.h"

class CMyDockablePane : public CDockablePane
{
    DECLARE_DYNAMIC(CMyDockablePane)

public:
    CMyDockablePane();
    virtual ~CMyDockablePane();

    void SetMessage(const CString& message);

protected:
    CStatic m_staticMessage;

    // Generated message map functions
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    DECLARE_MESSAGE_MAP()
};
