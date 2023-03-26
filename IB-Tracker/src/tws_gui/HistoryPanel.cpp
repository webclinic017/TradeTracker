
#include "framework.h"
#include "ib-tracker.h"
#include "HistoryPanel.h"
#include "tws-client.h"
#include "SuperLabel.h"
#include "Themes.h"



//' ========================================================================================
//' NavPanel Window procedure
//' ========================================================================================
LRESULT CALLBACK HistoryPanel_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {

    case WM_SIZE:
    {
        break;
    }


    case WM_ERASEBKGND:
    {
        // Handle all of the painting in WM_PAINT
        return TRUE;
        break;
    }


    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc = BeginPaint(hWnd, &ps);
        
        Graphics graphics(hdc);

        DWORD nBackColor = GetThemeColor(ThemeElement::HistoryPanelBack);

        // Create the background brush
        SolidBrush backBrush(nBackColor);

        // Paint the background using brush.
        graphics.FillRectangle(&backBrush, (int)ps.rcPaint.left, (int)ps.rcPaint.top, (int)ps.rcPaint.right, (int)ps.rcPaint.bottom);

        EndPaint(hWnd, &ps);
        break;
    }


    break;


    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;

}


//' ========================================================================================
//' HistoryPanel_Show
//' ========================================================================================
CWindow* HistoryPanel_Show(HWND hWndParent)
{
    // Create the window and child controls
    CWindow* pWindow = new CWindow;
   
    HWND HWND_FRMHISTORYPANEL =
        pWindow->Create(hWndParent, L"", & HistoryPanel_WndProc, 0, 0, HISTORYPANEL_WIDTH, 0,
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
            WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);
    
    // This is a child window of the main application parent so treat it like child
    // control and assign it a ControlID.
    SetWindowLongPtr(HWND_FRMHISTORYPANEL, GWLP_ID, IDC_FRMHISTORYPANEL);

    // Can only set the brush after the window is created
    pWindow->SetBrush(GetStockBrush(NULL_BRUSH));

    return pWindow;
}

