// log_queue.c
#include "log_queue.h"
#include <stdarg.h>
#include <stdio.h>
#include "pico/util/queue.h"
#include "pico/stdlib.h"
#include "tusb.h"

#ifndef LOGQ_SIZE
#define LOGQ_SIZE 4096 // adjust if you log a lot
#endif

static queue_t logq; // byte queue

void log_init(void)
{
    // each item is 1 byte
    queue_init(&logq, 1, LOGQ_SIZE);
}

int log_printf(const char *fmt, ...)
{
    char buf[256];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (n <= 0)
        return n;

    // Enqueue bytes (non-blocking); drop extras if queue is full
    for (int i = 0; i < n; i++)
    {
        // Optional CRLF conversion here or later; here we queue '\r' before '\n'
        if (buf[i] == '\n')
        {
            uint8_t cr = '\r';
            (void)queue_try_add(&logq, &cr);
        }
        uint8_t b = (uint8_t)buf[i];
        (void)queue_try_add(&logq, &b);
    }
    return n;
}

// Call this from core0 regularly (same place you call tud_task())
void log_flush_task(void)
{
    if (!tud_mounted())
    { // device in CONFIGURED state, independent of DTR
        return;
    }

    uint8_t ch;
    while (!queue_is_empty(&logq))
    {
        if (!queue_try_remove(&logq, &ch))
            break;

        // Try to push; if the IN endpoint is busy/NAKs, just stop and try next tick
        if (tud_cdc_write_char(ch) == 0)
            break;

        if (tud_cdc_write_available() < 16)
        {
            tud_cdc_write_flush();
        }
    }
    tud_cdc_write_flush();
}