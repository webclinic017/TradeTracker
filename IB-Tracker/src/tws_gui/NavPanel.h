#pragma once

#include "framework.h"
#include "CWindow.h"
#include "SuperLabel.h"

const int IDC_FRMNAVPANEL           = 100;

const int IDC_NAVPANEL_LOGO         = 101;
const int IDC_NAVPANEL_GEARICON     = 102;
const int IDC_NAVPANEL_USERNAME     = 103;
const int IDC_NAVPANEL_COMPANY      = 104;
const int IDC_NAVPANEL_DASHBOARD    = 105;
const int IDC_NAVPANEL_CLIENTS      = 106;
const int IDC_NAVPANEL_INVOICES     = 107;
const int IDC_NAVPANEL_EXPENSES     = 108;
const int IDC_NAVPANEL_ESTIMATES    = 109;
const int IDC_NAVPANEL_TIMETRACKING = 110;
const int IDC_NAVPANEL_PROJECTS     = 111;
const int IDC_NAVPANEL_MYTEAM       = 112;
const int IDC_NAVPANEL_REPORTS      = 113;
const int IDC_NAVPANEL_ACCOUNTING   = 114;
const int IDC_NAVPANEL_APPNAME      = 115;
const int IDC_NAVPANEL_BOTTOMSEP    = 116;

const int NAVPANEL_WIDTH = 180;

CWindow* NavPanel_Show(HWND hWndParent);
