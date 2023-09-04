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

#include "TradeDialog.h"
#include "Utilities/AfxWin.h"
#include "CustomLabel/CustomLabel.h"
#include "CustomTextBox/CustomTextBox.h"
#include "TradeGrid/TradeGrid.h"
#include "Strategy/StrategyButton.h"
#include "ActiveTrades/ActiveTrades.h"
#include "MainWindow/tws-client.h"
#include "Database/database.h"
#include "Category/Category.h"
#include "Config/Config.h"



// ========================================================================================
// Set the Short/Long background color.
// ========================================================================================
void TradeDialog_SetLongShortBackColor(HWND hCtl)
{
    LongShort ls = (LongShort)CustomLabel_GetUserDataInt(hCtl);

    if (ls == LongShort::Long) {
        CustomLabel_SetBackColor(hCtl, COLOR_GREEN);
    }
    else if (ls == LongShort::Short) {
        CustomLabel_SetBackColor(hCtl, COLOR_RED);
    }
}


// ========================================================================================
// Toggle the BUY Short/Long Shares/Futures text.
// ========================================================================================
void TradeDialog_ToggleBuyLongShortText(HWND hCtl)
{
    int sel = CustomLabel_GetUserDataInt(hCtl) + 1;
    if (sel == (int)LongShort::Count) sel = 0;

    CustomLabel_SetUserDataInt(hCtl, sel);

    std::wstring wszText;

    switch ((LongShort)sel)
    {
    case LongShort::Long:
        wszText = L"BUY LONG ";
        TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        break;
    case LongShort::Short:
        wszText = L"SELL SHORT ";
        TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        break;
    default:
        wszText = L"";
    }

    if (tdd.tradeAction == TradeAction::NewSharesTrade) wszText += L"SHARES";
    if (tdd.tradeAction == TradeAction::AddSharesToTrade) wszText += L"SHARES";
    if (tdd.tradeAction == TradeAction::NewFuturesTrade) wszText += L"FUTURES";
    if (tdd.tradeAction == TradeAction::AddFuturesToTrade) wszText += L"FUTURES";
    CustomLabel_SetText(hCtl, wszText);
}


// ========================================================================================
// Toggle the SELL Short/Long Shares/Futures text.
// ========================================================================================
void TradeDialog_ToggleSellLongShortText(HWND hCtl)
{
    int sel = CustomLabel_GetUserDataInt(hCtl) + 1;
    if (sel == (int)LongShort::Count) sel = 0;

    CustomLabel_SetUserDataInt(hCtl, sel);

    std::wstring wszText;

    switch ((LongShort)sel)
    {
    case LongShort::Short:
        wszText = L"SELL LONG ";
        TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"CR");
        break;
    case LongShort::Long:
        wszText = L"BUY SHORT ";
        TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
        break;
    default:
        wszText = L"";
    }

    if (tdd.tradeAction == TradeAction::ManageShares) wszText += L"SHARES";
    if (tdd.tradeAction == TradeAction::ManageFutures) wszText += L"FUTURES";
    CustomLabel_SetText(hCtl, wszText);
}


// ========================================================================================
// Display or hide the Futures Contract data picker
// ========================================================================================
void TradeDialogControls_ShowFuturesContractDate(HWND hwnd)
{
    HWND hCtl1 = GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACT);
    HWND hCtl2 = GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE);
    HWND hCtl3 = GetDlgItem(hwnd, IDC_TRADEDIALOG_CMDCONTRACTDATE);

    // Futures Ticker symbols will start with a forward slash character.
    std::wstring wszTicker = AfxGetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTICKER));
    int nShow = (IsFuturesTicker(wszTicker)) ? SW_SHOW : SW_HIDE;

    ShowWindow(hCtl1, nShow);
    ShowWindow(hCtl2, nShow);
    ShowWindow(hCtl3, nShow);

    if (nShow == SW_SHOW) {
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCOMPANY), L"Futures Contract");
    } else {
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCOMPANY), L"Company Name");
    }
}


// ========================================================================================
// Helper function to calculate and update the Total TextBox
// ========================================================================================
void TradeDialog_CalculateTradeTotal(HWND hwnd)
{
    double total = 0;
    double quantity = stod(AfxGetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTQUANTITY)));
    double multiplier = stod(AfxGetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTMULTIPLIER)));
    double price = stod(AfxGetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTPRICE)));
    double fees = stod(AfxGetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTFEES)));

    DWORD TextColor = COLOR_RED;
    DWORD BackColor = COLOR_GRAYMEDIUM;

    std::wstring DRCR = CustomLabel_GetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_COMBODRCR));
    if (DRCR == L"CR") {
        fees = fees * -1;
        TextColor = COLOR_GREEN;
    }

    total = (quantity * multiplier * price) + fees;

    CustomTextBox_SetColors(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTOTAL), TextColor, BackColor);
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTOTAL), std::to_wstring(total));
}


