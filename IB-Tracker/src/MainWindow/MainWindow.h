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

#include "Utilities/CWindowBase.h"


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

extern CMainWindow Main;
extern HWND HWND_MAINWINDOW;
extern HWND HWND_LEFTPANEL;

#define IDI_MAINICON 106
#define IDB_CONNECT 110
#define IDB_DISCONNECT 111
#define IDB_RECONCILE 114


constexpr int IDC_MAINWINDOW_WARNING = 100;
constexpr int IDC_MAINWINDOW_UPDATEAVAILABLE = 101;

constexpr int APP_LEFTMARGIN_WIDTH = 24;


void MainWindow_BlurPanels(bool active);
void MainWindow_SetLeftPanel(HWND hPanel);
void MainWindow_SetRightPanel(HWND hPanel);
void MainWindow_CloseComboPopups();

