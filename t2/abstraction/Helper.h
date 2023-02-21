#define GET_OBJECT_VARIABLE_BY_OFFSET(variable_type, object_pointer, offset_in_bytes) *((variable_type*)(((unsigned int)(object_pointer)) + (offset_in_bytes)))

#define GET_OBJECT_POINTER_TO_VARIABLE_BY_OFFSET(object_pointer, offset_in_bytes) (void*)(((unsigned int)object_pointer) + (offset_in_bytes))

#define GET_OBJECT_VMT_FUNCTION_BY_OFFSET(object_pointer, offset_in_bytes) *((unsigned int*)(*((unsigned int*)(object_pointer)) + (offset_in_bytes)))
#define GET_OBJECT_VMT_FUNCTION_BY_INDEX(object_pointer, index) *((unsigned int*)(*((unsigned int*)(object_pointer)) + (index*sizeof(void*))))

#ifndef _DEBUG
#define PLOG_DISABLE_LOGGING
#endif

#include <plog/Log.h>
#include <plog/Logger.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>

/*
#ifdef _DEBUG
#include <plog/Log.h>
#include <plog/Logger.h>
#include <plog/Init.h>
#include <plog/Formatters/TxtFormatter.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#else
#include <cstddef>
#endif
*/