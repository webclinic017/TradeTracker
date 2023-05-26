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
#include "..\Utilities\CWindowBase.h"
#include "..\CustomLabel\CustomLabel.h"
#include "..\TradeGrid\TradeGrid.h"
#include "..\TradeDialog\TradeDialog.h"

#include "StrategyButton.h"
#include "StrategyPopup.h"


HWND HWND_STRATEGYBUTTON = NULL;

extern HWND HWND_TRADEDIALOG;

extern CStrategyButton StrategyButton;

CStrategyPopup StrategyPopup;
extern HWND StrategyPopup_CreatePopup(HWND hParent, HWND hParentCtl);



// ========================================================================================
// Get the text for the specified LongShort
// ========================================================================================
std::wstring StrategyButton_GetLongShortEnumText(LongShort ls)
{
    switch (ls)
    {
    case LongShort::Long:
        return L"Long";
    case LongShort::Short:
        return L"Short";
    default:
        return L"";
    }
}

// ========================================================================================
// Get the text for the specified PutCall
// ========================================================================================
std::wstring StrategyButton_GetPutCallEnumText(PutCall pc)
{
    switch (pc)
    {
    case PutCall::Put:
        return L"Put";
    case PutCall::Call:
        return L"Call";
    default:
        return L"";
    }
}

// ========================================================================================
// Get the text for the specified Strategy
// ========================================================================================
std::wstring StrategyButton_GetStrategyEnumText(Strategy s)
{
    switch (s)
    {
    case Strategy::Vertical:
        return L"Vertical";
    case Strategy::Strangle:
        return L"Strangle";
    case Strategy::Straddle:
        return L"Straddle";
    case Strategy::Option:
        return L"Option";
    case Strategy::IronCondor:
        return L"Iron Condor";
    case Strategy::Covered:
        return L"Covered";
    case Strategy::Butterfly:
        return L"Butterfly";
    case Strategy::RatioSpread:
        return L"Ratio Spread";
    default:
        return L"";
    }
}


// ========================================================================================
// Process WM_ERASEBKGND message for window/dialog: StrategyButton
// ========================================================================================
BOOL StrategyButton_OnEraseBkgnd(HWND hwnd, HDC hdc)
{
    // Handle all of the painting in WM_PAINT
    return TRUE;
}


// ========================================================================================
// Process WM_PAINT message for window/dialog: StrategyButton
// ========================================================================================
void StrategyButton_OnPaint(HWND hwnd)
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
// Process WM_CREATE message for window/dialog: StrategyButton
// ========================================================================================
BOOL StrategyButton_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
    HWND_STRATEGYBUTTON = hwnd;

    int nHeight = 23;

    HWND hCtl = NULL;
    std::wstring wszFontName = L"Segoe UI";
    std::wstring wszText;
    int FontSize = 8;
    bool bold = false;

    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_STRATEGYBUTTON_LONGSHORT, L"",
        ThemeElement::WhiteLight, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleLeft, 0, 0, 50, nHeight);
    CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, bold);
    CustomLabel_SetTextOffset(hCtl, 5, 0);
    CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
    StrategyButton_SetLongShortBackColor(hCtl);
    wszText = AfxUpper(StrategyButton_GetLongShortEnumText(LongShort::Short));
    CustomLabel_SetText(hCtl, wszText);

    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_STRATEGYBUTTON_PUTCALL, L"",
        ThemeElement::WhiteLight, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleCenter, 51, 0, 50, nHeight);
    CustomLabel_SetUserDataInt(hCtl, (int)PutCall::Put);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, bold);
    CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
    wszText = AfxUpper(StrategyButton_GetPutCallEnumText(PutCall::Put));
    CustomLabel_SetText(hCtl, wszText);

    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_STRATEGYBUTTON_STRATEGY, L"",
        ThemeElement::WhiteLight, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleLeft, 102, 0, 100, nHeight);
    CustomLabel_SetUserDataInt(hCtl, (int)Strategy::Vertical);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, bold);
    CustomLabel_SetTextOffset(hCtl, 5, 0);
    CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
    wszText = AfxUpper(StrategyButton_GetStrategyEnumText(Strategy::Vertical));
    CustomLabel_SetText(hCtl, wszText);

    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_STRATEGYBUTTON_GO, L"GO",
        ThemeElement::Black, ThemeElement::Blue, ThemeElement::Blue, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleCenter, 203, 0, 30, nHeight);
    CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
    CustomLabel_SetTextColorHot(hCtl, ThemeElement::WhiteLight);

    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_STRATEGYBUTTON_DROPDOWN, L"\uE015",
        ThemeElement::WhiteDark, ThemeElement::GrayMedium, ThemeElement::GrayLight, ThemeElement::GrayMedium,
        CustomLabelAlignment::MiddleCenter, 234, 0, 30, nHeight);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
    CustomLabel_SetTextColorHot(hCtl, ThemeElement::WhiteLight);

    return TRUE;
}


