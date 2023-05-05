//
// Defines the entry point for the IB-TRACKER application.
//

#include "pch.h"

#include "MainWindow.h"
#include "tws-client.h"
#include "..\MenuPanel\MenuPanel.h"
#include "..\HistoryPanel\HistoryPanel.h"
#include "..\TradesPanel\TradesPanel.h"
#include "..\DailyPanel\DailyPanel.h"
#include "..\TickerPanel\TickerPanel.h"
#include "..\SuperLabel\SuperLabel.h"
#include "..\Themes\Themes.h"
#include "..\Utilities\UserMessages.h"
#include "..\Config\Config.h"


HWND HWND_MAINWINDOW = NULL;
HWND HWND_LEFTPANEL = NULL;
HWND HWND_MIDDLEPANEL = NULL;
HWND HWND_RIGHTPANEL = NULL;

CMainWindowShadow Shadow;
CMenuPanel        MenuPanel;
CHistoryPanel     HistoryPanel;
CTradesPanel      TradesPanel;
CTickerPanel      TickerPanel;
CDailyPanel       DailyPanel;


extern void TradesPanel_ShowActiveTrades();
extern ActiveThemeColor ActiveTheme;

RECT rcSplitter{};
bool isDragging = false;    // If dragging our splitter
POINT prev_pt{};            // for tracking current splitter drag


// ========================================================================================
// Set the HWND for the panel that will display on the right side of the MainWindow.
// Also place it into position and hide previous HWND of right panel.
// ========================================================================================
void MainWindow_SetRightPanel(HWND hPanel)
{
    // Get the current position size of the current right panel.
    RECT rc; GetWindowRect(HWND_RIGHTPANEL, &rc);
    MapWindowPoints(HWND_DESKTOP, HWND_MAINWINDOW, (LPPOINT)&rc, 2);

    ShowWindow(HWND_RIGHTPANEL, SW_HIDE);

    HWND_RIGHTPANEL = hPanel;
    SetWindowPos(HWND_RIGHTPANEL, 0, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
        SWP_NOZORDER | SWP_SHOWWINDOW);
}


