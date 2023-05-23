#pragma once

// User defined messages
const int MSG_CUSTOMLABEL_CLICK			= WM_USER + 1000;
const int MSG_CUSTOMLABEL_MOUSEMOVE		= WM_USER + 1001;
const int MSG_CUSTOMLABEL_MOUSELEAVE	= WM_USER + 1002;
const int MSG_TWS_CONNECT_START			= WM_USER + 1003;
const int MSG_TWS_CONNECT_SUCCESS		= WM_USER + 1004;
const int MSG_TWS_CONNECT_FAILURE		= WM_USER + 1005;
const int MSG_TWS_CONNECT_DISCONNECT	= WM_USER + 1006;
const int MSG_STARTUP_SHOWTRADES        = WM_USER + 1007;
const int MSG_DATEPICKER_DATECHANGED    = WM_USER + 1008;

const int DIALOG_RETURN_OK = 0;
const int DIALOG_RETURN_CANCEL = 1;