// ========================================================================================
// Load the legs for the Transaction Edit Action into the Trade Management table(s)
// ========================================================================================
void TradeDialog_LoadEditTransactionInTradeTable(HWND hwnd)
{
    if (tdd.legs.size() == 0) return;

    HWND hGridMain = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TABLEGRIDMAIN);
    HWND hGridRoll = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TABLEGRIDROLL);
    HWND hGrid = hGridMain;

    // If more than 4 legs exist in the legs vector then we have to also use the rolled grid.
    int row = 0;
    for (const auto& leg : tdd.legs) {

        if (row > 3) {
            hGrid = hGridRoll;
            row = 0;
        }
        if (row > 7) break;

        // QUANTITY (ORIGINAL)
        std::wstring legoriginal_quantity = std::to_wstring(leg->original_quantity / max(tdd.trans->quantity,1));
        TradeGrid_SetColData(hGrid, row, 0, legoriginal_quantity);

        // QUANTITY (OPEN)
        std::wstring legopen_quantity = std::to_wstring(leg->open_quantity / max(tdd.trans->quantity,1));
        TradeGrid_SetColData(hGrid, row, 1, legopen_quantity);

        // EXPIRY DATE
        TradeGrid_SetColData(hGrid, row, 2, leg->expiry_date);

        // STRIKE PRICE
        TradeGrid_SetColData(hGrid, row, 4, leg->strike_price);

        // PUT/CALL
        TradeGrid_SetColData(hGrid, row, 5, leg->put_call);

        // ACTION
        TradeGrid_SetColData(hGrid, row, 6, leg->action);

        row++;
    }

    // TRANSACTION DATE
    CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLTRANSDATE), AfxLongDate(tdd.trans->trans_date));
    CustomLabel_SetUserData(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLTRANSDATE), tdd.trans->trans_date);

    // FUTURES CONTRACT DATE
    CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE), AfxLongDate(tdd.trade->future_expiry));
    CustomLabel_SetUserData(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE), tdd.trade->future_expiry);

    // DESCRIPTION
    AfxSetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTDESCRIBE), tdd.trans->description);

    // DTE
    TradeGrid_CalculateDTE(hGridMain);
    TradeGrid_CalculateDTE(hGridRoll);

    // CATEGORY
    CategoryControl_SetSelectedIndex(GetDlgItem(hwnd, IDC_TRADEDIALOG_CATEGORY), tdd.trade->category);

    // QUANTITY
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTQUANTITY), std::to_wstring(tdd.trans->quantity));
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTPRICE), std::to_wstring(tdd.trans->price));
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTMULTIPLIER), std::to_wstring(tdd.trans->multiplier));
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTFEES), std::to_wstring(tdd.trans->fees));
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTOTAL), std::to_wstring(tdd.trans->total));
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTRADEBP), std::to_wstring(tdd.trade->trade_bp));

    if (tdd.trans->total < 0) {
        TradeDialog_SetComboDRCR(GetDlgItem(hwnd, IDC_TRADEDIALOG_COMBODRCR), L"DR");
    }
    else {
        TradeDialog_SetComboDRCR(GetDlgItem(hwnd, IDC_TRADEDIALOG_COMBODRCR), L"CR");
    }
}


