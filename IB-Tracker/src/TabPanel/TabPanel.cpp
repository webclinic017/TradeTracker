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

#include "ActiveTrades/ActiveTrades.h"
#include "ClosedTrades/ClosedTrades.h"
#include "Transactions/TransPanel.h"
#include "TickerTotals/TickerTotals.h"
#include "TradePlan/TradePlan.h"
#include "JournalNotes/JournalNotes.h"
#include "MainWindow/tws-client.h"
#include "CustomLabel/CustomLabel.h"

#include "TabPanel.h"


CTabPanel TabPanel;

HWND HWND_TABPANEL = NULL;


// ========================================================================================
// Select the incoming CtrlId in the TabPanel and deselect the others
// ========================================================================================
void TabPanel_SelectPanelItem(HWND hwnd, int CtrlId)
{
    static std::vector<int> panels;
    panels.push_back(IDC_TABPANEL_ACTIVETRADES);
    panels.push_back(IDC_TABPANEL_CLOSEDTRADES);
    panels.push_back(IDC_TABPANEL_TRANSACTIONS);
    panels.push_back(IDC_TABPANEL_TICKERTOTALS);
    panels.push_back(IDC_TABPANEL_JOURNALNOTES);
    panels.push_back(IDC_TABPANEL_TRADEPLAN);

    for (auto& panel : panels) {
        HWND hPanel = GetDlgItem(hwnd, panel);
        if (panel == CtrlId) {
            CustomLabel_SetSelected(hPanel, true);
            CustomLabel_SetSelectedUnderline(hPanel, true, COLOR_GREEN, 3);
        }
        else {
            CustomLabel_SetSelected(hPanel, false);
        }
    }
}


// ========================================================================================
// Return the Control ID of the selected TabPanel panel.
// ========================================================================================
int TabPanel_GetSelectedPanel(HWND hwnd)
{
    static std::vector<int> panels;
    panels.push_back(IDC_TABPANEL_ACTIVETRADES);
    panels.push_back(IDC_TABPANEL_CLOSEDTRADES);
    panels.push_back(IDC_TABPANEL_TRANSACTIONS);
    panels.push_back(IDC_TABPANEL_TICKERTOTALS);
    panels.push_back(IDC_TABPANEL_JOURNALNOTES);
    panels.push_back(IDC_TABPANEL_TRADEPLAN);

    for (auto& panel : panels) {
        HWND hPanel = GetDlgItem(hwnd, panel);
        if (CustomLabel_GetSelected(hPanel)) {
            return panel;
        }
    }
    return -1;
}




// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: TabPanel
// ========================================================================================
BOOL TabPanel_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: TabPanel
// ========================================================================================
void TabPanel_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    Graphics graphics(hdc);

    // Create the background brush
    Color back_color(COLOR_BLACK);
    SolidBrush back_brush(back_color);

    // Paint the background using brush.
    int nWidth = (ps.rcPaint.right - ps.rcPaint.left);
    int nHeight = (ps.rcPaint.bottom - ps.rcPaint.top);
    graphics.FillRectangle(&back_brush, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight);

    EndPaint(hwnd, &ps);
}


// ========================================================================================
// Process WM_SIZE message for window/dialog: TabPanel
// ========================================================================================
void TabPanel_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
}