// ========================================================================================
// Set the Short/Long Text color.
// ========================================================================================
void StrategyButton_SetLongShortTextColor(HWND hCtl)
{
    LongShort ls = (LongShort)CustomLabel_GetUserDataInt(hCtl);

    if (ls == LongShort::Long) {
        CustomLabel_SetTextColor(hCtl, ThemeElement::Green);
    }
    else if (ls == LongShort::Short) {
        CustomLabel_SetTextColor(hCtl, ThemeElement::Red);
    }
}

// ========================================================================================
// Set the Short/Long background color.
// ========================================================================================
void StrategyButton_SetLongShortBackColor(HWND hCtl)
{
    LongShort ls = (LongShort)CustomLabel_GetUserDataInt(hCtl);

    if (ls == LongShort::Long) {
        CustomLabel_SetBackColor(hCtl, ThemeElement::Green);
    }
    else if (ls == LongShort::Short) {
        CustomLabel_SetBackColor(hCtl, ThemeElement::Red);
    }
}


// ========================================================================================
// Toggle the Short/Long text.
// ========================================================================================
void StrategyButton_ToggleLongShortText(HWND hCtl)
{
    int sel = CustomLabel_GetUserDataInt(hCtl) + 1;
    if (sel == (int)LongShort::Count) sel = 0;

    CustomLabel_SetUserDataInt(hCtl, sel);
    std::wstring wszText = AfxUpper(StrategyButton_GetLongShortEnumText((LongShort)sel));
    CustomLabel_SetText(hCtl, wszText);
}

// ========================================================================================
// Check to see if the currently selected Strategy allows PutCall
// ========================================================================================
bool StrategyButton_StrategyAllowPutCall(HWND hCtl)
{
    Strategy s = (Strategy)CustomLabel_GetUserDataInt(hCtl);

    switch (s)
    {
    case Strategy::Vertical:
    case Strategy::Option:
    case Strategy::Covered:
    case Strategy::Butterfly:
    case Strategy::RatioSpread:
        return true;
    case Strategy::Straddle:
    case Strategy::Strangle:
    case Strategy::IronCondor:
        return false;
    default:
        return true;
    }
}

// ========================================================================================
// Toggle the Put/Call text.
// ========================================================================================
void StrategyButton_TogglePutCallText(HWND hCtlPutCall, HWND hCtlStrategy)
{
    if (!StrategyButton_StrategyAllowPutCall(hCtlStrategy)) {
        CustomLabel_SetText(hCtlPutCall, L"");
        return;
    }

    int sel = CustomLabel_GetUserDataInt(hCtlPutCall) + 1;
    if (sel == (int)PutCall::Count) sel = 0;

    CustomLabel_SetUserDataInt(hCtlPutCall, sel);
    std::wstring wszText = AfxUpper(StrategyButton_GetPutCallEnumText((PutCall)sel));
    CustomLabel_SetText(hCtlPutCall, wszText);
}

