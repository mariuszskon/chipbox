#ifndef CHIPBOX_LOG_H
#define CHIPBOX_LOG_H

/* log levels of importance */
#define CHIPBOX_LOG_LEVEL_NONE 0
#define CHIPBOX_LOG_LEVEL_INFO 1
#define CHIPBOX_LOG_LEVEL_WARN 2
#define CHIPBOX_LOG_LEVEL_ERROR 3

/* possible log message symbols */
#define CHIPBOX_LOG_UNIMPL 0 /* unimplemented */
#define CHIPBOX_LOG_IMPL_DEFINED 1 /* implementation-defined state - other implementations may be different */
#define CHIPBOX_LOG_UNSAFE 2 /* instruction does something unsafe/undefined for original hardware/implementation */
#define CHIPBOX_LOG_ILLEGAL 3 /* instruction attempts to do something that could lead to undefined behaviour in this implementation */
#define CHIPBOX_LOG_RANGE 4 /* an input value was out of range of valid inputs */
#define CHIPBOX_LOG_WAIT 5 /* waiting on input */

#endif
