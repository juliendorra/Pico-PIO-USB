#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

void log_init(void);
int log_printf(const char *fmt, ...);
void log_flush_task(void);

#ifdef __cplusplus
}
#endif

#endif // LOG_QUEUE_H