// ========================================================================================
// Load the legs for the edit Action into the Trade Management table
// ========================================================================================
void TradeDialog_LoadEditLegsInTradeTable(HWND hwnd)
{
    HWND hCtl = NULL;
    std::wstring wszText;

    if (tdd.tradeAction == TradeAction::NewOptionsTrade) return;
    if (tdd.tradeAction == TradeAction::NewSharesTrade) return;
    if (tdd.tradeAction == TradeAction::NewFuturesTrade) return;

    // Update the Trade Management table with the details of the Trade.
    if (tdd.trade != nullptr) {
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCOMPANY), tdd.trade->ticker_name);
        CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTCOMPANY), tdd.trade->ticker_name);  // hidden

        wszText = tdd.trade->ticker_symbol;
        if (tdd.trade->future_expiry.length()) {
            wszText = wszText + L": " + AfxFormatFuturesDate(tdd.trade->future_expiry);
            CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE), AfxLongDate(tdd.trade->future_expiry));
            CustomLabel_SetUserData(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE), tdd.trade->future_expiry);
        }
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLTICKER), wszText);
        CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTICKER), tdd.trade->ticker_symbol);  // hidden

        // Set the multiplier based on the incoming trade. Ensure that multiplier is always 100 for Option
        // transactions because it could be set to 1 if the Trade only contains existing Shares.
        double multiplier = tdd.trade->multiplier;
        if (IsNewOptionsTradeAction(tdd.tradeAction) ||
            tdd.tradeAction == TradeAction::AddCallToTrade ||
            tdd.tradeAction == TradeAction::AddPutToTrade ||
            tdd.tradeAction == TradeAction::AddOptionsToTrade) {
            if (IsFuturesTicker(tdd.trade->ticker_symbol) == false) multiplier = 100;
        }
        CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTMULTIPLIER), std::to_wstring(multiplier));
        CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTRADEBP), std::to_wstring(tdd.trade->trade_bp));
    }

    
    // Editing a previously created Transaction requires a separate function to
    // load the existing data into the table(s).
    if (tdd.tradeAction == TradeAction::EditTransaction) {
        TradeDialog_LoadEditTransactionInTradeTable(hwnd);
        return;
    }


    if (tdd.tradeAction == TradeAction::NewIronCondor) {
        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_LONGSHORT);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
        wszText = AfxUpper(StrategyButton_GetLongShortEnumText(LongShort::Short));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_put_call);
        CustomLabel_SetText(hCtl, L"");

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY);
        CustomLabel_SetUserDataInt(hCtl,(int)Strategy::IronCondor);
        wszText = AfxUpper(StrategyButton_GetStrategyEnumText(Strategy::IronCondor));
        CustomLabel_SetText(hCtl, wszText);

        StrategyButton_InvokeStrategy();
        return;
    }

    if (tdd.tradeAction == TradeAction::NewShortLT112) {
        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_LONGSHORT);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
        wszText = AfxUpper(StrategyButton_GetLongShortEnumText(LongShort::Short));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_put_call);
        CustomLabel_SetText(hCtl, L"");

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY);
        CustomLabel_SetUserDataInt(hCtl,(int)Strategy::LT112);
        wszText = AfxUpper(StrategyButton_GetStrategyEnumText(Strategy::LT112));
        CustomLabel_SetText(hCtl, wszText);

        StrategyButton_InvokeStrategy();
        return;
    }

    if (tdd.tradeAction == TradeAction::NewShortStrangle) {
        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_LONGSHORT);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
        wszText = AfxUpper(StrategyButton_GetLongShortEnumText(LongShort::Short));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_put_call);
        CustomLabel_SetText(hCtl, L"");

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY);
        CustomLabel_SetUserDataInt(hCtl,(int)Strategy::Strangle);
        wszText = AfxUpper(StrategyButton_GetStrategyEnumText(Strategy::Strangle));
        CustomLabel_SetText(hCtl, wszText);

        StrategyButton_InvokeStrategy();
        return;
    }

    if (tdd.tradeAction == TradeAction::NewShortPut ||
        tdd.tradeAction == TradeAction::AddPutToTrade) {
        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_LONGSHORT);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
        wszText = AfxUpper(StrategyButton_GetLongShortEnumText(LongShort::Short));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_put_call);
        CustomLabel_SetUserDataInt(hCtl, (int)put_call::Put);
        wszText = AfxUpper(StrategyButton_Getput_callEnumText(put_call::Put));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY);
        CustomLabel_SetUserDataInt(hCtl, (int)Strategy::Option);
        wszText = AfxUpper(StrategyButton_GetStrategyEnumText(Strategy::Option));
        CustomLabel_SetText(hCtl, wszText);

        StrategyButton_InvokeStrategy();
        return;
    }

    if (tdd.tradeAction == TradeAction::NewShortCall ||
        tdd.tradeAction == TradeAction::AddCallToTrade) {
        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_LONGSHORT);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
        wszText = AfxUpper(StrategyButton_GetLongShortEnumText(LongShort::Short));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_put_call);
        CustomLabel_SetUserDataInt(hCtl, (int)put_call::Call);
        wszText = AfxUpper(StrategyButton_Getput_callEnumText(put_call::Call));
        CustomLabel_SetText(hCtl, wszText);

        hCtl = GetDlgItem(HWND_STRATEGYBUTTON, IDC_STRATEGYBUTTON_STRATEGY);
        CustomLabel_SetUserDataInt(hCtl, (int)Strategy::Option);
        wszText = AfxUpper(StrategyButton_GetStrategyEnumText(Strategy::Option));
        CustomLabel_SetText(hCtl, wszText);

        StrategyButton_InvokeStrategy();
        return;
    }


    if (tdd.legs.size() == 0) return;

    int DefaultQuantity = 1;

    // Display the Trade's Buying Power BP in case that needs to be edited also.
    CustomTextBox_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTRADEBP), std::to_wstring(tdd.trade->trade_bp));

     
    // Display the legs being closed and set each to the needed inverse action.
    HWND hGridMain = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TABLEGRIDMAIN);
    TradeGrid* pData = TradeGrid_GetOptions(hGridMain);
    if (pData == nullptr) return;

    int row = 0;
    for (const auto& leg : tdd.legs) {

        // Only load a maximum of 4 legs even if the user had selected more than 4.
        if (row > 3) break;

        // QUANTITY
        std::wstring legQuantity = std::to_wstring(leg->open_quantity * -1);

        TradeGrid_SetColData(hGridMain, row, 0, legQuantity);

        // EXPIRY DATE
        TradeGrid_SetColData(hGridMain, row, 1, leg->expiry_date);

        // STRIKE PRICE
        TradeGrid_SetColData(hGridMain, row, 3, leg->strike_price);

        // PUT/CALL
        TradeGrid_SetColData(hGridMain, row, 4, leg->put_call);

        // ACTION
        if (leg->action == L"STO") { wszText = L"BTC"; }
        if (leg->action == L"BTO") { wszText = L"STC"; }
        TradeGrid_SetColData(hGridMain, row, 5, wszText);

        row++;
    }
    
    // DTE
    TradeGrid_CalculateDTE(hGridMain);

    // QUANTITY
    AfxSetWindowText(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTQUANTITY), std::to_wstring(abs(DefaultQuantity)));


    // Add some default information for the new Roll transaction
    if (tdd.tradeAction == TradeAction::RollLeg) {
        HWND hGridRoll = GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_TABLEGRIDROLL);
        TradeGrid* pData = TradeGrid_GetOptions(hGridRoll);
        if (pData == nullptr) return;

        int row = 0;
        for (const auto& leg : tdd.legs) {

            // Only load a maximum of 4 legs even if the user had selected more than 4.
            if (row > 3) break;

            // QUANTITY
            std::wstring legQuantity = std::to_wstring(leg->open_quantity);

            TradeGrid_SetColData(hGridRoll, row, 0, legQuantity);

            // EXPIRY DATE
            wszText = AfxDateAddDays(leg->expiry_date, 7);
            TradeGrid_SetColData(hGridRoll, row, 1, wszText);

            // STRIKE PRICE
            TradeGrid_SetColData(hGridRoll, row, 3, leg->strike_price);

            // PUT/CALL
            TradeGrid_SetColData(hGridRoll, row, 4, leg->put_call);

            // ACTION
            TradeGrid_SetColData(hGridRoll, row, 5, leg->action);

            row++;
        }

        // DTE
        TradeGrid_CalculateDTE(hGridRoll);

    }
    // Set the DR/CR to debit if this is a closetrade
    if (tdd.tradeAction == TradeAction::CloseLeg) {
        TradeDialog_SetComboDRCR(GetDlgItem(HWND_TRADEDIALOG, IDC_TRADEDIALOG_COMBODRCR), L"DR");
    }

}


