/*
*	预编译头
*/
#ifndef centerserver_pch_h__
#define centerserver_pch_h__

#include <sstream>

#include "sdu.h"

#include "message_encoder.h"

#include "tinyxml.h"

#include "json.h"

#include "commonlib.h"


// Libevent
#ifdef LINUX
#define SSIZE_T ssize_t
#endif

#include "event2/event.h"
#include "event2/buffer.h"
#include "event2/http.h"
#include "event2/keyvalq_struct.h"

#endif // centerserver_pch_h__
