#pragma once

#include "..\Utilities\CWindowBase.h"


class CMainWindow : public CWindowBase<CMainWindow>
{
public:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

class CMainWindowShadow : public CWindowBase<CMainWindowShadow>
{
public:
    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#define IDI_MAINICON           106

const int IDC_MAINWINDOW_DARKMODE = 100;
const int IDC_MAINWINDOW_AUTOCONNECT = 101;


void MainWindow_BlurPanels(bool active);
