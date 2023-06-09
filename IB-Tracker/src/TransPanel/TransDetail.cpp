/*

MIT License

Copyright(c) 2023 Paul Squires

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "pch.h"
#include "CustomLabel\CustomLabel.h"
#include "CustomVScrollBar\CustomVScrollBar.h"
#include "MainWindow\tws-client.h"
#include "Database\database.h"
#include "TradeDialog\TradeDialog.h"
#include "Utilities\ListBoxData.h"

#include "TransDetail.h"


HWND HWND_TRANSDETAIL = NULL;

extern CTransDetail TransDetail;

extern std::vector<std::shared_ptr<Trade>> trades;
extern int nColWidth[];

extern HWND HWND_MENUPANEL;

extern void MainWindow_SetRightPanel(HWND hPanel);
extern void TransPanel_ShowTransactions();

extern TradeDialogData tdd;

std::shared_ptr<Trade> tradeEditDelete = nullptr;
std::shared_ptr<Transaction> transEditDelete = nullptr;



// ========================================================================================
// Retrieve the Leg pointer based on a leg's backPointerID.
// ========================================================================================
std::shared_ptr<Leg> TransDetail_GetLegBackPointer(std::shared_ptr<Trade> trade, int backPointerID)
{
    for (auto trans : trade->transactions) {
        for (auto leg : trans->legs) {
            if (leg->legID == backPointerID) {
                return leg;
            }
        }
    }
    return nullptr;
}
    
    
// ========================================================================================
// Display the selected Transaction detail (legs) and ability to Edit it.
// ========================================================================================
void TransDetail_EditTransaction(HWND hwnd)
{
    if (tradeEditDelete == nullptr) return;
    if (transEditDelete == nullptr) return;

    tdd.legs.clear();
    tdd.legs = transEditDelete->legs;
    tdd.trade = tradeEditDelete;
    tdd.trans = transEditDelete;
    TradeDialog_Show(TradeAction::EditTransaction);
}
    

// ========================================================================================
// Delete (after confirmation) the selected Transaction.
// ========================================================================================
void TransDetail_DeleteTransaction(HWND hwnd)
{
    if (tradeEditDelete == nullptr) return;
    if (transEditDelete == nullptr) return;

    int res = MessageBox(
        HWND_TRANSDETAIL,
        (LPCWSTR)(L"Are you sure you wish to DELETE this Transaction?"),
        (LPCWSTR)L"Confirm",
        MB_ICONWARNING | MB_YESNOCANCEL | MB_DEFBUTTON2);

    if (res != IDYES) return;

    // Iterate the trades and look into each transactions vector to match the 
    // transaction that we need to delete.
    for (auto trade : trades) {
        auto iter = trade->transactions.begin();
        while (iter != trade->transactions.end())
        {
            // If element matches the element to be deleted then delete it
            if (*iter == transEditDelete)
            {
                // Cycle through the legs being deleted to see if any contains a leg backpointer.
                // If yes, then retrieve the leg related to that pointer and update its open
                // quantity amount. Backpointers exist for transactions that modify quantity
                // amounts like CLOSE, EXPIRE, ROLL.
                for (auto leg : transEditDelete->legs) {
                    if (leg->legBackPointerID != 0) {
                        // Get the backpointer leg.
                        std::shared_ptr<Leg> LegBackPointer = nullptr;
                        LegBackPointer = TransDetail_GetLegBackPointer(trade, leg->legBackPointerID);
                        if (LegBackPointer != nullptr) {
                            LegBackPointer->openQuantity = LegBackPointer->openQuantity - leg->origQuantity;
                        }
                    }
                }
                iter = trade->transactions.erase(iter);
            }
            else
            {
                iter++;
            }
        }

    }
    
    // Calculate the Trade open status because we may have just deleted the 
    // transaction that sets the trades OpenQuantity to zero.
    tradeEditDelete->setTradeOpenStatus();


    tws_PauseTWS();

    // Save the modified data
    SaveDatabase();
    LoadDatabase();

    tws_ResumeTWS();

    tradeEditDelete = nullptr;
    transEditDelete = nullptr;

    // Show our new list of transactions.
    TransPanel_ShowTransactions();


}


// ========================================================================================
// Display the selected Transaction detail (legs) and ability to Edit/Delete it.
// ========================================================================================
void TransPanel_ShowTransactionDetail(const std::shared_ptr<Trade> trade, const std::shared_ptr<Transaction> trans)
{
    if (trade == nullptr) return;
    if (trans == nullptr) return;

    tradeEditDelete = trade;
    transEditDelete = trans;

    // Clear the current transaction history table
    HWND hListBox = GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_LISTBOX);
    ListBoxData_DestroyItemData(hListBox);

    ListBoxData_HistoryHeader(hListBox, trade, trans);

    CustomLabel_SetText(
        GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_LABEL1),
        L"Transaction Details");

    // Construct the string to display the cost details of the selected transaction
    std::wstring wszDRCR = (transEditDelete->total < 0) ? L" DR" : L" CR";
    std::wstring wszPlusMinus = (transEditDelete->total < 0) ? L" + " : L" - ";
    std::wstring wszText;
    wszText = std::to_wstring(transEditDelete->quantity) + L" @ " +
                AfxMoney(transEditDelete->price, false, 4) + wszPlusMinus +
                AfxMoney(transEditDelete->fees) + L" = " +
                AfxMoney(abs(transEditDelete->total)) + wszDRCR;
    CustomLabel_SetText(GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_LBLCOST), wszText);
    
    CustomLabel_SetText(GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_CATEGORY), GLYPH_CIRCLE);
    CustomLabel_SetTextColor(
        GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_CATEGORY),
        GetCategoryColor(trade->category));

    wszText = trade->tickerSymbol + L": " + trade->tickerName;
    if (trade->futureExpiry.length()) {
        wszText = wszText + L" ( " + AfxFormatFuturesDate(trade->futureExpiry) + L" )";
    }
    CustomLabel_SetText(
        GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_SYMBOL), wszText);


    // Show the detail leg information for this transaction.
    for (const auto& leg : trans->legs) {
        if (leg->underlying == L"OPTIONS") { ListBoxData_HistoryOptionsLeg(hListBox, trade, trans, leg); }
        else if (leg->underlying == L"SHARES") { ListBoxData_HistorySharesLeg(hListBox, trade, trans, leg); }
        else if (leg->underlying == L"FUTURES") {
            ListBoxData_HistorySharesLeg(hListBox, trade, trans, leg);
        }
    }

    ListBoxData_AddBlankLine(hListBox);

    // Calculate the actual column widths based on the size of the strings in
    // ListBoxData while respecting the minimum values as defined in nMinColWidth[].
    // This function is also called when receiving new price data from TWS because
    // that data may need the column width to be wider.
    ListBoxData_ResizeColumnWidths(hListBox, TableType::TradeHistory, -1);

    // Ensure that the Transaction Detail panel is set
    MainWindow_SetRightPanel(HWND_TRANSDETAIL);

    // Set the ListBox to the topline.
    ListBox_SetTopIndex(hListBox, 0);

}


// ========================================================================================
// Listbox subclass Window procedure
// ========================================================================================
LRESULT CALLBACK TransDetail_ListBox_SubclassProc(
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
        HWND hCustomVScrollBar = GetDlgItem(HWND_TRANSDETAIL, IDC_TRANSDETAIL_CUSTOMVSCROLLBAR);
        CustomVScrollBar_Recalculate(hCustomVScrollBar);
        return 0;
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
            SolidBrush backBrush(GetThemeColor(ThemeElement::GrayDark));
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
        RemoveWindowSubclass(hWnd, TransDetail_ListBox_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


// ========================================================================================
// Process WM_MEASUREITEM message for window/dialog: TransDetail
// ========================================================================================
void TransDetail_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
    lpMeasureItem->itemHeight = AfxScaleY(TRANSDETAIL_LISTBOX_ROWHEIGHT);
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: TransDetail
// ========================================================================================
BOOL TransDetail_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: TransDetail
// ========================================================================================
void TransDetail_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    Graphics graphics(hdc);

    DWORD nBackColor = GetThemeColor(ThemeElement::Black);

    // Create the background brush
    SolidBrush backBrush(nBackColor);

    // Paint the background using brush.
    int nWidth = (ps.rcPaint.right - ps.rcPaint.left);
    int nHeight = (ps.rcPaint.bottom - ps.rcPaint.top);
    graphics.FillRectangle(&backBrush, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight);

    EndPaint(hwnd, &ps);
}


// ========================================================================================
// Process WM_SIZE message for window/dialog: TransDetail
// ========================================================================================
void TransDetail_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    HWND hListBox = GetDlgItem(hwnd, IDC_TRANSDETAIL_LISTBOX);
    HWND hCustomVScrollBar = GetDlgItem(hwnd, IDC_TRANSDETAIL_CUSTOMVSCROLLBAR);

    int margin = AfxScaleY(TRANSDETAIL_MARGIN);
    int nTop = 0;
    int nLeft = 0;
    int nButtonWidth = AfxScaleX(80);
    int nButtonHeight = AfxScaleX(23);

    HDWP hdwp = BeginDeferWindowPos(10);

    // Move and size the top label into place
    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRANSDETAIL_LABEL1), 0,
        0, nTop, cx, margin, SWP_NOZORDER | SWP_SHOWWINDOW);

    nTop += margin;
    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRANSDETAIL_CATEGORY), 0,
        0, nTop, AfxScaleX(18), margin, SWP_NOZORDER | SWP_SHOWWINDOW);

    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRANSDETAIL_SYMBOL), 0,
        AfxScaleX(18), nTop, cx, margin, SWP_NOZORDER | SWP_SHOWWINDOW);

    nTop += margin;
    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRANSDETAIL_LBLCOST), 0,
        0, nTop, cx - (nButtonWidth * 2) - AfxScaleX(8), margin, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = cx - (nButtonWidth * 2) - AfxScaleX(8);
    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRANSDETAIL_CMDEDIT), 0,
        nLeft, nTop, nButtonWidth, nButtonHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = cx - nButtonWidth;
    hdwp = DeferWindowPos(hdwp, GetDlgItem(hwnd, IDC_TRANSDETAIL_CMDDELETE), 0,
        nLeft, nTop, nButtonWidth, nButtonHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    // Do not call the calcVThumbRect() function during a scrollbar move. This WM_SIZE
    // gets triggered when the ListBox WM_DRAWITEM fires. If we do another calcVThumbRect()
    // calcualtion then the scrollbar will appear "jumpy" under the user's mouse cursor.
    bool bShowScrollBar = false;
    CustomVScrollBar* pData = CustomVScrollBar_GetPointer(hCustomVScrollBar);
    if (pData != nullptr) {
        if (pData->bDragActive) {
            bShowScrollBar = true;
        }
        else {
            bShowScrollBar = pData->calcVThumbRect();
        }
    }
    int CustomVScrollBarWidth = bShowScrollBar ? AfxScaleX(CUSTOMVSCROLLBAR_WIDTH) : 0;

    nTop = AfxScaleY(80);
    nLeft = 0;
    int nWidth = cx - CustomVScrollBarWidth;
    int nHeight = cy - nTop;
    hdwp = DeferWindowPos(hdwp, hListBox, 0, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_SHOWWINDOW);

    nLeft = nLeft + nWidth;   // right edge of ListBox
    nWidth = CustomVScrollBarWidth;
    hdwp = DeferWindowPos(hdwp, hCustomVScrollBar, 0, nLeft, nTop, nWidth, nHeight,
        SWP_NOZORDER | (bShowScrollBar ? SWP_SHOWWINDOW : SWP_HIDEWINDOW));

    EndDeferWindowPos(hdwp);
}


// ========================================================================================
// Process WM_CREATE message for window/dialog: TransDetail
// ========================================================================================
BOOL TransDetail_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_TRANSDETAIL = hwnd;

    std::wstring wszFontName = L"Segoe UI";
    int FontSize = 9;

    HWND hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRANSDETAIL_LABEL1, L"Transaction Details",
        ThemeElement::WhiteLight, ThemeElement::Black);

    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRANSDETAIL_CATEGORY, L"",
        ThemeElement::WhiteDark, ThemeElement::Black);

    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRANSDETAIL_SYMBOL, L"",
        ThemeElement::WhiteDark, ThemeElement::Black);

    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRANSDETAIL_LBLCOST, L"",
        ThemeElement::WhiteDark, ThemeElement::Black);


    // EDIT button
    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRANSDETAIL_CMDEDIT, L"EDIT",
        ThemeElement::Black, ThemeElement::Green, ThemeElement::Green, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleCenter, 0, 0, 0, 0);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
    CustomLabel_SetTextColorHot(hCtl, ThemeElement::WhiteLight);

    // DELETE button
    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRANSDETAIL_CMDDELETE, L"DELETE",
        ThemeElement::Black, ThemeElement::Red, ThemeElement::Red, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleCenter, 0, 0, 0, 0);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
    CustomLabel_SetTextColorHot(hCtl, ThemeElement::WhiteLight);

    // Create an Ownerdraw listbox that we will use to custom paint our transaction detail.
    hCtl =
        TransDetail.AddControl(Controls::ListBox, hwnd, IDC_TRANSDETAIL_LISTBOX, L"",
            0, 0, 0, 0,
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP |
            LBS_NOINTEGRALHEIGHT | LBS_NOSEL |
            LBS_OWNERDRAWFIXED | LBS_NOTIFY,
            WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR, NULL,
            (SUBCLASSPROC)TransDetail_ListBox_SubclassProc,
            IDC_TRANSDETAIL_LISTBOX, NULL);
    ListBox_AddString(hCtl, NULL);

    // Create our custom vertical scrollbar and attach the ListBox to it.
    CreateCustomVScrollBar(hwnd, IDC_TRANSDETAIL_CUSTOMVSCROLLBAR, hCtl);

    return TRUE;
}


// ========================================================================================
// Windows callback function.
// ========================================================================================
LRESULT CTransDetail::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, TransDetail_OnCreate);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, TransDetail_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, TransDetail_OnPaint);
        HANDLE_MSG(m_hwnd, WM_SIZE, TransDetail_OnSize);
        HANDLE_MSG(m_hwnd, WM_MEASUREITEM, TransDetail_OnMeasureItem);
        HANDLE_MSG(m_hwnd, WM_DRAWITEM, ListBoxData_OnDrawItem);


    case MSG_CUSTOMLABEL_CLICK:
    {
        HWND hCtl = (HWND)lParam;
        int CtrlId = (int)wParam;

        if (hCtl == NULL) return 0;

        if (CtrlId == IDC_TRANSDETAIL_CMDEDIT) {
            TransDetail_EditTransaction(m_hwnd);
            return 0;
        }

        if (CtrlId == IDC_TRANSDETAIL_CMDDELETE) {
            TransDetail_DeleteTransaction(m_hwnd);
            return 0;
        }

    }


    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}

