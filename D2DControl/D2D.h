#pragma once
#include "D2DRender.h"
#include "D2DControlUI.h"
#include "D2DText.h"
#include "D2DCoreUI.h"
#include "D2DCheckBoxUI.h"
#include "D2DEditUI.h"
#include "D2DItemUI.h"
#include "D2DListUI.h"
#include "D2DExplorerUI.h"
#include "D2DScrollBarUI.h"
#include "D2DTextAreaUI.h"
#include "D2DRichTextAreaUI.h"

#include "D2DContainerUI.h"
#include "D2DSelectUI.h"
#include "D2DWindow.h"


VOID D2DReisterClasses();

#ifndef GET_X_LPARAM_F
#define GET_X_LPARAM_F(lp)                  ((float)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM_F
#define GET_Y_LPARAM_F(lp)                    ((float)(short)HIWORD(lp))
#endif

#ifndef GET_WHEEL_DELTA_WPARAM_F

#define GET_WHEEL_DELTA_WPARAM_F(wParam)  ((float)(short)HIWORD(wParam))

#endif
