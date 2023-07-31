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

#include "Utilities/CWindowBase.h"
#include "MainWindow/MainWindow.h"
#include "TradeDialog/TradeDialog.h"
#include "Utilities/UserMessages.h"
#include "Config/Config.h"
#include "CustomLabel/CustomLabel.h"
#include "CustomTextBox/CustomTextBox.h"

#include "Category.h"
#include "CategoryDialog.h"


HWND HWND_CATEGORYDIALOG = NULL;

CCategoryDialog CategoryDialog;



// ========================================================================================
// Process WM_CLOSE message for window/dialog: CategoryDialog
// ========================================================================================
void CategoryDialog_OnClose(HWND hwnd)
{
    EnableWindow(HWND_TRADEDIALOG, TRUE);
    DestroyWindow(hwnd);
}


// ========================================================================================
// Process WM_DESTROY message for window/dialog: CategoryDialog
// ========================================================================================
void CategoryDialog_OnDestroy(HWND hwnd)
{
    PostQuitMessage(0);
}


// ========================================================================================
// Process WM_CREATE message for window/dialog: CategoryDialog
// ========================================================================================
BOOL CategoryDialog_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_CATEGORYDIALOG = hwnd;

    DWORD lightBackColor = COLOR_GRAYLIGHT;
    DWORD lightTextColor = COLOR_WHITELIGHT;

    DWORD darkBackColor = COLOR_GRAYMEDIUM;
    DWORD darkTextColor = COLOR_WHITEDARK;

    DWORD TextColor = COLOR_WHITELIGHT;
    DWORD TextColorDim = COLOR_WHITEDARK;
    DWORD BackColor = COLOR_GRAYDARK;

    int HTextMargin = 0;
    int VTextMargin = 3;

    HWND hCtl = NULL;

    CustomLabel_SimpleLabel(hwnd, -1, L"Category", TextColorDim, BackColor,
        CustomLabelAlignment::MiddleLeft, 40, 20, 68, 22);
    CustomLabel_SimpleLabel(hwnd, -1, L"Description", TextColorDim, BackColor,
        CustomLabelAlignment::MiddleLeft, 108, 20, 115, 22);

    int nTop = 45;
    int ctlID = IDC_CATEGORYCONTROL_FIRST;

    for (int i = 0; i < 8; ++i) {
        CustomLabel_SimpleLabel(hwnd, -1, std::to_wstring(i), TextColor, BackColor,
            CustomLabelAlignment::MiddleLeft, 60, nTop, 20, 23);
        hCtl = CreateCustomTextBox(hwnd, ctlID, ES_LEFT, L"", 110, nTop, 300, 23);
        CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
        CustomTextBox_SetColors(hCtl, lightTextColor, darkBackColor);
        AfxSetWindowText(hCtl, GetCategoryDescription(i));
        nTop += 25;
        ctlID++;
    }


    // SAVE button
    std::wstring wszFontName = L"Segoe UI";
    int FontSize = 9;
    bool bold = true;
    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_CATEGORYDIALOG_SAVE, L"SAVE",
        COLOR_BLACK, COLOR_GREEN, COLOR_GREEN, COLOR_GRAYMEDIUM, COLOR_WHITELIGHT,
        CustomLabelAlignment::MiddleCenter, 330, 265, 80, 23);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, bold);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITELIGHT);

    return TRUE;
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: CategoryDialog
// ========================================================================================
BOOL CategoryDialog_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: CategoryDialog
// ========================================================================================
void CategoryDialog_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    SaveDC(hdc);

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP hbit = CreateCompatibleBitmap(hdc, ps.rcPaint.right, ps.rcPaint.bottom);
    SelectBitmap(memDC, hbit);

    Graphics graphics(memDC);
    int nWidth = (ps.rcPaint.right - ps.rcPaint.left);
    int nHeight = (ps.rcPaint.bottom - ps.rcPaint.top);

    // Create the background brush
    SolidBrush backBrush(COLOR_GRAYDARK);
    graphics.FillRectangle(&backBrush, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight);

    // Copy the entire memory bitmap to the main display
    BitBlt(hdc, 0, 0, ps.rcPaint.right, ps.rcPaint.bottom, memDC, 0, 0, SRCCOPY);

    // Restore the original state of the DC
    RestoreDC(hdc, -1);

    // Cleanup
    DeleteObject(hbit);
    DeleteDC(memDC);

    EndPaint(hwnd, &ps);
}