// ========================================================================================
// Toggle through the different Strategies.
// ========================================================================================
void StrategyButton_ToggleStrategyText(HWND hCtl)
{
    int sel = CustomLabel_GetUserDataInt(hCtl) + 1;
    if (sel == (int)Strategy::Count) sel = 0;

    CustomLabel_SetUserDataInt(hCtl, sel);
    std::wstring wszText = AfxUpper(StrategyButton_GetStrategyEnumText((Strategy)sel));
    CustomLabel_SetText(hCtl, wszText);

    wszText = L"";
    HWND hCtlPutCall = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_PUTCALL);
    if (StrategyButton_StrategyAllowPutCall(hCtl)) {
        sel = CustomLabel_GetUserDataInt(hCtlPutCall);
        wszText = AfxUpper(StrategyButton_GetPutCallEnumText((PutCall)sel));
    }
    CustomLabel_SetText(hCtlPutCall, wszText);
}


// ========================================================================================
// Invoke the Strategy as set in the StrategyButton.
// ========================================================================================
void StrategyButton_InvokeStrategy()
{
    Strategy s = (Strategy)CustomLabel_GetUserDataInt(GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY));
    LongShort ls = (LongShort)CustomLabel_GetUserDataInt(GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_LONGSHORT));
    PutCall pc = (PutCall)CustomLabel_GetUserDataInt(GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_PUTCALL));

    HWND hTradeGrid = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TABLEGRIDMAIN);

    TradeGrid* pData = TradeGrid_GetOptions(hTradeGrid);
    if (pData == nullptr) return;

    int colStart = 0;
    int row = 0;
    int col = 0;

    // Clear/Reset the Trade grid (4 rows)
    for (int i = 0; i < 4; ++i) {
        colStart = i * 7;
        CustomTextBox_SetText(pData->gridCols.at(colStart)->hCtl, L"");     // Quantity
        CustomLabel_SetUserData(pData->gridCols.at(colStart + 1)->hCtl, L"");     // Expiry Date
        CustomLabel_SetText(pData->gridCols.at(colStart + 1)->hCtl, L"");     // Expiry Date
        CustomLabel_SetText(pData->gridCols.at(colStart + 2)->hCtl, L"");   // DTE
        CustomTextBox_SetText(pData->gridCols.at(colStart + 3)->hCtl, L"");   // Strike Price
        CustomLabel_SetText(pData->gridCols.at(colStart + 4)->hCtl, L"");     // Put/Call
        CustomLabel_SetText(pData->gridCols.at(colStart + 5)->hCtl, L"");     // Action
    }


    // Add the new strategy to the grid
    HWND hCtlDescription = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TXTDESCRIBE);
    std::wstring wszDate = AfxCurrentDate();

    HWND hGrid = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TABLEGRIDMAIN);

    switch (s)
    {
    case Strategy::Vertical:
    {
        std::wstring wszPutCall = (pc == PutCall::Put) ? L"P" : L"C";
        if (ls == LongShort::Long) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");  
            TradeGrid_SetColData(hGrid, row, col+1, wszDate);
            TradeGrid_SetColData(hGrid, row, col+4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col+5, L"BTO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        if (ls == LongShort::Short) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        }
        TradeGrid_CalculateDTE(pData->hWindow);
        wszPutCall = (pc == PutCall::Put) ? L"Put " : L"Call ";
        AfxSetWindowText(hCtlDescription, wszPutCall + StrategyButton_GetStrategyEnumText(s));
    }
    break;

    case Strategy::Strangle:
    case Strategy::Straddle:
    {
        if (ls == LongShort::Long) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"P");
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"C");
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        if (ls == LongShort::Short) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"P");
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"C");
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        }
        TradeGrid_CalculateDTE(pData->hWindow);
        AfxSetWindowText(hCtlDescription, StrategyButton_GetStrategyEnumText(s));
    }
    break;


    case Strategy::Option:
    case Strategy::Covered:
    {
        std::wstring wszPutCall = (pc == PutCall::Put) ? L"P" : L"C";
        if (ls == LongShort::Long) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        if (ls == LongShort::Short) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        }
        TradeGrid_CalculateDTE(pData->hWindow);
        if (s == Strategy::Covered) {
            wszPutCall = (pc == PutCall::Put) ? L" Put" : L" Call";
            AfxSetWindowText(hCtlDescription, StrategyButton_GetStrategyEnumText(s) + wszPutCall);
        } else {
            wszPutCall = (pc == PutCall::Put) ? L"Put " : L"Call ";
            AfxSetWindowText(hCtlDescription, wszPutCall + StrategyButton_GetStrategyEnumText(s));
        }
    }
    break;


    case Strategy::RatioSpread:
    {
        std::wstring wszPutCall = (pc == PutCall::Put) ? L"P" : L"C";
        if (ls == LongShort::Long) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"2");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        if (ls == LongShort::Short) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-2");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        }
        TradeGrid_CalculateDTE(pData->hWindow);
        wszPutCall = (pc == PutCall::Put) ? L"Put " : L"Call ";
        AfxSetWindowText(hCtlDescription, wszPutCall + StrategyButton_GetStrategyEnumText(s));
    }
    break;


    case Strategy::IronCondor:
    {
        if (ls == LongShort::Long) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"P");
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"P");
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 2; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"C");
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 3; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"C");
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        if (ls == LongShort::Short) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"P");
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"P");
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 2; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"C");
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 3; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, L"C");
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        }
        TradeGrid_CalculateDTE(pData->hWindow);
        AfxSetWindowText(hCtlDescription, StrategyButton_GetStrategyEnumText(s));
    }
    break;


    case Strategy::Butterfly:
    {
        std::wstring wszPutCall = (pc == PutCall::Put) ? L"P" : L"C";
        if (ls == LongShort::Long) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-2");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 2; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        if (ls == LongShort::Short) {
            row = 0; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            row = 1; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"2");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"BTO");
            row = 2; col = 0;
            TradeGrid_SetColData(hGrid, row, col, L"-1");
            TradeGrid_SetColData(hGrid, row, col + 1, wszDate);
            TradeGrid_SetColData(hGrid, row, col + 4, wszPutCall);
            TradeGrid_SetColData(hGrid, row, col + 5, L"STO");
            TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        }
        TradeGrid_CalculateDTE(pData->hWindow);
        AfxSetWindowText(hCtlDescription, StrategyButton_GetStrategyEnumText(s));
    }
    break;

    }

}