// ========================================================================================
// Get the description for the type of action being performed.
// Also sets the descriptin labels above the Table main grid and Table roll grid.
// Also sets the Action label (in uppercase)
// ========================================================================================
std::wstring TradeDialogControls_GetTradeDescription(HWND hwnd)
{
    std::wstring wszDescription;
    std::wstring wszGridMain;
    std::wstring wszGridRoll;

    switch (tdd.tradeAction)
    {
    case TradeAction::EditTransaction:
        wszDescription = L"Edit";
        wszGridMain = L"Transaction Data";
        wszGridRoll = L"Transaction Data";
        break;

    case TradeAction::NewOptionsTrade:
    case TradeAction::NewShortStrangle:
    case TradeAction::NewShortCall:
    case TradeAction::NewIronCondor:
    case TradeAction::NewShortLT112:
    case TradeAction::NewShortPut:
        wszDescription = L"Options";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::NewSharesTrade:
    case TradeAction::ManageShares:
        wszDescription = L"Shares";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::NewFuturesTrade:
    case TradeAction::ManageFutures:
        wszDescription = L"Futures";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::CloseLeg:
        wszDescription = L"Close";
        wszGridMain = L"Close Transaction";
        break;
    case TradeAction::ExpireLeg:
        wszDescription = L"Expire";
        break;
    case TradeAction::RollLeg:
        wszDescription = L"Roll";
        wszGridMain = L"Close Transaction";
        wszGridRoll = L"Rolled Transaction";
        break;
    case TradeAction::Assignment:
        wszDescription = L"Assignment";
        break;
    case TradeAction::AddOptionsToTrade:
        wszDescription = L"Add Options";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::AddPutToTrade:
        wszDescription = L"Add Put";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::AddCallToTrade:
        wszDescription = L"Add Call";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::AddSharesToTrade:
        wszDescription = L"Add Shares";
        wszGridMain = L"New Transaction";
        break;
    case TradeAction::AddFuturesToTrade:
        wszDescription = L"Add Futures";
        wszGridMain = L"New Transaction";
        break;
    }

    CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLEDITACTION), AfxUpper(wszDescription));
    CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLGRIDMAIN), wszGridMain);
    CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLGRIDROLL), wszGridRoll);

    return wszDescription;
}