// ========================================================================================
// Windows callback function.
// ========================================================================================
LRESULT CCategoryDialog::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, CategoryDialog_OnCreate);
        HANDLE_MSG(m_hwnd, WM_DESTROY, CategoryDialog_OnDestroy);
        HANDLE_MSG(m_hwnd, WM_CLOSE, CategoryDialog_OnClose);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, CategoryDialog_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, CategoryDialog_OnPaint);


    case WM_SHOWWINDOW:
    {
        // Workaround for the Windows 11 (The cloaking solution seems to work only
        // on Windows 10 whereas this WM_SHOWWINDOW workaround seems to only work
        // on Windows 11).
        // https://stackoverflow.com/questions/69715610/how-to-initialize-the-background-color-of-win32-app-to-something-other-than-whit

        SetWindowLongPtr(m_hwnd,
            GWL_EXSTYLE,
            GetWindowLongPtr(m_hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);

        if (!GetLayeredWindowAttributes(m_hwnd, NULL, NULL, NULL))
        {
            SetLayeredWindowAttributes(m_hwnd, 0, 0, LWA_ALPHA);
            DefWindowProc(m_hwnd, WM_ERASEBKGND, (WPARAM)GetDC(m_hwnd), lParam);
            SetLayeredWindowAttributes(m_hwnd, 0, 255, LWA_ALPHA);
            AnimateWindow(m_hwnd, 1, AW_ACTIVATE | AW_BLEND);
            return 0;
        }
        SetWindowLongPtr(m_hwnd,
            GWL_EXSTYLE,
            GetWindowLong(m_hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

        return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
    break;


    case WM_KEYDOWN:
    {
        // We are handling the TAB naviagation ourselves.
        if (wParam == VK_TAB) {
            HWND hFocus = GetFocus();
            HWND hNextCtrl = NULL;
            if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
                hNextCtrl = GetNextDlgTabItem(m_hwnd, hFocus, TRUE);
            }
            else {
                hNextCtrl = GetNextDlgTabItem(m_hwnd, hFocus, FALSE);
            }
            SetFocus(hNextCtrl);
            return TRUE;
        }
    }


    case MSG_CUSTOMLABEL_CLICK:
    {
        HWND hCtl = (HWND)lParam;
        int CtrlId = (int)wParam;

        if (hCtl == NULL) return 0;

        if (CtrlId == IDC_CATEGORYDIALOG_SAVE) {
            int cID = IDC_CATEGORYCONTROL_FIRST;
            for (int i = 0; i < 8; ++i) {
                SetCategoryDescription(i, AfxGetWindowText(GetDlgItem(m_hwnd, cID)));
                cID++;
            }
            SaveConfig();
            DialogReturnCode = DIALOG_RETURN_OK;
            SendMessage(m_hwnd, WM_CLOSE, 0, 0);
        }

    }
    return 0;


    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}


// ========================================================================================
// Create and show the Category modal dialog.
// ========================================================================================
int CategoryDialog_Show()
{
    int nWidth = 460;
    int nHeight = 345;

    HWND hwnd = CategoryDialog.Create(HWND_TRADEDIALOG, L"Category Management", 0, 0, nWidth, nHeight,
        WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_CONTROLPARENT | WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    // Attempt to apply the standard Windows dark theme to the non-client areas of the main form.
    BOOL value = true;
    ::DwmSetWindowAttribute(hwnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &value, sizeof(value));

    HBRUSH hbrBackground = (HBRUSH)GetStockObject(HOLLOW_BRUSH);
    SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG_PTR)hbrBackground);

    HANDLE hIconSmall = LoadImage(CategoryDialog.hInst(), MAKEINTRESOURCE(IDI_MAINICON), IMAGE_ICON, 16, 16, LR_SHARED);
    SendMessage(hwnd, WM_SETICON, (WPARAM)ICON_SMALL, (LPARAM)hIconSmall);


    AfxCenterWindow(hwnd, HWND_TRADEDIALOG);

    EnableWindow(HWND_TRADEDIALOG, FALSE);

    // Fix Windows 10 white flashing
    BOOL cloak = TRUE;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &cloak, sizeof(cloak));

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    cloak = FALSE;
    DwmSetWindowAttribute(hwnd, DWMWA_CLOAK, &cloak, sizeof(cloak));

    SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    DialogReturnCode = DIALOG_RETURN_CANCEL;

    // Call modal message pump and wait for it to end.
    MSG msg{};
    while (GetMessage(&msg, NULL, 0, 0))
    {
        if (msg.message == WM_KEYUP && msg.wParam == VK_ESCAPE) {
            SendMessage(hwnd, WM_CLOSE, 0, 0);
        }

        // We handle VK_TAB processing ourselves rather than using IsDialogMessage
        // Translates virtual-key messages into character messages.
        TranslateMessage(&msg);
        // Dispatches a message to a window procedure.
        DispatchMessage(&msg);
    }

    return DialogReturnCode;
}

