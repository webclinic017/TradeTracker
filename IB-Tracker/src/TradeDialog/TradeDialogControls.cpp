#include "pch.h"

#include "TradeDialog.h"
#include "..\VScrollBar\VScrollBar.h"
#include "..\Utilities\ListBoxData.h"


struct LineCtrl {
    HWND cols[6]{ NULL };
};

std::vector<LineCtrl> lCtrls;

extern CTradeDialog TradeDialog;



// ========================================================================================
// Listbox subclass Window procedure
// ========================================================================================
LRESULT CALLBACK TradeDialog_ListBox_SubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    // Create static accumulation variable to collect the data from
    // a series of middle mouse wheel scrolls.
    static int accumDelta = 0;

    switch (uMsg)
    {

    case WM_MOUSEWHEEL:
    {
        // Accumulate delta until scroll one line (up +120, down -120). 
        // 120 is the Microsoft default delta
        int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        int nTopIndex = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
        accumDelta += zDelta;
        if (accumDelta >= 120) {     // scroll up 3 lines
            nTopIndex -= 3;
            nTopIndex = max(0, nTopIndex);
            SendMessage(hWnd, LB_SETTOPINDEX, nTopIndex, 0);
            accumDelta = 0;
        }
        else {
            if (accumDelta <= -120) {     // scroll down 3 lines
                nTopIndex += +3;
                SendMessage(hWnd, LB_SETTOPINDEX, nTopIndex, 0);
                accumDelta = 0;
            }
        }

        switch (uIdSubclass)
        {
        case IDC_TRADEDIALOG_TEMPLATES:
            VScrollBar_Recalculate(GetDlgItem(TradeDialog.WindowHandle(), IDC_TRADEDIALOG_VSCROLLBAR1));
            break;
        }
        break;
    }


    case WM_ERASEBKGND:
    {
        // If the number of lines in the listbox maybe less than the number per page then 
        // calculate from last item to bottom of listbox, otherwise calculate based on
        // the mod of the lineheight to listbox height so we can color the partial line
        // that won't be displayed at the bottom of the list.
        RECT rc; GetClientRect(hWnd, &rc);

        RECT rcItem{};
        SendMessage(hWnd, LB_GETITEMRECT, 0, (LPARAM)&rcItem);
        int itemHeight = (rcItem.bottom - rcItem.top);
        int NumItems = ListBox_GetCount(hWnd);
        int nTopIndex = SendMessage(hWnd, LB_GETTOPINDEX, 0, 0);
        int visible_rows = 0;
        int ItemsPerPage = 0;
        int bottom_index = 0;
        int nWidth = (rc.right - rc.left);
        int nHeight = (rc.bottom - rc.top);

        if (NumItems > 0) {
            ItemsPerPage = (nHeight) / itemHeight;
            bottom_index = (nTopIndex + ItemsPerPage);
            if (bottom_index >= NumItems)
                bottom_index = NumItems - 1;
            visible_rows = (bottom_index - nTopIndex) + 1;
            rc.top = visible_rows * itemHeight;
        }

        if (rc.top < rc.bottom) {
            nHeight = (rc.bottom - rc.top);
            HDC hDC = (HDC)wParam;
            Graphics graphics(hDC);
            SolidBrush backBrush(GetThemeColor(ThemeElement::TradesPanelBack));
            graphics.FillRectangle(&backBrush, rc.left, rc.top, nWidth, nHeight);
        }

        ValidateRect(hWnd, &rc);
        return TRUE;
        break;

    }


    case WM_DESTROY:
        // Destroy all manually allocated ListBox display data that is held
        // in the LineData structures..
        ListBoxData_DestroyItemData(hWnd);

        // REQUIRED: Remove control subclassing
        RemoveWindowSubclass(hWnd, TradeDialog_ListBox_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


// ========================================================================================
// Generic Control subclass Window procedure to deal with centering vertical 
// text in textboxes.
// ========================================================================================
LRESULT CALLBACK TradeDialog_TextBox_SubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {

    case WM_NCCALCSIZE:
    {
        // If this is a TextBox then we need to vertically center the text. The text is
        // already centered horizontally so we can simply push it down from the top slightly.
        if (wParam) {
            NCCALCSIZE_PARAMS* ncParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            ncParams->rgrc[0].top += AfxScaleY(3);
            return WVR_HREDRAW;
        }
        return 0;
    }


    case WM_DESTROY:
        // REQUIRED: Remove control subclassing
        RemoveWindowSubclass(hWnd, TradeDialog_TextBox_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


// ========================================================================================
// Generic Control subclass Window procedure to remove borders on DateTimePicker
// and ComboBox.
// ========================================================================================
LRESULT CALLBACK TradeDialog_Control_SubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {

    case WM_NCCREATE:
    {
    }
    break;


    case WM_NCPAINT:
    {
    }
    break;

    case WM_DESTROY:
        // REQUIRED: Remove control subclassing
        RemoveWindowSubclass(hWnd, TradeDialog_Control_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


// ========================================================================================
// Helper function for WM_SIZE message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialogControls_SizeControls(HWND hwnd, int cx, int cy)
{
    HWND hTemplates = GetDlgItem(hwnd, IDC_TRADEDIALOG_TEMPLATES);
    HWND hVScrollBar1 = GetDlgItem(hwnd, IDC_TRADEDIALOG_VSCROLLBAR1);
    HWND hTransDate = GetDlgItem(hwnd, IDC_TRADEDIALOG_TRANSDATE);
    HWND txtTicker = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTICKER);
    HWND txtCompany = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTCOMPANY);
    HWND txtDescription = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTDESCRIPTION);
    HWND hHeader = GetDlgItem(hwnd, IDC_TRADEDIALOG_HEADER);
    HWND hVScrollBar2 = GetDlgItem(hwnd, IDC_TRADEDIALOG_VSCROLLBAR2);
    HWND lblQuantity = GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLQUANTITY);
    HWND lblPrice = GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLPRICE);
    HWND lblFees = GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLFEES);
    HWND lblTotal = GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLTOTAL);
    HWND txtQuantity = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTQUANTITY);
    HWND txtPrice = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTPRICE);
    HWND txtFees = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTFEES);
    HWND txtTotal = GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTOTAL);
    HWND comboDRCR = GetDlgItem(hwnd, IDC_TRADEDIALOG_COMBODRCR);


    int vmargin = AfxScaleY(6);
    int hmargin = AfxScaleX(10);

    // Do not call the calcVThumbRect() function during a scrollbar move. This WM_SIZE
    // gets triggered when the ListBox WM_DRAWITEM fires. If we do another calcVThumbRect()
    // calcualtion then the scrollbar will appear "jumpy" under the user's mouse cursor.
    bool bShowScrollBar1 = false;
    VScrollBar* pData = VScrollBar_GetPointer(hVScrollBar1);
    if (pData != nullptr) bShowScrollBar1 = (pData->bDragActive) ? true : pData->calcVThumbRect();
    int VScrollBarWidth1 = bShowScrollBar1 ? AfxScaleX(VSCROLLBAR_WIDTH) : 0;

    bool bShowScrollBar2 = false;
    pData = VScrollBar_GetPointer(hVScrollBar2);
    if (pData != nullptr) bShowScrollBar1 = (pData->bDragActive) ? true : pData->calcVThumbRect();
    int VScrollBarWidth2 = bShowScrollBar2 ? AfxScaleX(VSCROLLBAR_WIDTH) : 0;


    int nTop = (vmargin * 2);
    int nLeft = 0;
    int nWidth = 0;
    int nHeight = cy - nTop;
    int nCtlHeight = AfxScaleY(24);

    nWidth = AfxScaleX(180);
    SetWindowPos(hTemplates, 0, nLeft, nTop, nWidth, nHeight,
        SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nLeft + nWidth;   // right edge of ListBox
    nWidth = VScrollBarWidth1;
    SetWindowPos(hVScrollBar1, 0, nLeft, nTop, nWidth, nHeight,
        SWP_NOZORDER | (bShowScrollBar1 ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

    int nStartLeft = nLeft + nWidth + AfxScaleX(20) + hmargin;
    
    nLeft = nStartLeft;
    nTop = (vmargin * 2);
    nWidth = AfxScaleX(100);
    nHeight = nCtlHeight;
    SetWindowPos(hTransDate, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nLeft + nWidth + (hmargin * 2);
    nWidth = AfxScaleX(75);
    nHeight = nCtlHeight;
    SetWindowPos(txtTicker, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nLeft + nWidth + hmargin;
    nWidth = AfxScaleX(235);
    nHeight = nCtlHeight;
    SetWindowPos(txtCompany, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nStartLeft + AfxScaleX(100) + (hmargin * 2);
    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(320);
    nHeight = nCtlHeight;
    SetWindowPos(txtDescription, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);



    // Position all of the controls for the Trade Management table
    nLeft = nStartLeft;
    nTop = nTop + nHeight + (vmargin * 2);

    nWidth = AfxScaleX(480);
    nHeight = nCtlHeight;
    SetWindowPos(hHeader, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);


    nTop = nTop + nHeight;
    nWidth = AfxScaleX(90);
    nHeight = nCtlHeight;

    for (int i = 0; i < 10; i++) {
        for (int ii = 0; ii < 6; ii++) {
            SetWindowPos(lCtrls.at(i).cols[ii], 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
            nLeft += nWidth;
        }
        nLeft = nStartLeft;
        nTop = nTop + nHeight;
    }




    int nStartTop = nTop + nHeight + (vmargin * 3);

    nLeft = nStartLeft + AfxScaleX(100);
    nTop = nStartTop + AfxScaleY(4);
    nWidth = AfxScaleX(75);
    nHeight = nCtlHeight;
    SetWindowPos(lblQuantity, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nStartLeft + AfxScaleX(100);
    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(75);
    nHeight = nCtlHeight;
    SetWindowPos(lblPrice, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nStartLeft + AfxScaleX(100);
    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(75);
    nHeight = nCtlHeight;
    SetWindowPos(lblFees, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nStartLeft + AfxScaleX(100);
    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(75);
    nHeight = nCtlHeight;
    SetWindowPos(lblTotal, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);


    nLeft = nStartLeft + AfxScaleX(100) + nWidth;
    nTop = nStartTop;
    nWidth = AfxScaleX(80);
    nHeight = nCtlHeight;
    SetWindowPos(txtQuantity, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(80);
    nHeight = nCtlHeight;
    SetWindowPos(txtPrice, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(80);
    nHeight = nCtlHeight;
    SetWindowPos(txtFees, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nTop = nTop + nHeight + vmargin;
    nWidth = AfxScaleX(80);
    nHeight = nCtlHeight;
    SetWindowPos(txtTotal, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nLeft + nWidth + hmargin;
    nWidth = AfxScaleX(75);
    nHeight = nCtlHeight;
    SetWindowPos(comboDRCR, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

 
    VScrollBar_Recalculate(hVScrollBar1);
    VScrollBar_Recalculate(hVScrollBar2);
}


// ========================================================================================
// Helper function for WM_CREATE message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialogControls_CreateControls(HWND hwnd)
{
    HWND hCtl = NULL;

    // Create an Ownerdraw listbox that we will use to custom paint our various Trade Templates.
    hCtl =
        TradeDialog.AddControl(Controls::ListBox, hwnd, IDC_TRADEDIALOG_TEMPLATES, L"",
            0, 0, 0, 0,
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP |
            LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_NOTIFY,
            WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR, NULL,
            (SUBCLASSPROC)TradeDialog_ListBox_SubclassProc,
            IDC_TRADEDIALOG_TEMPLATES, NULL);

    // Create our custom vertical scrollbar and attach the ListBox to it.
    CreateVScrollBar(hwnd, IDC_TRADEDIALOG_TEMPLATES, hCtl);

    // Add all of our Trade Templates to the ListBox
    ListBoxData_OutputTradesTemplates(hCtl);

    // Resize columns
    ListBoxData_ResizeColumnWidths(hCtl, TableType::TradeTemplates, -1);

    hCtl = TradeDialog.AddControl(Controls::DateTimePicker, hwnd, IDC_TRADEDIALOG_TRANSDATE);
    DateTime_SetFormat(hCtl, L"yyyy-MM-dd");

    hCtl = TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTTICKER);
    Edit_SetCueBannerText(hCtl, L"Ticker");

    hCtl = TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTCOMPANY);
    Edit_SetCueBannerText(hCtl, L"Company Name");

    hCtl = TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTDESCRIPTION);
    Edit_SetCueBannerText(hCtl, L"Description");

    TradeDialog.AddControl(Controls::Header, hwnd, IDC_TRADEDIALOG_HEADER);

    // Create the Trade Management table controls 
    lCtrls.reserve(20);

    for (int i = 0; i < 20; i++) {
        LineCtrl lc;

        lc.cols[0] = TradeDialog.AddControl(Controls::TextBox, hwnd, -1, L"-1", 0, 0, 0, 0,
            WS_VISIBLE | WS_TABSTOP | ES_CENTER | ES_AUTOHSCROLL, 0, NULL,
            (SUBCLASSPROC)TradeDialog_TextBox_SubclassProc, NULL, NULL);

        hCtl = lc.cols[1] = TradeDialog.AddControl(Controls::DateTimePicker, hwnd, -1, L"", 0, 0, 0, 0,
            WS_VISIBLE | WS_TABSTOP | DTS_SHORTDATEFORMAT, 0, NULL,
            (SUBCLASSPROC)TradeDialog_Control_SubclassProc, NULL, NULL);

        lc.cols[2] = TradeDialog.AddControl(Controls::Label, hwnd, -1, L"0d", 0, 0, 0, 0,
            WS_VISIBLE | SS_CENTER | SS_CENTERIMAGE | WS_GROUP | SS_NOTIFY, 0);

        lc.cols[3] = TradeDialog.AddControl(Controls::TextBox, hwnd, -1, L"45.5", 0, 0, 0, 0,
            WS_VISIBLE | WS_TABSTOP | ES_CENTER | ES_AUTOHSCROLL, 0, NULL,
            (SUBCLASSPROC)TradeDialog_TextBox_SubclassProc, NULL, NULL);

        lc.cols[4] = TradeDialog.AddControl(Controls::ComboBox, hwnd, -1L, L"P", 0, 0, 0, 0,
            WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_NOINTEGRALHEIGHT, 0);

        hCtl = lc.cols[5] = TradeDialog.AddControl(Controls::ComboBox, hwnd, -1L, L"P", 0, 0, 0, 0,
            WS_VISIBLE | WS_VSCROLL | WS_TABSTOP | CBS_DROPDOWNLIST | CBS_HASSTRINGS | CBS_NOINTEGRALHEIGHT, 0);

        lCtrls.push_back(lc);
    }


    TradeDialog.AddControl(Controls::Label, hwnd, IDC_TRADEDIALOG_LBLQUANTITY, L"Quantity");
    TradeDialog.AddControl(Controls::Label, hwnd, IDC_TRADEDIALOG_LBLPRICE, L"Price");
    TradeDialog.AddControl(Controls::Label, hwnd, IDC_TRADEDIALOG_LBLFEES, L"Fees");
    TradeDialog.AddControl(Controls::Label, hwnd, IDC_TRADEDIALOG_LBLTOTAL, L"Total");

    TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTQUANTITY);
    TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTPRICE);
    TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTFEES);
    TradeDialog.AddControl(Controls::TextBox, hwnd, IDC_TRADEDIALOG_TXTTOTAL);

    TradeDialog.AddControl(Controls::ComboBox, hwnd, IDC_TRADEDIALOG_COMBODRCR);

    TradeDialog.AddControl(Controls::Button, hwnd, IDC_TRADEDIALOG_OK, L"OK", 640, 464, 74, 28);
    TradeDialog.AddControl(Controls::Button, hwnd, IDC_TRADEDIALOG_CANCEL, L"Cancel", 640, 500, 74, 28);

}
