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

#include "Config/Config.h"
#include "CustomLabel/CustomLabel.h"
#include "Utilities/ListBoxData.h"
#include "CategoryPopup.h"


HWND HWND_CATEGORYPOPUP = NULL;

CCategoryPopup CategoryPopup;

// Control on parent window that new selected category will be stored in and displayed.
HWND hCategoryUpdateParentCtl = NULL;
Category SelectedCategory = Category::Category0;



// ========================================================================================
// Handle selecting an item in the listview. This will set the parent label window, update
// its CustomDataInt, and set its text label. Finally, it will close the popup dialog.
// ========================================================================================
void CategoryPopup_DoSelected(int idx)
{
    CustomLabel_SetUserDataInt(hCategoryUpdateParentCtl, idx);
    CustomLabel_SetText(hCategoryUpdateParentCtl, GetCategoryDescription(idx));
    DestroyWindow(HWND_CATEGORYPOPUP);
}


// ========================================================================================
// Listbox subclass Window procedure
// ========================================================================================
LRESULT CALLBACK CategoryPopup_ListBox_SubclassProc(
    HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
    UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
    switch (uMsg)
    {

    case WM_MOUSEMOVE:
    {
        // Tracks the mouse movement and stores the hot state
        if (GetProp(hWnd, L"HOT") == 0) {
            TRACKMOUSEEVENT trackMouse;
            trackMouse.cbSize = sizeof(trackMouse);
            trackMouse.dwFlags = TME_LEAVE;
            trackMouse.hwndTrack = hWnd;
            trackMouse.dwHoverTime = 1;
            TrackMouseEvent(&trackMouse);
            SetProp(hWnd, L"HOT", (HANDLE)TRUE);
        }
        AfxRedrawWindow(hWnd);
        return 0;
    }
    break;


    case WM_MOUSELEAVE:
    {
        RemoveProp(hWnd, L"HOT");
        AfxRedrawWindow(hWnd);
        return 0;
    }
    break;


    case WM_LBUTTONDOWN:
    {
        int idx = Listbox_ItemFromPoint(hWnd, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
        // The return value contains the index of the nearest item in the LOWORD. The HIWORD is zero 
        // if the specified point is in the client area of the list box, or one if it is outside the 
        // client area.
        if (HIWORD(idx) == 1) break;

        CategoryPopup_DoSelected(idx);
        return 0;
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
            SolidBrush backBrush(COLOR_GRAYDARK);
            graphics.FillRectangle(&backBrush, rc.left, rc.top, nWidth, nHeight);
        }

        ValidateRect(hWnd, &rc);
        return TRUE;
        break;

    }


    case WM_DESTROY:
        // REQUIRED: Remove control subclassing
        RemoveWindowSubclass(hWnd, CategoryPopup_ListBox_SubclassProc, uIdSubclass);
        break;


    }   // end of switch statment

    // For messages that we don't deal with
    return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


// ========================================================================================
// Process WM_SIZE message for window/dialog: CategoryPopup
// ========================================================================================
void CategoryPopup_OnSize(HWND hwnd, UINT state, int cx, int cy)
{
    int margin = AfxScaleX(1);
    SetWindowPos(GetDlgItem(hwnd, IDC_CATEGORYPOPUP_LISTBOX), 0,
        margin, margin, cx - (margin * 2), cy - (margin * 2), SWP_NOZORDER | SWP_SHOWWINDOW);
    return;
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: CategoryPopup
// ========================================================================================
BOOL CategoryPopup_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: CategoryPopup
// ========================================================================================
void CategoryPopup_OnPaint(HWND hwnd)
{
    PAINTSTRUCT ps;

    HDC hdc = BeginPaint(hwnd, &ps);

    Graphics graphics(hdc);

    // Create the background brush
    SolidBrush backBrush(COLOR_BLACK);

    // Paint the background using brush.
    int nWidth = (ps.rcPaint.right - ps.rcPaint.left);
    int nHeight = (ps.rcPaint.bottom - ps.rcPaint.top);
    graphics.FillRectangle(&backBrush, ps.rcPaint.left, ps.rcPaint.top, nWidth, nHeight);

    EndPaint(hwnd, &ps);
}


// ========================================================================================
// Process WM_DRAWITEM message for window/dialog: CategoryPopup
// ========================================================================================
void CategoryPopup_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT* lpDrawItem)
{
    if (lpDrawItem->itemID == -1) return;

    if (lpDrawItem->itemAction == ODA_DRAWENTIRE ||
        lpDrawItem->itemAction == ODA_SELECT) {

        int nWidth = (lpDrawItem->rcItem.right - lpDrawItem->rcItem.left);
        int nHeight = (lpDrawItem->rcItem.bottom - lpDrawItem->rcItem.top);

        bool bIsHot = false;

        SaveDC(lpDrawItem->hDC);

        HDC memDC = NULL;         // Double buffering
        HBITMAP hbit = NULL;      // Double buffering

        memDC = CreateCompatibleDC(lpDrawItem->hDC);
        hbit = CreateCompatibleBitmap(lpDrawItem->hDC, nWidth, nHeight);
        if (hbit) SelectObject(memDC, hbit);

        if ((lpDrawItem->itemAction | ODA_SELECT) &&
            (lpDrawItem->itemState & ODS_SELECTED)) {
            bIsHot = true;
        }

        // Determine if we are in a Hot mouseover state
        POINT pt; GetCursorPos(&pt);
        MapWindowPoints(HWND_DESKTOP, lpDrawItem->hwndItem, &pt, 1);
        if (PtInRect(&lpDrawItem->rcItem, pt)) bIsHot = true;

        Graphics graphics(memDC);
        graphics.SetTextRenderingHint(TextRenderingHintClearTypeGridFit);

        DWORD nBackColor = (bIsHot) ? COLOR_SELECTION : COLOR_GRAYMEDIUM;
        DWORD nBackColorHot = COLOR_SELECTION;
        DWORD nTextColor = (bIsHot) ? COLOR_WHITELIGHT : COLOR_WHITEDARK;

        std::wstring wszFontName = AfxGetDefaultFont();
        FontFamily   fontFamily(wszFontName.c_str());
        REAL fontSize = 9;
        int fontStyle = FontStyleRegular;

        // Paint the full width background using brush 
        SolidBrush backBrush(nBackColor);
        graphics.FillRectangle(&backBrush, 0, 0, nWidth, nHeight);

        Font         font(&fontFamily, fontSize, fontStyle, Unit::UnitPoint);
        SolidBrush   textBrush(nTextColor);
        StringFormat stringF(StringFormatFlagsNoWrap);
        stringF.SetLineAlignment(StringAlignment::StringAlignmentCenter);

        std::wstring wszText;

        if ((int)SelectedCategory == lpDrawItem->itemID) wszText = GLYPH_CHECKMARK;
        RectF rcText1((REAL)0, (REAL)0, (REAL)AfxScaleX(24), (REAL)nHeight);
        stringF.SetAlignment(StringAlignment::StringAlignmentCenter);
        graphics.DrawString(wszText.c_str(), -1, &font, rcText1, &stringF, &textBrush);

        wszText = AfxGetListBoxText(lpDrawItem->hwndItem, lpDrawItem->itemID);
        RectF rcText2((REAL)AfxScaleX(24), (REAL)0, (REAL)nWidth, (REAL)nHeight);
        stringF.SetAlignment(StringAlignment::StringAlignmentNear);
        graphics.DrawString(wszText.c_str(), -1, &font, rcText2, &stringF, &textBrush);

        BitBlt(lpDrawItem->hDC, lpDrawItem->rcItem.left,
            lpDrawItem->rcItem.top, nWidth, nHeight, memDC, 0, 0, SRCCOPY);


        // Cleanup
        RestoreDC(lpDrawItem->hDC, -1);
        if (hbit) DeleteObject(hbit);
        if (memDC) DeleteDC(memDC);
    }
}


// ========================================================================================
// Process WM_MEASUREITEM message for window/dialog: CategoryPopup
// ========================================================================================
void CategoryPopup_OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT* lpMeasureItem)
{
    lpMeasureItem->itemHeight = AfxScaleY(CATEGORYPOPUP_LISTBOX_ROWHEIGHT);
}