// ========================================================================================
// Process WM_CREATE message for window/dialog: TabPanel
// ========================================================================================
BOOL TabPanel_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_TABPANEL = hwnd;

    int image_size = 18;

    int item_top = 0;
    int item_left = 40;
    int item_height = TABPANEL_HEIGHT - 2;
    int item_width = 0;

    int separator_left = 0;
    int separator_top = 6;
    int separator_width = 6;
    int separator_height = item_height - (separator_top * 2);

    std::wstring font_name = AfxGetDefaultFont();
    int font_size = 9;

    CustomLabel* pData = nullptr;

    HWND hCtl = CustomLabel_SimpleImageLabel(
        hwnd, IDC_TABPANEL_CONNECT, 
        IDB_DISCONNECT, IDB_DISCONNECT,
        image_size, image_size,
        12, 10, image_size, image_size);
    std::wstring tooltip_text = L"Click to connect";
    CustomLabel_SetToolTip(hCtl, tooltip_text);

    item_width = 90;
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TABPANEL_ACTIVETRADES, L"Active Trades",
        COLOR_WHITEMEDIUM, COLOR_BLACK, CustomLabelAlignment::middle_center,
        item_left, item_top, item_width, item_height);
    CustomLabel_SetFontHot(hCtl, font_name, font_size, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITE);
    CustomLabel_SetBackColorSelected(hCtl, COLOR_GRAYDARK);

    separator_left = item_left + item_width;
    hCtl = CustomLabel_VerticalLine(hwnd, IDC_TABPANEL_SEPARATOR, COLOR_WHITEDARK, COLOR_BLACK,
        separator_left, separator_top, separator_width, separator_height);
        
    
    item_left = separator_left + separator_width;
    item_width = 90;
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TABPANEL_CLOSEDTRADES, L"Closed Trades",
        COLOR_WHITEMEDIUM, COLOR_BLACK, CustomLabelAlignment::middle_center,
        item_left, item_top, item_width, item_height);
    CustomLabel_SetFontHot(hCtl, font_name, font_size, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITE);
    CustomLabel_SetBackColorSelected(hCtl, COLOR_GRAYDARK);

    separator_left = item_left + item_width;
    hCtl = CustomLabel_VerticalLine(hwnd, IDC_TABPANEL_SEPARATOR, COLOR_WHITEDARK, COLOR_BLACK,
        separator_left, separator_top, separator_width, separator_height);

    
    item_left = separator_left + separator_width;
    item_width = 90;
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TABPANEL_TRANSACTIONS, L"Transactions",
        COLOR_WHITEMEDIUM, COLOR_BLACK, CustomLabelAlignment::middle_center,
        item_left, item_top, item_width, item_height);
    CustomLabel_SetFontHot(hCtl, font_name, font_size, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITE);
    CustomLabel_SetBackColorSelected(hCtl, COLOR_GRAYDARK);

    separator_left = item_left + item_width;
    hCtl = CustomLabel_VerticalLine(hwnd, IDC_TABPANEL_SEPARATOR, COLOR_WHITEDARK, COLOR_BLACK,
        separator_left, separator_top, separator_width, separator_height);

    
    item_left = separator_left + separator_width;
    item_width = 90;
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TABPANEL_TICKERTOTALS, L"Ticker Totals",
        COLOR_WHITEMEDIUM, COLOR_BLACK, CustomLabelAlignment::middle_center,
        item_left, item_top, item_width, item_height);
    CustomLabel_SetFontHot(hCtl, font_name, font_size, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITE);
    CustomLabel_SetBackColorSelected(hCtl, COLOR_GRAYDARK);

    separator_left = item_left + item_width;
    hCtl = CustomLabel_VerticalLine(hwnd, IDC_TABPANEL_SEPARATOR, COLOR_WHITEDARK, COLOR_BLACK,
        separator_left, separator_top, separator_width, separator_height);


    item_left = separator_left + separator_width;
    item_width = 90;
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TABPANEL_JOURNALNOTES, L"Journal Notes",
        COLOR_WHITEMEDIUM, COLOR_BLACK, CustomLabelAlignment::middle_center,
        item_left, item_top, item_width, item_height);
    CustomLabel_SetFontHot(hCtl, font_name, font_size, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITE);
    CustomLabel_SetBackColorSelected(hCtl, COLOR_GRAYDARK);

    separator_left = item_left + item_width;
    hCtl = CustomLabel_VerticalLine(hwnd, IDC_TABPANEL_SEPARATOR, COLOR_WHITEDARK, COLOR_BLACK,
        separator_left, separator_top, separator_width, separator_height);


    item_left = separator_left + separator_width;
    item_width = 90;
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TABPANEL_TRADEPLAN, L"Trade Plan",
        COLOR_WHITEMEDIUM, COLOR_BLACK, CustomLabelAlignment::middle_center,
        item_left, item_top, item_width, item_height);
    CustomLabel_SetFontHot(hCtl, font_name, font_size, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITE);
    CustomLabel_SetBackColorSelected(hCtl, COLOR_GRAYDARK);

    separator_left = item_left + item_width;
    hCtl = CustomLabel_VerticalLine(hwnd, IDC_TABPANEL_SEPARATOR, COLOR_WHITEDARK, COLOR_BLACK,
        separator_left, separator_top, separator_width, separator_height);


    TabPanel_SelectPanelItem(hwnd, IDC_TABPANEL_ACTIVETRADES);

    return TRUE;
}


