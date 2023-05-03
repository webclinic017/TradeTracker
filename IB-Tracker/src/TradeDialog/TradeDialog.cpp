
#include "pch.h"

#include "TradeDialog.h"
#include "TradeDialogControls.h"
#include "..\MainWindow\MainWindow.h"
#include "..\Utilities\ListBoxData.h"
#include "..\TradesPanel\TradesPanel.h"


HWND HWND_TRADEDIALOG = NULL;

extern HWND HWND_MAINWINDOW;

CTradeDialog TradeDialog;

COLORREF CtrlTextColor = GetThemeCOLORREF(ThemeElement::TradesPanelText);
COLORREF CtrlTextBack = GetThemeCOLORREF(ThemeElement::TradesPanelBack);
HBRUSH CtrlBackBrush = NULL;

int tradeAction = ACTION_NOACTION;


// ========================================================================================
// Process WM_MEASUREITEM message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialog_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
    switch (lpMeasureItem->CtlID)
    {
    case IDC_TRADEDIALOG_TEMPLATES:
        lpMeasureItem->itemHeight = AfxScaleY(TRADEDIALOG_TEMPLATES_ROWHEIGHT);
        break;
    }
}


// ========================================================================================
// Process WM_CLOSE message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialog_OnClose(HWND hwnd)
{
    MainWindow_BlurPanels(false);
    EnableWindow(HWND_MAINWINDOW, TRUE);
    DestroyWindow(hwnd);
}


// ========================================================================================
// Process WM_DESTROY message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialog_OnDestroy(HWND hwnd)
{
    // Clear the Trade Templates Linedata held in the ListBox
    ListBoxData_DestroyItemData(GetDlgItem(hwnd, IDC_TRADEDIALOG_TEMPLATES));

    DeleteObject(CtrlBackBrush);

    PostQuitMessage(0);
}


// ========================================================================================
// Process WM_SIZE message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialog_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    TradeDialogControls_SizeControls(hwnd, cx, cy);
}


// ========================================================================================
// Process WM_CREATE message for window/dialog: TradeDialog
// ========================================================================================
BOOL TradeDialog_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_TRADEDIALOG = hwnd;

    CtrlBackBrush = CreateSolidBrush(CtrlTextBack);
    TradeDialogControls_CreateControls(hwnd);

    return TRUE;
}


// ========================================================================================
// Process WM_CTLCOLORBTN message for window/dialog: TradeDialog
// ========================================================================================
HBRUSH TradeDialog_OnCtlColorBtn(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    SetTextColor(hdc, CtrlTextColor);
    SetBkColor(hdc, CtrlTextBack);
    return GetSysColorBrush(COLOR_WINDOW);
}


// ========================================================================================
// Process WM_CTLCOLOREDIT message for window/dialog: TradeDialog
// ========================================================================================
HBRUSH TradeDialog_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    SetTextColor(hdc, CtrlTextColor);

    // If this is the Total TextBox then display text as Red/Green 
    // depending on the status of the Debit/Credit combobox.
    if (GetDlgCtrlID(hwndChild) == IDC_TRADEDIALOG_TXTTOTAL) {
        std::wstring wszText = AfxGetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_COMBODRCR));
        if (wszText == L"DEBIT") SetTextColor(hdc, GetThemeCOLORREF(ThemeElement::valueNegative));
        if (wszText == L"CREDIT") SetTextColor(hdc, GetThemeCOLORREF(ThemeElement::valuePositive));
    }

    SetBkColor(hdc, CtrlTextBack);
    SetBkMode(hdc, OPAQUE);
    return CtrlBackBrush;
}


// ========================================================================================
// Process WM_CTLCOLORSTATIC message for window/dialog: TradeDialog
// ========================================================================================
HBRUSH TradeDialog_OnCtlColorStatic(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    SetTextColor(hdc, CtrlTextColor);
    SetBkColor(hdc, CtrlTextBack);
    return CtrlBackBrush;
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: TradeDialog
// ========================================================================================
BOOL TradesDialog_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: TradeDialog
// ========================================================================================
void TradesDialog_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    Graphics graphics(hdc);

    DWORD nBackColor = GetThemeColor(ThemeElement::TradesPanelBack);

    // Create the background brush
    SolidBrush backBrush(nBackColor);

    // Paint the background using brush.
    int nWidth = (ps.rcPaint.right - ps.rcPaint.left);
    int nHeight = (ps.rcPaint.bottom - ps.rcPaint.top);
    graphics.FillRectangle(&backBrush, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight);

    EndPaint(hwnd, &ps);
}


