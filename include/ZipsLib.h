// Zack Young ZB25

#pragma once

#ifndef _STDARG_H
#include <stdarg.h>
#endif

#ifndef _PICO_STDLIB_H
#include <pico/stdlib.h>
#endif

#ifdef USING_PICO_W
#ifndef _PICO_CYW43_ARCH_H
#include <pico/cyw43_arch.h>
#endif
#endif

// Change these to zero or one depending on the use case!
// Printing is turned off by default and program looping is turned on by default.
// You can also `#define` the macro *BEFORE* including the header to make changes.
#ifndef USING_PRINT
#define USING_PRINT 0
#endif
#ifndef LOOP_PROGRAM_ON_ERROR
#define LOOP_PROGRAM_ON_ERROR 1
#endif
#ifndef PREINIT_LED
#define PREINIT_LED 1
#endif
//

#if USING_PRINT
#include <pico/stdio.h>
#include <stdio.h>
#define LOG(...) printf(__VA_ARGS__)
#define LOGCHAR(v) putchar(v)
#else
#define LOG(...)
#define LOGCHAR(v)
#endif

#if defined(USING_PICO) || defined(USING_PICO_2)
#define LED_PIN 25
#define PICO_LED_ON() gpio_put(LED_PIN, 1)
#define PICO_LED_OFF() gpio_put(LED_PIN, 0)
#define PICO_LED_TOGGLE() gpio_put(LED_PIN, !gpio_get(LED_PIN))
#define PICO_LED_INIT() gpio_init(LED_PIN); gpio_set_dir(LED_PIN, GPIO_OUT)
#elif defined(USING_PICO_W)
#define LED_PIN 0
#define PICO_LED_ON() cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1)
#define PICO_LED_OFF() cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0)
#define PICO_LED_TOGGLE() cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, !cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN))
#define PICO_LED_INIT()
#else
#warning "Please define a macro for the Pico board type."
#endif

#define BEGIN_SETUP() PICO_LED_ON()
#define BEGIN_LOOP() PICO_LED_OFF()

#if LOOP_PROGRAM_ON_ERROR
#define THROW(...) \
    while (1) \
    { \
    PICO_LED_ON(); \
    sleep_ms(500); \
    PICO_LED_OFF(); \
    sleep_ms(500); \
    LOG(__VA_ARGS__); \
    }
#else
#define THROW(...) LOG(__VA_ARGS__); return -1
#endif

static inline void init_libs()
{
#if USING_PRINT
    if (!stdio_init_all())
        THROW("Failed to init Standard I/O.");
#endif
#ifdef USING_PICO_W
    if (cyw43_arch_init())
        THROW("Failed to init Wi-Fi.");
#endif
#if PREINIT_LED
    PICO_LED_INIT();
#endif
}

#define resume_on(func) while (1) \
    { \
        if (func) \
            break; \
    }

typedef bool (*vararg_func_ptr_t)(...);
static inline void resume_on_ptr(vararg_func_ptr_t func, ...)
{
    va_list args;
    while (1)
    {
        if ((*func)(args));
            break;
    }
}