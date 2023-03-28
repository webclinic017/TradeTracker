#pragma once

#include "pch.h"
#include "CWindow.h"
#include "Themes.h"
#include "trade.h"

typedef long TickerId;

const int IDC_TRADESPANEL = 101;

const int IDC_LISTBOX = 102;
const int IDC_LABEL = 103;

// Construct the Trades ListBox data structure (Vector) that will be directly accessed
// for each row during the WM_DRAWITEM notification. The message will specify which ListBox
// line it is attempting to draw. We simply get that line information from the following
// display structure vector. Likewise, when new price data is received from TWS, it will also
// directly reference a specific ListBox line because we would have set the requested tickerId
// to be the same as the line number of the ticker in the ListBox. Need to just invalidate
// that ListBox line in order to display the updated price information.

class ColumnData {
public:
    std::wstring        wszText;
    StringAlignment     alignment = StringAlignmentNear;  // StringAlignmentNear, StringAlignmentCenter, StringAlignmentFar
    ThemeElement        backTheme = ThemeElement::TradesPanelBack;
    ThemeElement        textTheme = ThemeElement::TradesPanelText;;
    REAL                fontSize = 8;                    // 8, 10
    int                 fontStyle = FontStyleRegular;    // FontStyleRegular, FontStyleBold
};

class LineData {
public:
    bool            isTickerLine = false;
    Trade*          trade = nullptr;
    ColumnData      col[8];
    TickerId        tickerId = 0;
};

const int TICKER_NUMBER_OFFEST = 100;

CWindow* TradesPanel_Show(HWND hWndParent);
void ShowActiveTrades();