// ========================================================================================
// TabPanel Window procedure
// ========================================================================================
LRESULT CTabPanel::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, TabPanel_OnCreate);
        HANDLE_MSG(m_hwnd, WM_SIZE, TabPanel_OnSize);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, TabPanel_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, TabPanel_OnPaint);


    case MSG_TWS_CONNECT_START:
    {
        SetCursor(LoadCursor(0, IDC_WAIT));
        //int index = AfxReplaceListBoxData(hListBox, listbox_index_connecttws, L"Connecting to TWS", IDC_SIDEMENU_CONNECTTWS);
        return 0;
    }
    break;


    case MSG_TWS_CONNECT_SUCCESS:
    {
        SetCursor(LoadCursor(0, IDC_ARROW));
        HWND hCtl = GetDlgItem(HWND_TABPANEL, IDC_TABPANEL_CONNECT);
        //int index = AfxReplaceListBoxData(hListBox, listbox_index_connecttws, L"TWS Connected", IDC_SIDEMENU_CONNECTTWS);
        CustomLabel_SetImages(hCtl, IDB_CONNECT, IDB_CONNECT);
        CustomLabel_SetToolTip(hCtl, L"Connected");
        return 0;
    }
    break;


    case MSG_TWS_CONNECT_WAIT_RECONNECTION:
    {
        SetCursor(LoadCursor(0, IDC_ARROW));
        //int index = AfxReplaceListBoxData(hListBox, listbox_index_connecttws, L"Reconnect Wait", IDC_SIDEMENU_CONNECTTWS);
        return 0;
    }
    break;


    case MSG_TWS_WARNING_EXCEPTION:
    {
        SetCursor(LoadCursor(0, IDC_ARROW));
        //CustomLabel_SetText(GetDlgItem(HWND_MAINWINDOW, IDC_MAINWINDOW_WARNING),
        //    L"Monitoring thread exception! Please restart application.");
        //ShowWindow(GetDlgItem(HWND_MAINWINDOW, IDC_MAINWINDOW_WARNING), SW_SHOWNORMAL);
        return 0;
    }
    break;


    case MSG_TWS_CONNECT_FAILURE:
    case MSG_TWS_CONNECT_DISCONNECT:
    {
        SetCursor(LoadCursor(0, IDC_ARROW));
        HWND hCtl = GetDlgItem(HWND_TABPANEL, IDC_TABPANEL_CONNECT);
        //int index = AfxReplaceListBoxData(hListBox, listbox_index_connecttws, L"TWS Connected", IDC_SIDEMENU_CONNECTTWS);
        CustomLabel_SetImages(hCtl, IDB_DISCONNECT, IDB_DISCONNECT);
        CustomLabel_SetToolTip(hCtl, L"Click to connect");
        tws_EndMonitorThread();
        return 0;
    }
    break;


    case MSG_CUSTOMLABEL_CLICK:
    {
        HWND hCtl = (HWND)lParam;
        int CtrlId = (int)wParam;

        if (hCtl == NULL) return 0;

        if (CtrlId == IDC_TABPANEL_CONNECT) {
            // If already connected then don't try to connect again
            if (!tws_IsConnected()) {
                // Prevent multiple clicks of the connect button by waiting until
                // the first click is finished.
                static bool processing_connect_click = false;
                if (processing_connect_click) break;
                processing_connect_click = true;
                bool res = tws_Connect();
                processing_connect_click = false;
            }
            break;
        }

        // Do not refresh if the panel is already active
        if (CtrlId == TabPanel_GetSelectedPanel(m_hwnd)) {
            break;
        }


        TabPanel_SelectPanelItem(m_hwnd, CtrlId);

        if (CtrlId == IDC_TABPANEL_ACTIVETRADES) {
            ActiveTrades_ShowActiveTrades(true);
        }
        if (CtrlId == IDC_TABPANEL_CLOSEDTRADES) {
            ClosedTrades_ShowClosedTrades();
        }
        if (CtrlId == IDC_TABPANEL_TRANSACTIONS) {
            TransPanel_ShowTransactions();
        }
        if (CtrlId == IDC_TABPANEL_TICKERTOTALS) {
            TickerPanel_ShowTickerTotals();
        }
        if (CtrlId == IDC_TABPANEL_JOURNALNOTES) {
            JournalNotes_ShowJournalNotes();
        }
        if (CtrlId == IDC_TABPANEL_TRADEPLAN) {
            TradePlan_ShowTradePlan();
        }

        return 0;
    }
    break;

    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);

    }   // end of switch statement

    return 0;

}
