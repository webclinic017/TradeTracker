#include "pch.h"
#include "..\MainWindow\tws-client.h"
#include "..\Database\trade.h"
#include "..\SuperLabel\SuperLabel.h"
#include "..\Utilities\ListBoxData.h"
#include "..\Themes\Themes.h"
#include "..\MenuPanel\MenuPanel.h"
#include "..\VScrollBar\VScrollBar.h"
#include "TradesPanel.h"


HWND HWND_TRADESPANEL = NULL;

extern std::vector<Trade*> trades;

extern HWND HWND_HISTORYPANEL;
extern HWND HWND_MENUPANEL;
extern CTradesPanel TradesPanel;


extern void HistoryPanel_ShowTradesHistoryTable(Trade* trade);
void TradesPanel_OnSize(HWND hwnd, UINT state, int cx, int cy);



// ========================================================================================
// Central function that actually selects and displays the incoming ListBox index item.
// ========================================================================================
void TradesPanel_ShowListBoxItem(int index)
{
    HWND hListBox = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_LISTBOX);
    HWND hVScrollBar = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_VSCROLLBAR);

    ListBox_SetCurSel(hListBox, index);

    //  update the scrollbar position if necessary
    VScrollBar_Recalculate(hVScrollBar);

    // Get the current line to determine if a valid Trade pointer exists so that we
    // can show the trade history.
    if (index > -1) {
        ListBoxData* ld = (ListBoxData*)ListBox_GetItemData(hListBox, index);
        if (ld != nullptr)
            HistoryPanel_ShowTradesHistoryTable(ld->trade);
    }

    SetFocus(hListBox);
}


// ========================================================================================
// Populate the Trades ListBox with the current active/open trades
// ========================================================================================
void TradesPanel_ShowActiveTrades()
{
    HWND hListBox = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_LISTBOX);
    HWND hVScrollBar = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_VSCROLLBAR);
    HWND hLabel = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_LABEL);

    tws_PauseTWS();

    // Prevent ListBox redrawing until all calculations are completed
    SendMessage(hListBox, WM_SETREDRAW, FALSE, 0);


    // TODO: optimize this by sorting after initial database load and after
    // newly added/deleted data, rather than here every time we display the
    // active trades.
    // Sort the trades vector based on ticker symbol
    std::sort(trades.begin(), trades.end(),
        [](const Trade* trade1, const Trade* trade2) {
            return (trade1->tickerSymbol < trade2->tickerSymbol) ? true : false;
        });


    // Destroy any existing ListBox line data
    // This will also clear the LineData pointers and cancel any previous market data
    ListBoxData_DestroyItemData(hListBox);


    // Create the new ListBox line data and initiate the new market data.
    int tickerId = 100;
    for (const auto& trade : trades) {
        // We are displaying only all open trades
        if (trade->isOpen) {
            ListBoxData_OpenPosition(hListBox, trade, tickerId);
        }
        tickerId++;
    }


    // Calculate the actual column widths based on the size of the strings in
    // ListBoxData while respecting the minimum values as defined in nMinColWidth[].
    // This function is also called when receiving new price data from TWS because
    // that data may need the column width to be wider.
    ListBoxData_ResizeColumnWidths(hListBox, TableType::ActiveTrades, -1);

    
    // Select the correct menu panel item
    MenuPanel_SelectMenuItem(HWND_MENUPANEL, IDC_MENUPANEL_ACTIVETRADES);

    
    // Set the label text indicated the type of trades being listed
    SuperLabel_SetText(hLabel, L"Active Trades");


    // Need to force a resize of the TradesPanel in order to properly show (or not show) 
    // and position the Header control.
    RECT rc; GetClientRect(HWND_TRADESPANEL, &rc);
    TradesPanel_OnSize(HWND_TRADESPANEL, 0, rc.right, rc.bottom);


    // Redraw the ListBox to ensure that any recalculated columns are 
    // displayed correctly. Re-enable redraw.
    SendMessage(hListBox, WM_SETREDRAW, TRUE, 0);
    AfxRedrawWindow(hListBox);


    // If trades exist then select the first trade so that its history will show
    if (ListBox_GetCount(hListBox)) {
        TradesPanel_ShowListBoxItem(0);
    }else {
        ListBoxData_HistoryBlankLine(hListBox);
    }
    

    VScrollBar_Recalculate(hVScrollBar);

    tws_ResumeTWS();
}


