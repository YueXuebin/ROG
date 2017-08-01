#include "commonlib_pch.h"
#include "signal_try_catcher.h"

jmp_buf g_JmpBuffer;
bool    g_InSetJmp = false;