#pragma once

/******************************************************************************
		Copyright (C) Shanda Corporation. All rights reserved.

sdmacros.h - sdu相关的编译宏开关	

******************************************************************************/


#ifndef SDUMACROS_H
#define SDUMACROS_H

//SDU组件内部使用了部分boost库的内容,如正规表达式处理,
//如需要使用正规表达式函数,请打开此宏
#define SDU_WITH_BOOST   0

#define SDU_WITH_OPENSSL 0

#define SDU_WITH_LIBICONV 0

#endif //