// ========================================================================================
// Populate the ListBox with the closed trades
// ========================================================================================
void TradesPanel_ShowClosedTrades()
{
    HWND hListBox = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_LISTBOX);
    HWND hVScrollBar = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_VSCROLLBAR);
    HWND hLabel = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_LABEL);

    tws_PauseTWS();

    // Prevent ListBox redrawing until all calculations are completed
    SendMessage(hListBox, WM_SETREDRAW, FALSE, 0);


    struct ClosedData {
        std::wstring closedDate;
        Trade* trade = nullptr;
    };

    std::vector<ClosedData> vectorClosed;
    vectorClosed.reserve(1000);         // reserve space for 1000 closed trades

    for (auto& trade : trades) {
        if (!trade->isOpen) {
            ClosedData data;

            // Iterate the transactions to find the latest closed date
            for (auto& trans : trade->transactions) {
                if (trans->transDate > data.closedDate) {
                    data.closedDate = trans->transDate;
                }
            }
            data.trade = trade;
            vectorClosed.push_back(data);
        }
    }


    // Destroy any existing ListBox line data
    ListBoxData_DestroyItemData(hListBox);


    // Sort the closed vector based on trade closed date
    std::sort(vectorClosed.begin(), vectorClosed.end(),
        [](const ClosedData data1, const ClosedData data2) {
            return (data1.closedDate > data2.closedDate) ? true : false;
        });


    for (const auto& ClosedData : vectorClosed) {
        ListBoxData_OutputClosedPosition(hListBox, ClosedData.trade, ClosedData.closedDate);
    }


    // Calculate the actual column widths based on the size of the strings in
    // ListBoxData while respecting the minimum values as defined in nMinColWidth[].
    // This function is also called when receiving new price data from TWS because
    // that data may need the column width to be wider.
    ListBoxData_ResizeColumnWidths(hListBox, TableType::ClosedTrades, -1);


    // Select the correct menu panel item
    MenuPanel_SelectMenuItem(HWND_MENUPANEL, IDC_MENUPANEL_CLOSEDTRADES);

    
    // Set the label text indicated the type of trades being listed
    SuperLabel_SetText(hLabel, L"Closed Trades");


    // Need to force a resize of the TradesPanel in order to properly show (or not show) 
    // and position the Header control.
    RECT rc; GetClientRect(HWND_TRADESPANEL, &rc);
    TradesPanel_OnSize(HWND_TRADESPANEL, 0, rc.right, rc.bottom);


    // Redraw the ListBox to ensure that any recalculated columns are 
    // displayed correctly. Re-enable redraw.
    SendMessage(hListBox, WM_SETREDRAW, TRUE, 0);
    AfxRedrawWindow(hListBox);


    // If closed trades exist then select the first trade so that its history will show
    if (ListBox_GetCount(hListBox)) {
        TradesPanel_ShowListBoxItem(0);
    }
    else {
        ListBoxData_HistoryBlankLine(hListBox);
    }

    VScrollBar_Recalculate(hVScrollBar);

    tws_ResumeTWS();

}


