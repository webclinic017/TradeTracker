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

#pragma once

#include "..\Utilities\CWindowBase.h"


class CTradesPanel : public CWindowBase<CTradesPanel>
{
public:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

};


const int IDC_TRADES_LISTBOX = 100;
const int IDC_TRADES_LABEL = 101;
const int IDC_TRADES_CustomVScrollBar = 102;
const int IDC_TRADES_HEADER = 103;

const int ACTIVE_TRADES_LISTBOX_ROWHEIGHT = 24;
const int CLOSED_TRADES_LISTBOX_ROWHEIGHT = 18;
const int TRADESPANEL_MARGIN = 24;


// These columns in the table are updated in real time when connected
// to TWS. The LineData pointer is updated via a call to SetColumnData
// and the correct ListBox line is invalidated/redrawn in order to force
// display of the new price data. Refer to TwsClient::tickPrice in the
// tws-client.cpp file to see this in action.
const int COLUMN_TICKER_ITM          = 2;    // ITM (In the Money)
const int COLUMN_TICKER_CHANGE       = 5;    // price change
const int COLUMN_TICKER_CURRENTPRICE = 6;    // current price
const int COLUMN_TICKER_PERCENTAGE   = 7;    // price percentage change


// Various actions that can be performed on selected Trade or Legs.
// TODO: Need to add logic to handle share assignments.
// TODO: Add default multipliers for known futures contracts.
enum class TradeAction
{
    NewOptionsTrade,
    NewSharesTrade,
    NewFuturesTrade,
    ManageShares,
    ManageFutures,
    RollLeg,
    CloseLeg,
    ExpireLeg,
    Assignment,
    AddOptionsToTrade,
    AddSharesToTrade,
    AddFuturesToTrade,
    AddPutToTrade,
    AddCallToTrade,
    NewIronCondor,
    NewShortStrangle,
    NewShortPut,
    NewShortCall,
    NoAction
};

bool IsNewOptionsTradeAction(TradeAction action);
bool IsNewSharesTradeAction(TradeAction action);