// ========================================================================================
// Windows callback function.
// ========================================================================================
LRESULT CStrategyButton::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        HANDLE_MSG(m_hwnd, WM_CREATE, StrategyButton_OnCreate);
        HANDLE_MSG(m_hwnd, WM_ERASEBKGND, StrategyButton_OnEraseBkgnd);
        HANDLE_MSG(m_hwnd, WM_PAINT, StrategyButton_OnPaint);


    case MSG_CUSTOMLABEL_CLICK:
    {
        HWND hCtl = (HWND)lParam;
        int CtrlId = (int)wParam;

        if (hCtl == NULL) return 0;

        if (CtrlId == IDC_STRATEGYBUTTON_LONGSHORT) {
            StrategyButton_ToggleLongShortText(hCtl);
            StrategyButton_SetLongShortBackColor(hCtl);
        }
        if (CtrlId == IDC_STRATEGYBUTTON_PUTCALL) {
            StrategyButton_TogglePutCallText(hCtl, GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY));
        }
        if (CtrlId == IDC_STRATEGYBUTTON_STRATEGY) {
            StrategyButton_ToggleStrategyText(hCtl);
        }
        if (CtrlId == IDC_STRATEGYBUTTON_DROPDOWN) {
            StrategyPopup_CreatePopup(GetParent(m_hwnd), m_hwnd);
        }
        if (CtrlId == IDC_STRATEGYBUTTON_GO) {
            StrategyButton_InvokeStrategy();
        }
        return 0;
    }
    break;

    default: return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}