// ========================================================================================
// Helper function for WM_CREATE message for window/dialog: TradeDialog
// ========================================================================================
void TradeDialogControls_CreateControls(HWND hwnd)
{
    HWND hCtl = NULL;
    CustomLabel* pData = nullptr;

    int HTextMargin = 0;
    int VTextMargin = 3;

    // EDIT ACTION LABEL
    hCtl = CreateCustomLabel(
        hwnd, IDC_TRADEDIALOG_LBLEDITACTION, CustomLabelType::TextOnly,
        542, 10, 120, 20);
    pData = CustomLabel_GetOptions(hCtl);
    if (pData) {
        pData->BackColor = COLOR_GRAYDARK;
        pData->BackColorButtonDown = COLOR_GRAYDARK;
        pData->TextColor = COLOR_WHITELIGHT;
        pData->FontSize = 12;
        pData->FontBold = true;
        pData->TextAlignment = CustomLabelAlignment::TopRight;
        CustomLabel_SetOptions(hCtl, pData);
    }


    // NEW TRADE SHOWS TEXTBOXES, OTHERS JUST LABELS
    if (IsNewOptionsTradeAction(tdd.tradeAction) == true ||
        IsNewSharesTradeAction(tdd.tradeAction) == true) {
        CustomLabel_SimpleLabel(hwnd, -1, L"Ticker", COLOR_WHITEDARK, COLOR_GRAYDARK,
            CustomLabelAlignment::MiddleLeft, 40, 20, 65, 22);
        hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTTICKER, false, ES_LEFT | ES_UPPERCASE, L"", 40, 45, 65, 23);
        CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
        CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);

        hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLCOMPANY, L"Company Name", COLOR_WHITEDARK, COLOR_GRAYDARK,
            CustomLabelAlignment::MiddleLeft, 115, 20, 115, 22);
        if (tdd.tradeAction == TradeAction::NewFuturesTrade) {
            CustomLabel_SetText(hCtl, L"Futures Contract");
        }
        hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTCOMPANY, false, ES_LEFT, L"", 115, 45, 215, 23);
        CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
        CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);

    }
    else {
        hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTTICKER, false, ES_LEFT | ES_UPPERCASE, L"", 0, 0, 0, 0);
        ShowWindow(hCtl, SW_HIDE);
        hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTCOMPANY, false, ES_LEFT, L"", 0, 0, 0, 0);
        ShowWindow(hCtl, SW_HIDE);

        hCtl = CreateCustomLabel(
            hwnd, IDC_TRADEDIALOG_LBLCOMPANY, CustomLabelType::TextOnly,
            40, 10, 250, 22);
        pData = CustomLabel_GetOptions(hCtl);
        if (pData) {
            pData->BackColor = COLOR_GRAYDARK;
            pData->BackColorButtonDown = COLOR_GRAYDARK;
            pData->TextColor = COLOR_WHITELIGHT;
            pData->FontSize = 12;
            pData->TextAlignment = CustomLabelAlignment::TopLeft;
            pData->wszText = L"";
            CustomLabel_SetOptions(hCtl, pData);
        }

        hCtl = CreateCustomLabel(
            hwnd, IDC_TRADEDIALOG_LBLTICKER, CustomLabelType::TextOnly,
            40, 33, 250, 22);
        pData = CustomLabel_GetOptions(hCtl);
        if (pData) {
            pData->BackColor = COLOR_GRAYDARK;
            pData->BackColorButtonDown = COLOR_GRAYDARK;
            pData->TextColor = COLOR_WHITEDARK;
            pData->FontSize = 10;
            pData->TextAlignment = CustomLabelAlignment::TopLeft;
            pData->wszText = L"";
            CustomLabel_SetOptions(hCtl, pData);
        }
    }


    bool ShowContractExpiry = false;
    std::wstring wszContractDate;

    if (tdd.tradeAction == TradeAction::NewFuturesTrade) {
        wszContractDate = AfxCurrentDate();
        ShowContractExpiry = true;
    }
    if (tdd.tradeAction == TradeAction::EditTransaction) {
        if (IsFuturesTicker(tdd.trade->ticker_symbol)) {   // this is a Future
            wszContractDate = tdd.trade->future_expiry;
            ShowContractExpiry = true;
        }
    }
    
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLCONTRACT, L"Contract Expiry", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 340, 20, 120, 22);
    CustomLabel_SetUserData(hCtl, wszContractDate);
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE, AfxLongDate(wszContractDate),
        COLOR_WHITELIGHT, COLOR_GRAYMEDIUM, CustomLabelAlignment::MiddleLeft, 340, 45, 79, 23);
    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRADEDIALOG_CMDCONTRACTDATE, GLYPH_DROPDOWN,
        COLOR_WHITEDARK, COLOR_GRAYMEDIUM, COLOR_GRAYLIGHT, COLOR_GRAYMEDIUM, COLOR_WHITE,
        CustomLabelAlignment::MiddleCenter, 419, 45, 23, 23);
        
    if (ShowContractExpiry == true) {
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCOMPANY), L"Futures Contract");
    }
    else {
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACT), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLCONTRACTDATE), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_CMDCONTRACTDATE), SW_HIDE);
    }


    // CATEGORY SELECTOR
    int SelectedIndex = (tdd.trade != nullptr) ? tdd.trade->category : 0;
    hCtl = CreateCategoryControl(hwnd, IDC_TRADEDIALOG_CATEGORY, 450, 45, SelectedIndex, false);
    ShowWindow(hCtl, SW_HIDE);
    if (IsNewOptionsTradeAction(tdd.tradeAction) == true ||
        IsNewSharesTradeAction(tdd.tradeAction) == true ||
        tdd.tradeAction == TradeAction::EditTransaction) {
        ShowWindow(hCtl, SW_SHOW);
    }


    CustomLabel_SimpleLabel(hwnd, -1, L"Date", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 40, 72, 100, 22);
    std::wstring wszDate = AfxCurrentDate();
    hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLTRANSDATE, AfxLongDate(wszDate), COLOR_WHITELIGHT, COLOR_GRAYMEDIUM,
        CustomLabelAlignment::MiddleLeft, 40, 97, 86, 23);
    CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
    CustomLabel_SetUserData(hCtl, wszDate);

    CustomLabel_ButtonLabel(hwnd, IDC_TRADEDIALOG_CMDTRANSDATE, GLYPH_DROPDOWN,
        COLOR_WHITEDARK, COLOR_GRAYMEDIUM, COLOR_GRAYLIGHT, COLOR_GRAYMEDIUM, COLOR_WHITE,
        CustomLabelAlignment::MiddleCenter, 126, 97, 23, 23);

    // We always want the Description textbox to exists because even for rolled and closed transaction
    // we need to set the description (even though the user will never see the actual textbox in those
    // types of actions).

    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLDESCRIBE, L"Description", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 159, 72, 115, 22);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTDESCRIBE, false, ES_LEFT, L"", 159, 97, 171, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);

    if (IsNewOptionsTradeAction(tdd.tradeAction) == false ||
        IsNewSharesTradeAction(tdd.tradeAction) == true) {
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLDESCRIBE), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTDESCRIBE), SW_HIDE);
    }

    if (tdd.tradeAction == TradeAction::EditTransaction) {
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLDESCRIBE), SW_SHOW);
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTDESCRIBE), SW_SHOW);
    }



    // We create the Strategy button and label but we only show it for New options
    // However we do need the window for other tradeAction cases for example "Add To"
    // because the tradeAction into the Strategy button and then InvokeStrategy.
    if (IsNewOptionsTradeAction(tdd.tradeAction) == true ||
        tdd.tradeAction == TradeAction::AddOptionsToTrade ||
        tdd.tradeAction == TradeAction::AddPutToTrade ||
        tdd.tradeAction == TradeAction::AddCallToTrade) {

        hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLSTRATEGY, L"Strategy", COLOR_WHITEDARK, COLOR_GRAYDARK,
            CustomLabelAlignment::MiddleLeft, 340, 72, 100, 22);
        hCtl = StrategyButton.Create(hwnd, L"", 340, 97, 264, 23,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, WS_EX_CONTROLPARENT);
    }


    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLGRIDMAIN, L"", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 40, 155, 300, 22);

    if (IsNewSharesTradeAction(tdd.tradeAction) == true ||
        tdd.tradeAction == TradeAction::AddSharesToTrade ||
        tdd.tradeAction == TradeAction::AddFuturesToTrade) {
        std::wstring wszFontName = L"Segoe UI";
        std::wstring wszText;
        int FontSize = 8;

        hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_BUYSHARES, L"",
            COLOR_WHITELIGHT, COLOR_GRAYMEDIUM,
            CustomLabelAlignment::MiddleLeft, 40, 180, 150, 23);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Long);
        CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
        CustomLabel_SetTextOffset(hCtl, 5, 0);
        TradeDialog_SetLongShortBackColor(hCtl);
        CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
        wszText = L"BUY LONG ";
        if (tdd.tradeAction == TradeAction::NewSharesTrade) wszText += L"SHARES";
        if (tdd.tradeAction == TradeAction::AddSharesToTrade) wszText += L"SHARES";
        if (tdd.tradeAction == TradeAction::NewFuturesTrade) wszText += L"FUTURES";
        if (tdd.tradeAction == TradeAction::AddFuturesToTrade) wszText += L"FUTURES";
        CustomLabel_SetText(hCtl, wszText);

        hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRADEDIALOG_BUYSHARES_DROPDOWN, GLYPH_DROPDOWN,
            COLOR_WHITEDARK, COLOR_GRAYMEDIUM, COLOR_GRAYLIGHT, COLOR_GRAYMEDIUM, COLOR_WHITE,
            CustomLabelAlignment::MiddleCenter, 191, 180, 30, 23);
        CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
        CustomLabel_SetTextColorHot(hCtl, COLOR_WHITELIGHT);

    } else if (tdd.tradeAction == TradeAction::ManageShares || tdd.tradeAction == TradeAction::ManageFutures) {
        std::wstring wszFontName = L"Segoe UI";
        std::wstring wszText;
        int FontSize = 8;

        hCtl = CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_SELLSHARES, L"",
            COLOR_WHITELIGHT, COLOR_GRAYMEDIUM,
            CustomLabelAlignment::MiddleLeft, 40, 180, 150, 23);
        CustomLabel_SetUserDataInt(hCtl, (int)LongShort::Short);
        CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
        CustomLabel_SetTextOffset(hCtl, 5, 0);
        TradeDialog_SetLongShortBackColor(hCtl);
        CustomLabel_SetMousePointer(hCtl, CustomLabelPointer::Hand, CustomLabelPointer::Hand);
        wszText = L"SELL LONG ";
        if (tdd.tradeAction == TradeAction::ManageShares) wszText += L"SHARES";
        if (tdd.tradeAction == TradeAction::ManageFutures) wszText += L"FUTURES";
        CustomLabel_SetText(hCtl, wszText);

        hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRADEDIALOG_SELLSHARES_DROPDOWN, GLYPH_DROPDOWN,
            COLOR_WHITEDARK, COLOR_GRAYMEDIUM, COLOR_GRAYLIGHT, COLOR_GRAYMEDIUM, COLOR_WHITE,
            CustomLabelAlignment::MiddleCenter, 191, 180, 30, 23);
        CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
        CustomLabel_SetTextColorHot(hCtl, COLOR_WHITELIGHT);

    }
    else
    {
        bool bShowOriginalQuantity = (tdd.tradeAction == TradeAction::EditTransaction) ? true : false;

        // Create the main trade options leg grid
        HWND hGridMain = CreateTradeGrid(hwnd, IDC_TRADEDIALOG_TABLEGRIDMAIN, 40, 180, 0, 0, bShowOriginalQuantity);

        // If we are rolling or editing then create the second trade grid.
        if (tdd.tradeAction == TradeAction::RollLeg || tdd.tradeAction == TradeAction::EditTransaction) {
            int nWidth = AfxUnScaleX((float)AfxGetWindowWidth(hGridMain)) + 60;
            CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLGRIDROLL, L"", COLOR_WHITEDARK, COLOR_GRAYDARK,
                CustomLabelAlignment::MiddleLeft, nWidth, 155, 300, 22);
            HWND hGridRoll = CreateTradeGrid(hwnd, IDC_TRADEDIALOG_TABLEGRIDROLL, nWidth, 180, 0, 0, bShowOriginalQuantity);
        }
    }


    CustomLabel_SimpleLabel(hwnd, -1, L"Quantity", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleRight, 40, 310, 80, 23);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTQUANTITY, false, ES_RIGHT,
        L"0", 40, 337, 80, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);
    CustomTextBox_SetNumericAttributes(hCtl, 5, CustomTextBoxNegative::Disallow, CustomTextBoxFormatting::Allow);
    if (tdd.tradeAction == TradeAction::ManageShares ||
        tdd.tradeAction == TradeAction::ManageFutures) {
        // If the aggregate shares are negative then toggle the sell to buy in order to close the trade
        int aggregate = AfxValInteger(tdd.sharesAggregateEdit);
        CustomTextBox_SetText(hCtl, std::to_wstring(abs(aggregate)));  // set quantity before doing the toggle
    }

    
    CustomLabel_SimpleLabel(hwnd, -1, L"Price", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleRight, 130, 310, 80, 23);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTPRICE, false, ES_RIGHT,
        L"0", 130, 337, 80, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);
    CustomTextBox_SetNumericAttributes(hCtl, 5, CustomTextBoxNegative::Disallow, CustomTextBoxFormatting::Allow);


    CustomLabel_SimpleLabel(hwnd, -1, L"Multiplier", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleRight, 220, 310, 80, 23);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTMULTIPLIER, false, ES_RIGHT,
        L"100.0000", 220, 337, 80, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);
    CustomTextBox_SetNumericAttributes(hCtl, 5, CustomTextBoxNegative::Disallow, CustomTextBoxFormatting::Allow);
    if (tdd.tradeAction == TradeAction::NewSharesTrade ||
        tdd.tradeAction == TradeAction::NewFuturesTrade ||
        tdd.tradeAction == TradeAction::ManageShares ||
        tdd.tradeAction == TradeAction::ManageFutures ||
        tdd.tradeAction == TradeAction::AddSharesToTrade ||
        tdd.tradeAction == TradeAction::AddFuturesToTrade) {
        CustomTextBox_SetText(hCtl, L"1");
    }


    CustomLabel_SimpleLabel(hwnd, -1, L"Fees", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleRight, 310, 310, 80, 23);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTFEES, false, ES_RIGHT,
        L"0", 310, 337, 80, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);
    CustomTextBox_SetNumericAttributes(hCtl, 2, CustomTextBoxNegative::Disallow, CustomTextBoxFormatting::Allow);


    CustomLabel_SimpleLabel(hwnd, -1, L"Total", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleRight, 400, 310, 80, 23);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTTOTAL, false, ES_RIGHT,
        L"0", 400, 337, 80, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);
    CustomTextBox_SetNumericAttributes(hCtl, 2, CustomTextBoxNegative::Disallow, CustomTextBoxFormatting::Allow);


    std::wstring wszFontName = L"Segoe UI";
    int FontSize = 9;
    bool bold = false;

    // DR / CR toggle label
    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRADEDIALOG_COMBODRCR, L"CR",
        COLOR_BLACK, COLOR_GREEN, COLOR_GREEN, COLOR_GRAYMEDIUM, COLOR_WHITE,
        CustomLabelAlignment::MiddleCenter, 490, 337, 30, 23);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITELIGHT);
    if (IsNewSharesTradeAction(tdd.tradeAction) == true ||
        tdd.tradeAction == TradeAction::AddSharesToTrade ||
        tdd.tradeAction == TradeAction::AddFuturesToTrade) {
        TradeDialog_SetComboDRCR(hCtl, L"DR");
    } else {
        TradeDialog_SetComboDRCR(hCtl, L"CR");
    }


    // TRADE BUYING POWER
    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLTRADEBP, L"Buying Power", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 580, 310, 100, 23);
    hCtl = CreateCustomTextBox(hwnd, IDC_TRADEDIALOG_TXTTRADEBP, false, ES_RIGHT,
        L"0", 580, 337, 80, 23);
    CustomTextBox_SetMargins(hCtl, HTextMargin, VTextMargin);
    CustomTextBox_SetColors(hCtl, COLOR_WHITELIGHT, COLOR_GRAYMEDIUM);
    CustomTextBox_SetNumericAttributes(hCtl, 2, CustomTextBoxNegative::Allow, CustomTextBoxFormatting::Allow);

    if (tdd.tradeAction == TradeAction::NewSharesTrade ||
        tdd.tradeAction == TradeAction::ManageShares ||
        tdd.tradeAction == TradeAction::AddSharesToTrade ||
        tdd.tradeAction == TradeAction::NewFuturesTrade ||
        tdd.tradeAction == TradeAction::ManageFutures ||
        tdd.tradeAction == TradeAction::AddFuturesToTrade) {
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLTRADEBP), SW_HIDE);
        ShowWindow(GetDlgItem(hwnd, IDC_TRADEDIALOG_TXTTRADEBP), SW_HIDE);
    }

    // SAVE button
    hCtl = CustomLabel_ButtonLabel(hwnd, IDC_TRADEDIALOG_SAVE, L"SAVE",
        COLOR_BLACK, COLOR_GREEN, COLOR_GREEN, COLOR_GRAYMEDIUM, COLOR_WHITE,
        CustomLabelAlignment::MiddleCenter, 580, 396, 80, 23);
    CustomLabel_SetFont(hCtl, wszFontName, FontSize, true);
    CustomLabel_SetTextColorHot(hCtl, COLOR_WHITELIGHT);


    TradeDialogControls_GetTradeDescription(hwnd);


    // If the aggregate shares are negative then toggle the sell to buy in order to close the trade.
    // Must do this after the quantity, multiplier, etc have been set otherwise we'll get a GPF
    // when the calculate totals eventually gets called during the DR/CR toggle.
    if (tdd.tradeAction == TradeAction::ManageShares ||
        tdd.tradeAction == TradeAction::ManageFutures) {
        int aggregate = AfxValInteger(tdd.sharesAggregateEdit);
        if (aggregate < 0) {
            TradeDialog_ToggleSellLongShortText(GetDlgItem(hwnd, IDC_TRADEDIALOG_SELLSHARES));
            TradeDialog_SetLongShortBackColor(GetDlgItem(hwnd, IDC_TRADEDIALOG_SELLSHARES));
        }
    }


    // EDIT TRANSACTION WARNING
    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING1, L"", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 40, 380, 80, 16);
    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING2, L"", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 120, 380, 500, 16);
    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING3, L"", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 120, 396, 500, 16);
    CustomLabel_SimpleLabel(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING4, L"", COLOR_WHITEDARK, COLOR_GRAYDARK,
        CustomLabelAlignment::MiddleLeft, 120, 412, 500, 16);

    if (tdd.tradeAction == TradeAction::NewFuturesTrade ||
        tdd.tradeAction == TradeAction::AddFuturesToTrade) {
        std::wstring wszText1 = L"NOTE:";
        std::wstring wszText2 = L"Future Ticker names must start with a forward slash.";
        std::wstring wszText3 = L"For example: /ES, /MES, /NQ, etc.";
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING1), wszText1);
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING2), wszText2);
        CustomLabel_SetText(GetDlgItem(hwnd, IDC_TRADEDIALOG_LBLEDITWARNING3), wszText3);
    }

}

