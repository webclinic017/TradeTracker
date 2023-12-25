/*

MIT License

Copyright(c) 2023-2024 Paul Squires

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

#pragma once

#include "Utilities/CWindowBase.h"


class CTransPanel : public CWindowBase<CTransPanel> {
public:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    HWND hWindow = NULL;

    HWND TradesListBox();
    HWND TradesHeader();
    HWND VScrollBar();
    HWND TickerFilterLabel();
    HWND TickerTextBox();
    HWND TickerGoButton();
    HWND DateFilterLabel();
    HWND TransDateCombo();
    HWND TransDateButton();
    HWND StartDateLabel();
    HWND StartDateCombo();
    HWND StartDateButton();
    HWND EndDateLabel();
    HWND EndDateCombo();
    HWND EndDateButton();

    void ShowTransactions();

private:
    bool OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
    void OnSize(HWND hwnd, UINT state, int cx, int cy);
    void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
    void OnPaint(HWND hwnd);
    bool OnEraseBkgnd(HWND hwnd, HDC hdc);
    void OnMeasureItem(HWND hwnd, MEASUREITEMSTRUCT* lpMeasureItem);

    void SetStartEndDates(HWND hwnd);
    void ShowListBoxItem(int index);

    static LRESULT CALLBACK ListBox_SubclassProc(
        HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
        UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK Header_SubclassProc(
        HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
        UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};


constexpr int IDC_TRANS_LISTBOX = 100;
constexpr int IDC_TRANS_CUSTOMVSCROLLBAR = 102;
constexpr int IDC_TRANS_HEADER = 103;

constexpr int IDC_TRANS_LBLTICKERFILTER = 110;
constexpr int IDC_TRANS_TXTTICKER = 111;
constexpr int IDC_TRANS_CMDTICKERGO = 112;

constexpr int IDC_TRANS_LBLDATEFILTER = 115;
constexpr int IDC_TRANS_TRANSDATE = 116;
constexpr int IDC_TRANS_CMDTRANSDATE = 117;

constexpr int IDC_TRANS_LBLSTARTDATE = 120;
constexpr int IDC_TRANS_STARTDATE = 121;
constexpr int IDC_TRANS_CMDSTARTDATE = 122;

constexpr int IDC_TRANS_LBLENDDATE = 125;
constexpr int IDC_TRANS_ENDDATE = 126;
constexpr int IDC_TRANS_CMDENDDATE = 127;


constexpr int TRANSPANEL_LISTBOX_ROWHEIGHT = 18;
constexpr int TRANSPANEL_MARGIN = 80;

extern CTransPanel TransPanel;