// ========================================================================================
// Header control subclass Window procedure
// ========================================================================================
LRESULT CALLBACK TradesPanel_Header_SubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {

    case WM_ERASEBKGND:
    {
        return TRUE;
    }


    case WM_PAINT:
    {
        Header_OnPaint(hWnd);
        break;
    }


    case WM_DESTROY:

        // REQUIRED: Remove control subclassing
        RemoveWindowSubclass(hWnd, TradesPanel_Header_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


// ========================================================================================
// Select a line in the Listbox and deselect any other lines that do not match this
// new line's Trade pointer.
// ========================================================================================
void TradesPanel_SelectListBoxItem(HWND hWnd, int idx)
{
    // Show the trade history for the selected trade
    TradesPanel_ShowListBoxItem(idx);

    // Get the trade pointer for the newly selected line.
    Trade* trade = nullptr;
    ListBoxData* ld = (ListBoxData*)ListBox_GetItemData(hWnd, idx);
    if (ld == nullptr) return;

    trade = ld->trade;

    // Check to see if other lines in the listbox are selected. We will deselect those other
    // lines if they do not belong to the current trade being selected.
    // If the line being clicked on is the main header line for the trade (IsTickerLine)
    // then we deselected everything else including legs of this same trade. We do this
    // because there are different popup menu actions applicable to the main header line
    // and/or the individual trade legs.
    bool IsTickerLine = ld->isTickerLine;

    int nCount = ListBox_GetSelCount(hWnd);

    if (nCount) {
        int* selItems = new int[nCount]();
        SendMessage(hWnd, LB_GETSELITEMS, (WPARAM)nCount, (LPARAM)selItems);

        for (int i = 0; i < nCount; i++)
        {
            ld = (ListBoxData*)ListBox_GetItemData(hWnd, selItems[i]);
            if (ld != nullptr) {
                if (ld->trade != trade) {
                    ListBox_SetSel(hWnd, false, selItems[i]);
                }
                else {
                    // If selecting items within the same Trade then do not allow
                    // mixing selections of the header line and individual legs.
                    if (IsTickerLine != ld->isTickerLine) {
                        ListBox_SetSel(hWnd, false, selItems[i]);
                    }
                }
            }
        }

        delete[] selItems;
    }
}


// ========================================================================================
// Handle the right-click popup menu on the ListBox's selected lines.
// ========================================================================================
void TradesPanel_RightClickMenu(HWND hWnd, int idx)
{
    HMENU hMenu = CreatePopupMenu();

    std::wstring wszText;
    std::wstring wszPlural;
    int nCount = ListBox_GetSelCount(hWnd);

    bool IsTickerLine = false;

    if (nCount == 1) {
        // Is this the Trade header line
        int nCurSel = ListBox_GetCurSel(hWnd);
        ListBoxData* ld = (ListBoxData*)ListBox_GetItemData(hWnd, nCurSel);
        if (ld != nullptr) {
            if (ld->isTickerLine) IsTickerLine = true;
        }
    }
    else {
        wszPlural = L"s";
    }

    if (IsTickerLine) {
        InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 1, L"Close Entire Trade");
        InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 2, L"Expire Entire Trade");
    }
    else {
        wszText = L"Roll Leg" + wszPlural;
        InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 3, wszText.c_str());

        wszText = L"Close Leg" + wszPlural;
        InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 4, wszText.c_str());

        wszText = L"Expire Leg" + wszPlural;
        InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 5, wszText.c_str());

        if (nCount == 1) {
            InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_SEPARATOR | MF_ENABLED, 6, L"");
            InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 7, L"Shares Assignment");
        }
    }

    InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_SEPARATOR | MF_ENABLED, 8, L"");
    InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 9, L"Add Transaction to Trade");
    InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 10, L"Add Put");
    InsertMenu(hMenu, 0, MF_BYCOMMAND | MF_STRING | MF_ENABLED, 11, L"Add Call");

    POINT pt; GetCursorPos(&pt);
    int selected =
        TrackPopupMenu(hMenu, TPM_TOPALIGN | TPM_LEFTALIGN | TPM_RETURNCMD, pt.x, pt.y, 0, hWnd, NULL);

    std::cout << "Menu Selected: " << selected << std::endl;

}