// ========================================================================================
// Blur the MainWindow panels. This function is called prior and after
// a popup dialog is shown in order to reduce the underlying "visual noise" for that 
// popup window. For example, the Trade Management popup is easier to use and enter data
// into if the user is not distracted by the MainWindow menu panel, trades and history
// data displaying behind the popup.
// ========================================================================================
void MainWindow_BlurPanels(bool active)
{
    if (active) {
        RECT rc;       GetWindowRect(HWND_MAINWINDOW, &rc);
        RECT rcClient; GetClientRect(HWND_MAINWINDOW, &rcClient);
        
        int border_thickness = ((rc.right - rc.left) - rcClient.right) / 2;
        InflateRect(&rc, -border_thickness, -border_thickness);

        if (Shadow.WindowHandle() == NULL) {
            Shadow.Create(HWND_MAINWINDOW, L"", 0, 0, 0, 0,
                WS_POPUP | WS_DISABLED | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_LAYERED | WS_EX_NOACTIVATE);
        
            HBRUSH hbrBackground = GetSysColorBrush(COLOR_WINDOWTEXT);
            SetClassLongPtr(Shadow.WindowHandle(), GCLP_HBRBACKGROUND, (LONG_PTR)hbrBackground);

            // Make this window 50% alpha
            SetLayeredWindowAttributes(Shadow.WindowHandle(), 0, (255 * 50) / 100, LWA_ALPHA);
        }

        SetWindowPos(Shadow.WindowHandle(), 0,
            rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, 
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
        
    }
    else {
        ShowWindow(Shadow.WindowHandle(), SW_HIDE);
    }

}


// ========================================================================================
// Automatically show any existing trades on program startup & connect to TWS.
// ========================================================================================
void MainWindow_StartupShowTrades()
{
    if (trades.size() != 0) {
        TradesPanel_ShowActiveTrades();

        if (GetStartupConnect()) {
            SendMessage(MenuPanel.WindowHandle(), MSG_TWS_CONNECT_START, 0, 0);
            bool res = tws_connect();
            if (tws_isConnected()) {
                // Need to re-populate the Trades if successfully connected in order
                // to send the request market data for each ticker.
                TradesPanel_ShowActiveTrades();
            }
        }

    }
}


// ========================================================================================
// Process WM_DESTROY message for window/dialog: MainWindow
// ========================================================================================
void MainWindow_OnDestroy(HWND hwnd)
{
    // Disconnect from IBKR TWS and shut down monitoring thread.
    tws_disconnect();

    // Destroy the popup shadow window should it exist.
    if (Shadow.WindowHandle()) DestroyWindow(Shadow.WindowHandle());

    // Quit the application
    PostQuitMessage(0);
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: MainWindow
// ========================================================================================
BOOL MainWindow_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    // We do not need to paint the background because the full client area will always
    // be covered by child windows (NavPanel, Trades Panel, History Panel)
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: MainWindow
// ========================================================================================
void MainWindow_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    Graphics graphics(hdc);

    DWORD nBackColor = GetThemeColor(ThemeElement::MenuPanelBack);

    // Create the background brush
    SolidBrush backBrush(nBackColor);

    int nWidth = (ps.rcPaint.right - ps.rcPaint.left);
    int nHeight = (ps.rcPaint.bottom - ps.rcPaint.top);
    graphics.FillRectangle(&backBrush, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight);

    EndPaint(hwnd, &ps);
}


// ========================================================================================
// Process WM_SIZE message for window/dialog: MainWindow
// ========================================================================================
void MainWindow_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    // Position all of the child windows
    if (state == SIZE_MINIMIZED) return;
    

    int MARGIN = AfxScaleY(ACTIVE_TRADES_LISTBOX_ROWHEIGHT);
    int INNER_MARGIN = AfxScaleY(6);
    int SPLITTER_WIDTH = AfxScaleX(6);

    HDWP hdwp = BeginDeferWindowPos(5);

    // Position the left hand side Navigation Panel
    int nLeftPanelWidth = AfxGetWindowWidth(HWND_LEFTPANEL);
    hdwp = DeferWindowPos(hdwp, HWND_LEFTPANEL, 0,
                0, 0, nLeftPanelWidth, cy, SWP_NOZORDER | SWP_SHOWWINDOW);


    // Position the right hand side History Panel
    int nRightPanelWidth = AfxGetWindowWidth(HWND_RIGHTPANEL);
    hdwp = DeferWindowPos(hdwp, HWND_RIGHTPANEL, 0,
                cx - nRightPanelWidth - INNER_MARGIN, 0, nRightPanelWidth, cy - MARGIN,
                SWP_NOZORDER | SWP_SHOWWINDOW);

        
    // Position the middle Trades Panel
    int nMiddlePanelWidth = (cx - nRightPanelWidth - nLeftPanelWidth - SPLITTER_WIDTH - INNER_MARGIN);
    hdwp = DeferWindowPos(hdwp, HWND_MIDDLEPANEL, 0,
                nLeftPanelWidth, 0, nMiddlePanelWidth, cy - MARGIN,
                SWP_NOZORDER | SWP_SHOWWINDOW);


    // Position the Autoconnect indicator label
    HWND hCtl = GetDlgItem(hwnd, IDC_MAINWINDOW_AUTOCONNECT);
    int nHeight = AfxScaleY(HISTORYPANEL_MARGIN);
    int nWidth = AfxScaleX(100);
    DeferWindowPos(hdwp, hCtl, 0, cx - nWidth, cy - nHeight, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);


    // Position the Dark mode indicator label
    hCtl = GetDlgItem(hwnd, IDC_MAINWINDOW_DARKMODE);
    DeferWindowPos(hdwp, hCtl, 0, cx - (nWidth*2), cy - nHeight, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);


    EndDeferWindowPos(hdwp);

    // Calculate the area for the "splitter control"
    rcSplitter.left = nLeftPanelWidth + nMiddlePanelWidth;
    rcSplitter.top = 0;
    rcSplitter.right = rcSplitter.left + SPLITTER_WIDTH;
    rcSplitter.bottom = cy;

}