// ========================================================================================
// Process WM_NOTIFY message for window/dialog: TradeDialog
// ========================================================================================
LRESULT TradeDialog_OnNotify(HWND hwnd, int id, LPNMHDR lpNMHDR)
{
    switch (lpNMHDR->code)
    {
    case (DTN_DATETIMECHANGE):
        if (lpNMHDR->idFrom == IDC_TRADEDIALOG_TRANSDATE ||
            (lpNMHDR->idFrom >= IDC_TRADEDIALOG_TABLEEXPIRY && lpNMHDR->idFrom <= IDC_TRADEDIALOG_TABLEEXPIRY + 10)) {
            CalculateTradeDTE(hwnd);
            return 0;
        }
        break;
    }

    return 0;
}



// ========================================================================================
// Process WM_COMMAND message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialog_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch (id)
    {
    case (IDC_TRADEDIALOG_OK):
        if (codeNotify == BN_CLICKED) {
            // SaveDatabase();
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
        break;

    case (IDC_TRADEDIALOG_CANCEL):
        if (codeNotify == BN_CLICKED) {
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }
        break;

    case (IDC_TRADEDIALOG_COMBODRCR):
        if (codeNotify == CBN_SELCHANGE) {
            CalculateTradeTotal(hwnd);
        }
        break;

    }
}


// ========================================================================================
// Windows callback function.
// ========================================================================================
LRESULT CTradeDialog::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, TradeDialog_OnCreate);
        HANDLE_MSG(m_hwnd, WM_SIZE, TradeDialog_OnSize);
        HANDLE_MSG(m_hwnd, WM_COMMAND, TradeDialog_OnCommand);
        HANDLE_MSG(m_hwnd, WM_NOTIFY, TradeDialog_OnNotify);
        HANDLE_MSG(m_hwnd, WM_DESTROY, TradeDialog_OnDestroy);
        HANDLE_MSG(m_hwnd, WM_CLOSE, TradeDialog_OnClose);
        HANDLE_MSG(m_hwnd, WM_CTLCOLORBTN, TradeDialog_OnCtlColorBtn);
        HANDLE_MSG(m_hwnd, WM_CTLCOLOREDIT, TradeDialog_OnCtlColorEdit);
        HANDLE_MSG(m_hwnd, WM_CTLCOLORSTATIC, TradeDialog_OnCtlColorStatic);
        HANDLE_MSG(m_hwnd, WM_MEASUREITEM, TradeDialog_OnMeasureItem);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, TradesDialog_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, TradesDialog_OnPaint);
        HANDLE_MSG(m_hwnd, WM_DRAWITEM, ListBoxData_OnDrawItem);

    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}


// ========================================================================================
// Create and show the Trade modal dialog.
// ========================================================================================
void TradeDialog_Show(int inTradeAction)
{
    tradeAction = inTradeAction;

    int nWidth = 900;
    int nHeight = 640;
    if (tradeAction != ACTION_NEW_TRADE) {
        nWidth = nWidth - TRADEDIALOG_TRADETEMPLATES_WIDTH;
    }

    HWND hwnd = TradeDialog.Create(HWND_MAINWINDOW, L"Trade Management", 0, 0, nWidth, nHeight,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    HBRUSH hbrBackground = GetSysColorBrush(COLOR_WINDOWTEXT);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrBackground);

    HANDLE hIconSmall = LoadImage(TradeDialog.hInst(), MAKEINTRESOURCE(IDI_MAINICON), IMAGE_ICON, 16, 16, LR_SHARED);
    SendMessage(hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);

    AfxCenterWindow(hwnd, HWND_MAINWINDOW);

    EnableWindow(HWND_MAINWINDOW, FALSE);
    
    // Show any selected Trade Template that might be coming from a selected menu
    // item from the main application's navigation MenuPanel menu.
    //LoadTemplateInTradeTable(hwnd, pTradeTemplate);

    // Show the legsEdit legs (if any) based on the incoming action.
    LoadEditLegsInTradeTable(hwnd);

    
    // Blur the underlying MainWindow panels in order to reduce "visual noise" while
    // our Trade Management popup is active. The MainWindow panels are shown again
    // during our call to TradeDialog_OnClose() prior to enabling the MainWindow
    // and this popup closing.
    MainWindow_BlurPanels(true);


    ShowWindow(hwnd, SW_SHOWNORMAL);
    
    // Need to force a resize of the TradeDialog in order to properly show (or not show) 
    // and position the Templates scrollbar.
    RECT rc; GetClientRect(HWND_TRADEDIALOG, &rc);
    TradeDialog_OnSize(HWND_TRADEDIALOG, 0, rc.right, rc.bottom);

    
    // set focus to the Transaction date picker
    SetFocus(GetDlgItem(hwnd, IDC_TRADEDIALOG_TRANSDATE));

     
   
    // Call modal message pump and wait for it to end.
    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        // Determines whether a message is intended for the specified
        // dialog box and, if it is, processes the message.
        if (!IsDialogMessage(hwnd, &msg)) {
            // Translates virtual-key messages into character messages.
            TranslateMessage(&msg);
            // Dispatches a message to a window procedure.
            DispatchMessage(&msg);
        }
    }

}