// ========================================================================================
// Process WM_CREATE message for window/dialog: CategoryPopup
// ========================================================================================
BOOL CategoryPopup_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_CATEGORYPOPUP = hwnd;

    HWND hCtl =
        CategoryPopup.AddControl(Controls::ListBox, hwnd, IDC_CATEGORYPOPUP_LISTBOX, L"",
            0, 0, 0, 0,
            WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP |
            LBS_NOINTEGRALHEIGHT | LBS_OWNERDRAWFIXED | LBS_NOTIFY | LBS_HASSTRINGS,
            WS_EX_LEFT | WS_EX_RIGHTSCROLLBAR, NULL,
            (SUBCLASSPROC)CategoryPopup_ListBox_SubclassProc,
            IDC_CATEGORYPOPUP_LISTBOX, NULL);

    for (int i = (int)Category::Category_Start; i <= (int)Category::Category_End; ++i) {
        int idx = ListBox_AddString(hCtl, GetCategoryDescription(i).c_str());
        ListBox_SetItemData(hCtl, idx, i);
    }

    return TRUE;
}


// ========================================================================================
// Windows callback function.
// ========================================================================================
LRESULT CCategoryPopup::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    // Prevent a recursive calling of the WM_NCACTIVATE message as DestroyWindow deactivates the window.
    static bool destroyed = false;

    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, CategoryPopup_OnCreate);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, CategoryPopup_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, CategoryPopup_OnPaint);
        HANDLE_MSG(m_hwnd, WM_SIZE, CategoryPopup_OnSize);
        HANDLE_MSG(m_hwnd, WM_MEASUREITEM, CategoryPopup_OnMeasureItem);
        HANDLE_MSG(m_hwnd, WM_DRAWITEM, CategoryPopup_OnDrawItem);


    case WM_DESTROY:
    {
        // Reset our destroyed variable for future use of the DatePicker
        destroyed = false;
        return 0;
    }
    break;


    case WM_NCACTIVATE:
    {
        // Detect that we have clicked outside the popup CategoryPopup and will now close it.
        if (wParam == false) {
            // Set our static flag to prevent recursion
            if (destroyed == false) {
                destroyed = true;
                DestroyWindow(m_hwnd);
            }
            return TRUE;
        }
        return 0;
    }
    break;


    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}


// ========================================================================================
// Create category popup control and move it into position under the 
// specified incoming control.
// ========================================================================================
HWND CategoryPopup_CreatePopup(HWND hParent, HWND hParentCtl)
{
    CategoryPopup.Create(hParent, L"", 0, 0, 0, 0,
        WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        WS_EX_LEFT | WS_EX_LTRREADING | WS_EX_RIGHTSCROLLBAR);

    int margin = AfxScaleX(1);
    RECT rc; GetWindowRect(hParentCtl, &rc);
    SetWindowPos(CategoryPopup.WindowHandle(), HWND_TOP,
        rc.left - margin, rc.bottom,
        AfxScaleX(CATEGORYPOPUP_WIDTH),
        AfxScaleY(CATEGORYPOPUP_LISTBOX_ROWHEIGHT * ((int)Category::Category_End + 1)),
        SWP_SHOWWINDOW);

    // Get the current selected category and apply it to the popup
    SelectedCategory = (Category)CustomLabel_GetUserDataInt(hParentCtl);


    // Set the module global hUpdateParentCtl after the above is created in
    // to ensure the variable address is correct.
    hCategoryUpdateParentCtl = hParentCtl;

    return CategoryPopup.WindowHandle();
}

