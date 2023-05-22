#pragma once

#include "..\Themes\Themes.h"
#include "..\Database\trade.h"


// ListBox data structure that will be directly accessed for each row 
// during the WM_DRAWITEM notification. 

typedef long TickerId;

class ColumnData {
public:
    std::wstring        wszText;
    StringAlignment     HAlignment = StringAlignmentNear;  
    StringAlignment     VAlignment = StringAlignmentCenter;
    ThemeElement        backTheme = ThemeElement::GrayDark;
    ThemeElement        textTheme = ThemeElement::WhiteLight;;
    REAL                fontSize = 8;                    // 8, 10
    int                 fontStyle = FontStyleRegular;    // FontStyleRegular, FontStyleBold
    int                 colWidth = 0;
};

enum class LineType {
    None,
    TickerLine,
    OptionsLeg,
    Shares,
    Futures
};

class ListBoxData {
public:
    LineType        lineType = LineType::None;
    bool            isDailyTotalsNode = false;
    bool            isDailyTotalsNodeOpen = false;
    std::wstring    DailyTotalsDate;
    std::wstring    AggregateShares;
    TickerId        tickerId = -1;
    std::shared_ptr<Trade> trade;
    std::shared_ptr<Leg> leg;
    ColumnData      col[10];


    void SetData(
        int index, std::shared_ptr<Trade> tradeptr, TickerId tickId,
        std::wstring wszText, StringAlignment HAlignment, StringAlignment VAlignment, 
        ThemeElement backTheme, ThemeElement textTheme, REAL fontSize, int fontStyle)
    {
        if (tickId != -1) lineType = LineType::TickerLine;
        tickerId = tickId;
        trade = tradeptr;
        col[index].wszText = wszText;
        col[index].HAlignment = HAlignment;
        col[index].VAlignment = VAlignment;
        col[index].backTheme = backTheme;
        col[index].textTheme = textTheme;
        col[index].fontSize = fontSize;
        col[index].fontStyle = fontStyle;
    }

    // Update Text & color only. This is called from tws-client when TWS
    // sends new price data that needs to be updated.
    void SetTextData(int index, std::wstring wszText, ThemeElement textTheme)
    {
        col[index].wszText = wszText;
        col[index].textTheme = textTheme;
    }

};


enum class TableType
{
    ActiveTrades,
    ClosedTrades,
    TradeHistory,
    TickerTotals,
    DailyTotals,
    DailyTotalsSummary,
    TradeTemplates,
    TradeManagement
};

void ListBoxData_ResizeColumnWidths(HWND hListBox, TableType tabletype, int nIndex);
void ListBoxData_DestroyItemData(HWND hListBox);
void ListBoxData_RequestMarketData(HWND hListBox);
void ListBoxData_OpenPosition(HWND hListBox, const std::shared_ptr<Trade>& trade, TickerId tickerId);
void ListBoxData_HistoryHeader(
    HWND hListBox, const std::shared_ptr<Trade>& trade, const std::shared_ptr<Transaction>& trans);
void ListBoxData_HistoryOptionsLeg(
    HWND hListBox, const std::shared_ptr<Trade>& trade, const std::shared_ptr<Transaction>& trans, const std::shared_ptr<Leg>& leg);
void ListBoxData_HistorySharesLeg(
    HWND hListBox, const std::shared_ptr<Trade>& trade, const std::shared_ptr<Transaction>& trans, const std::shared_ptr<Leg>& leg);
void ListBoxData_HistoryBlankLine(HWND hListBox);
void ListBoxData_OutputClosedPosition(HWND hListBox, const std::shared_ptr<Trade>& trade, std::wstring closedDate);
void ListBoxData_OutputTickerTotals(HWND hListBox, std::wstring ticker, double amount);
void ListBoxData_OutputDailyTotalsNodeHeader(HWND hListBox, std::wstring date, double amount, bool isOpen);
void ListBoxData_OutputDailyTotalsDetailLine(HWND hListBox, const std::shared_ptr<Trade>& trade, const std::shared_ptr<Transaction>& trans);
void ListBoxData_OutputDailyTotalsSummary(HWND hListBox, double grandTotal, double MTD, double YTD);
void ListBoxData_OnDrawItem(HWND hwnd, const DRAWITEMSTRUCT* lpDrawItem);
void Header_OnPaint(HWND hWnd);