// ========================================================================================
// Process WM_CREATE message for window/dialog: MainWindow
// ========================================================================================
BOOL MainWindow_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_MAINWINDOW = hwnd;

    HWND_LEFTPANEL = MenuPanel.Create( hwnd, L"", 0, 0, MENUPANEL_WIDTH, 0,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    HWND_RIGHTPANEL = HistoryPanel.Create(hwnd, L"", 0, 0, HISTORYPANEL_WIDTH, 0,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    HWND_MIDDLEPANEL = TradesPanel.Create(hwnd, L"", 0, 0, 0, 0,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);
    
    TickerPanel.Create(hwnd, L"", 0, 0, 0, 0,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    DailyPanel.Create(hwnd, L"", 0, 0, 0, 0,
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    // Create a label that will display at the very bottom of the Main window
    // that allows toggling Dark Theme on/off. This label is always positioned
    // at the bottom of the window via On_Size().
    SuperLabel* pData = nullptr;

    HWND hCtl = CreateSuperLabel(
        hwnd,
        IDC_MAINWINDOW_DARKMODE,
        SuperLabelType::TextOnly,
        0, 0, 0, 0);
    pData = SuperLabel_GetOptions(hCtl);
    if (pData) {
        pData->HotTestEnable = true;
        pData->AllowSelect = false;
        pData->BackColor = ThemeElement::MenuPanelBack;
        pData->BackColorHot = ThemeElement::MenuPanelBackHot;
        pData->TextColor = ThemeElement::MenuPanelTextDim;
        pData->TextColorHot = ThemeElement::MenuPanelTextDim;
        pData->FontSize = 8;
        pData->FontSizeHot = 8;
        pData->wszText = L"Dark Mode: ON";
        pData->wszTextHot = pData->wszText;
        SuperLabel_SetOptions(hCtl, pData);
    }


    // Create a label that will display at the very bottom of the Main window
    // that allows toggling Autoconnect on/off. This label is always positioned
    // at the bottom of the window via On_Size().
    hCtl = CreateSuperLabel(
        hwnd,
        IDC_MAINWINDOW_AUTOCONNECT,
        SuperLabelType::TextOnly,
        0, 0, 0, 0);
    pData = SuperLabel_GetOptions(hCtl);
    if (pData) {
        pData->HotTestEnable = true;
        pData->AllowSelect = false;
        pData->BackColor = ThemeElement::MenuPanelBack;
        pData->BackColorHot = ThemeElement::MenuPanelBackHot;
        pData->TextColor = ThemeElement::MenuPanelTextDim;
        pData->TextColorHot = ThemeElement::MenuPanelTextDim;
        pData->FontSize = 8;
        pData->FontSizeHot = 8;
        pData->wszText = GetStartupConnect() ? L"Autoconnect: ON" : L"Autoconnect: OFF";
        pData->wszTextHot = pData->wszText;
        SuperLabel_SetOptions(hCtl, pData);
    }

    return TRUE;
}



// ========================================================================================
// Determine if mouse cursor is over our splitter control area.
// ========================================================================================
bool IsMouseOverSplitter(HWND hwnd)
{
    POINT pt; GetCursorPos(&pt);
    MapWindowPoints(HWND_DESKTOP, hwnd, (POINT*)&pt, 1);

    if (PtInRect(&rcSplitter, pt)) {
        SetCursor(LoadCursor(NULL, (LPCWSTR)IDC_SIZEWE));
        return TRUE;
    }
    return FALSE;
}


// ========================================================================================
// Process WM_LBUTTONDOWN message for window/dialog: MainWindow
// ========================================================================================
void MainWindow_OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, UINT keyFlags)
{
    if (IsMouseOverSplitter(hwnd)) {
        isDragging = true;
        prev_pt.x = x;
        SetCapture(hwnd);
    }
}


void UpdateSplitterChildren(HWND hwnd, int xDelta)
{
    int nRightPanelWidth = AfxGetWindowWidth(HWND_RIGHTPANEL) + xDelta;
    int nRightPanelHeight = AfxGetWindowHeight(HWND_RIGHTPANEL);
    SetWindowPos(HWND_RIGHTPANEL, 0,
        0, 0, nRightPanelWidth, nRightPanelHeight, SWP_NOZORDER | SWP_NOMOVE);

    RECT rc; GetClientRect(hwnd, &rc);
    MainWindow_OnSize(hwnd, SIZE_RESTORED, rc.right, rc.bottom);
    AfxRedrawWindow(HWND_MIDDLEPANEL);
    AfxRedrawWindow(HWND_RIGHTPANEL);
    AfxRedrawWindow(hwnd);
}


// ========================================================================================
// Process WM_MOUSEMOVE message for window/dialog: MainWindow
// ========================================================================================
void MainWindow_OnMouseMove(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (isDragging) {
        // Calculate the change between mouse movements and resize
        // the child windows accordingly.
        int xDelta = prev_pt.x - x;

        // Only reposition after a cummulative greater than 3 pixel move
        // in order to reduce the amount of screen updating.
        if (abs(xDelta) > 3) {
            UpdateSplitterChildren(hwnd, xDelta);
            prev_pt.x = x;
        }

    }
}


// ========================================================================================
// Process WM_LBUTTONUP message for window/dialog: MainWindow
// ========================================================================================
void MainWindow_OnLButtonUp(HWND hwnd, int x, int y, UINT keyFlags)
{
    if (isDragging) {
        isDragging = false;
        int xDelta = prev_pt.x - x;
        UpdateSplitterChildren(hwnd, xDelta);
        ReleaseCapture();
    }
}


// ========================================================================================
// Process WM_SETCURSOR message for window/dialog: MainWindow
// ========================================================================================
BOOL MainWindow_OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, UINT msg)
{
    LPWSTR curId = IDC_ARROW;

    // Determine if the mouse is over our "splitter control"
    if (IsMouseOverSplitter(hwnd)) {
        SetCursor(LoadCursor(NULL, (LPCWSTR)IDC_SIZEWE));
        return TRUE;
    }

    switch (codeHitTest)
    {
    case HTTOPRIGHT:
    case HTBOTTOMLEFT:
        curId = IDC_SIZENESW;
        break;

    case HTBOTTOMRIGHT:
    case HTTOPLEFT:
        curId = IDC_SIZENWSE;
        break;

    case HTLEFT:
    case HTRIGHT:
        curId = IDC_SIZEWE;
        break;

    case HTTOP:
    case HTBOTTOM:
        curId = IDC_SIZENS;
        break;

    default:
        curId = IDC_ARROW;
    }

    SetCursor(LoadCursor(NULL, (LPCWSTR)curId));
    return TRUE;
}


// ========================================================================================
// Windows callback function (MainWindow).
// ========================================================================================
LRESULT CMainWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, MainWindow_OnCreate);
        HANDLE_MSG(m_hwnd, WM_DESTROY, MainWindow_OnDestroy);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, MainWindow_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, MainWindow_OnPaint);
        HANDLE_MSG(m_hwnd, WM_SIZE, MainWindow_OnSize);
        HANDLE_MSG(m_hwnd, WM_MOUSEMOVE, MainWindow_OnMouseMove);
        HANDLE_MSG(m_hwnd, WM_LBUTTONDOWN, MainWindow_OnLButtonDown);
        HANDLE_MSG(m_hwnd, WM_LBUTTONUP, MainWindow_OnLButtonUp);
        HANDLE_MSG(m_hwnd, WM_SETCURSOR, MainWindow_OnSetCursor);

    case MSG_STARTUP_SHOWTRADES:
    {
        MainWindow_StartupShowTrades();
        return 0;
    }

    case MSG_SUPERLABEL_CLICK:
    {
        HWND hCtl = (HWND)lParam;
        int CtrlId = (int)wParam;

        if (hCtl == NULL) return 0;
        SuperLabel* pData = (SuperLabel*)GetWindowLongPtr(hCtl, 0);

        if (pData) {
            if (CtrlId == IDC_MAINWINDOW_DARKMODE) {
                if (ActiveTheme == ActiveThemeColor::Dark) {
                    SetThemeName(L"Light");
                    SuperLabel_SetText(hCtl, L"Dark Mode: OFF");
                }
                else {
                    SetThemeName(L"Dark");
                    SuperLabel_SetText(hCtl, L"Dark Mode: ON");
                }
                ApplyActiveTheme();
                SaveConfig();
            }

            if (CtrlId == IDC_MAINWINDOW_AUTOCONNECT) {
                SetStartupConnect(!GetStartupConnect());
                if (GetStartupConnect()) {
                    SuperLabel_SetText(hCtl, L"Autoconnect: ON");
                }
                else {
                    SuperLabel_SetText(hCtl, L"Autoconnect: OFF");
                }
                SaveConfig();
            }
        }
        return 0;

    }

    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}



// ========================================================================================
// Windows callback function (MainWindowShadow).
// ========================================================================================
LRESULT CMainWindowShadow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(m_hwnd, msg, wParam, lParam);
}