// ========================================================================================
// Listbox subclass Window procedure
// ========================================================================================
LRESULT CALLBACK TradesPanel_ListBox_SubclassProc(
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
        HWND hVScrollBar = GetDlgItem(HWND_TRADESPANEL, IDC_TRADES_VSCROLLBAR);
        VScrollBar_Recalculate(hVScrollBar);
        break;
    }


    case WM_RBUTTONDOWN:
    {
        int idx = Listbox_ItemFromPoint(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        
        // The return value contains the index of the nearest item in the LOWORD. The HIWORD is zero 
        // if the specified point is in the client area of the list box, or one if it is outside the 
        // client area.
        if (HIWORD(idx) == -1) break;
        
        TradesPanel_SelectListBoxItem(hWnd, idx);
        ListBox_SetSel(hWnd, true, idx);

        TradesPanel_RightClickMenu(hWnd, idx);
    }
    break;


    case WM_LBUTTONDOWN:
    {
        int idx = Listbox_ItemFromPoint(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        // The return value contains the index of the nearest item in the LOWORD. The HIWORD is zero 
        // if the specified point is in the client area of the list box, or one if it is outside the 
        // client area.
        if (HIWORD(idx) == -1) break;
        TradesPanel_SelectListBoxItem(hWnd, idx);
    }
    break;


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
        RemoveWindowSubclass(hWnd, TradesPanel_ListBox_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}



// ========================================================================================
// Process WM_MEASUREITEM message for window/dialog: TradesPanel
// ========================================================================================
void TradesPanel_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
    // The ListBox was created using LBS_OWNERDRAWVARIABLE so that this OnMeasureItem
    // would get called for every item being drawn. This allows us to change the item
    // height depending on what type of table data is being displayed. We can't rely
    // on lpMeasureItem->CtrlID because we are usign the same ListBox to display data
    // for open and closed trades.

    int menuId = MenuPanel_GetActiveMenuItem(HWND_MENUPANEL);
    switch (menuId)
    {
    case IDC_MENUPANEL_ACTIVETRADES:
        lpMeasureItem->itemHeight = AfxScaleY(ACTIVE_TRADES_LISTBOX_ROWHEIGHT);
        break;
    case IDC_MENUPANEL_CLOSEDTRADES:
        lpMeasureItem->itemHeight = AfxScaleY(CLOSED_TRADES_LISTBOX_ROWHEIGHT);
        break;

    default:
        lpMeasureItem->itemHeight = AfxScaleY(ACTIVE_TRADES_LISTBOX_ROWHEIGHT);
    }
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: TradesPanel
// ========================================================================================
BOOL TradesPanel_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: TradesPanel
// ========================================================================================
void TradesPanel_OnPaint(HWND hwnd)
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
// Process WM_SIZE message for window/dialog: TradesPanel
// ========================================================================================
void TradesPanel_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    HWND hHeader = GetDlgItem(hwnd, IDC_TRADES_HEADER);
    HWND hListBox = GetDlgItem(hwnd, IDC_TRADES_LISTBOX);
    HWND hVScrollBar = GetDlgItem(hwnd, IDC_TRADES_VSCROLLBAR);
        
    int margin = AfxScaleY(TRADESPANEL_MARGIN);

    HDWP hdwp = BeginDeferWindowPos(4);

    // Move and size the top label into place
    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRADES_LABEL), 0,
        0, 0, cx, margin, SWP_NOZORDER | SWP_SHOWWINDOW);


    // Do not call the calcVThumbRect() function during a scrollbar move. This WM_SIZE
    // gets triggered when the ListBox WM_DRAWITEM fires. If we do another calcVThumbRect()
    // calcualtion then the scrollbar will appear "jumpy" under the user's mouse cursor.
    bool bShowScrollBar = false;
    VScrollBar* pData = VScrollBar_GetPointer(hVScrollBar);
    if (pData != nullptr) {
        if (pData->bDragActive) {
            bShowScrollBar = true;
        }
        else {
            bShowScrollBar = pData->calcVThumbRect();
        }
    }
    int VScrollBarWidth = bShowScrollBar ? AfxScaleX(VSCROLLBAR_WIDTH) : 0;


    int nLeft = 0;
    int nTop = margin;
    int nWidth = cx;
    int nHeight = AfxScaleY(CLOSED_TRADES_LISTBOX_ROWHEIGHT);

    int menuId = MenuPanel_GetActiveMenuItem(HWND_MENUPANEL);
    if (menuId == IDC_MENUPANEL_CLOSEDTRADES) {
        hdwp = DeferWindowPos(hdwp, hHeader, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
        nTop = nTop + nHeight + AfxScaleY(1);
    }
    else {
        ShowWindow(hHeader, SW_HIDE);
    }


    nWidth = cx - VScrollBarWidth;
    nHeight = cy - nTop; 
    hdwp = DeferWindowPos(hdwp, hListBox, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nLeft + nWidth;   // right edge of ListBox
    nWidth = VScrollBarWidth;
    hdwp = DeferWindowPos(hdwp, hVScrollBar, 0, nLeft, nTop, nWidth, nHeight,
        SWP_NOZORDER | (bShowScrollBar ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

    EndDeferWindowPos(hdwp);
}


// ========================================================================================
// Process WM_CREATE message for window/dialog: TradesPanel
// ========================================================================================
BOOL TradesPanel_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_TRADESPANEL = hwnd;
        
    HWND hCtl = SuperLabel_SimpleLabel(hwnd, IDC_TRADES_LABEL, L"Active Trades", 
        ThemeElement::MenuPanelText, ThemeElement::MenuPanelBack);
    
    hCtl = TradesPanel.AddControl(Controls::Header, hwnd, IDC_TRADES_HEADER, L"", 
        0, 0, 0, 0, -1, -1, NULL, (SUBCLASSPROC)TradesPanel_Header_SubclassProc,
        IDC_TRADES_HEADER, NULL);
    int nWidth = AfxScaleX(50);
    Header_InsertNewItem(hCtl, 0, nWidth, L"", HDF_CENTER);
    Header_InsertNewItem(hCtl, 1, nWidth, L"Date", HDF_LEFT);
    Header_InsertNewItem(hCtl, 2, nWidth, L"Ticker", HDF_LEFT);
    Header_InsertNewItem(hCtl, 3, nWidth, L"Company Name", HDF_LEFT);
    Header_InsertNewItem(hCtl, 4, nWidth, L"Amount", HDF_RIGHT);
    // Must turn off Window Theming for the control in order to correctly apply colors via NM_CUSTOMDRAW
    SetWindowTheme(hCtl, L"", L"");


    // Create an Ownerdraw variable row sized listbox that we will use to custom
    // paint our various open trades.
    hCtl =
        TradesPanel.AddControl(Controls::ListBox, hwnd, IDC_TRADES_LISTBOX, L"",
            0, 0, 0, 0,
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP |
            LBS_NOINTEGRALHEIGHT | LBS_MULTIPLESEL | LBS_EXTENDEDSEL | 
            LBS_OWNERDRAWVARIABLE | LBS_NOTIFY,
            WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR, NULL,
            (SUBCLASSPROC)TradesPanel_ListBox_SubclassProc,
            IDC_TRADES_LISTBOX, NULL);
    ListBox_AddString(hCtl, NULL);


    // Create our custom vertical scrollbar and attach the ListBox to it.
    CreateVScrollBar(hwnd, IDC_TRADES_VSCROLLBAR, hCtl);

    return TRUE;
}


// ========================================================================================
// Process WM_CONTEXTMENU message for window/dialog: TradesPanel
// ========================================================================================
void TradesPanel_OnContextMenu(HWND hwnd, HWND hwndContext, UINT xPos, UINT yPos)
{
    if (hwndContext == GetDlgItem(hwnd, IDC_TRADES_LISTBOX)) {

    }
}


// ========================================================================================
// Windows callback function.
// ========================================================================================
LRESULT CTradesPanel::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CONTEXTMENU, TradesPanel_OnContextMenu);
        HANDLE_MSG(m_hwnd, WM_CREATE, TradesPanel_OnCreate);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, TradesPanel_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, TradesPanel_OnPaint);
        HANDLE_MSG(m_hwnd, WM_SIZE, TradesPanel_OnSize);
        HANDLE_MSG(m_hwnd, WM_MEASUREITEM, TradesPanel_OnMeasureItem);
        HANDLE_MSG(m_hwnd, WM_DRAWITEM, ListBoxData_OnDrawItem);

    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}

