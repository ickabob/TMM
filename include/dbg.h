#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>


#define debug(M, ...) fprintf(stderr, "[DEBUG] %s: %d: " M "\n", __FILE__, __LINE__, __VA_ARGS__)

#define clean_errno() (errno == 0? "None" : strerror(errno))

#define log_err(M, ...) fprintf(stderr, "[ERROR] %s: %d: errno: %s " M "\n", __FILE__, __LINE__, clean_errno(), __VA_ARGS__)

#define log_info(M, ...) fprintf(stderr, "[INFO] %s: %d: " M "\n", __FILE__, __LINE__, __VA_ARGS__)

#endif
