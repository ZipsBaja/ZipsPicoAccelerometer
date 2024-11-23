#include <pico/stdlib.h>

#ifndef USING_MULTIPLEXING
#define USING_MULTIPLEXING 1
#endif

#if USING_MULTIPLEXING

namespace uazips
{

    struct PinMode
    {
        uint8_t pin;
        bool mode;
    };

    class VirtualMultiplexer
    {
    private:
        PinMode* pins_used;
        const size_t total_pins;

    public:
        /*
        * Constructs a new mutliplexer object.
        * @param pins All of the pins being used by this multiplexer.
        * @param total_pins The number of pins in the previous parameter's array. Use `countof(pins)` or `sizeof(pins)/sizeof(uint8_t)` if the array is stack-allocated.
        */
        inline VirtualMultiplexer(uint8_t pins[], size_t total_pins) : total_pins(total_pins)
        {
            pins_used = new PinMode[total_pins];
            for (int i = 0; i < total_pins; i++)
            {
                pins_used[i] = {pins[i], 0};
            }
            pins_used[0].mode = 1;
        }

        /*
        * Delete heap-allocated data.
        */
        inline ~VirtualMultiplexer()
        {
            delete[] pins_used;
        }

        inline void InitializePins()
        {
            for (int i = 0; i < total_pins; i++)
            {
                gpio_init(pins_used[i].pin);
            }
        }

        // Sets a specific pin number to high and all others to low.
        inline void SetHigh(uint8_t pin)
        {
            for (int i = 0; i < total_pins; i++)
                if (pins_used[i].pin == pin)
                {
                    pins_used[i].mode = 1;
                    gpio_put(pin, 1);
                }
                else
                {
                    pins_used[i].mode = 0;
                    gpio_put(pin, 0);
                }
        }

        // Sets a pin number to high by its index and all others to low, in the pin array entered during object construction.
        inline uint8_t SetHighByIndex(size_t index)
        {
            uint8_t high_pin = 0;
            for (int i = 0; i < total_pins; i++)
            {
                if (index == i)
                {
                    pins_used[i].mode = 1;
                    high_pin = pins_used[i].pin;
                    gpio_put(high_pin, 1);
                }
                else
                {
                    pins_used[i].mode = 0;
                    gpio_put(pins_used[i].pin, 0);
                }
            }
            return high_pin;
        }

        inline uint8_t GetHigh() const
        {
            for (int i = 0; i < total_pins; i++)
            {
                if (pins_used[i].mode = 1)
                    return pins_used[i].pin;
            }
            return 0;
        }

        inline size_t GetTotalPins() const
        {
            return total_pins;
        }

        inline uint8_t GetPinByIndex(size_t index) const
        {
            if (index >= total_pins)
                return 0;
            return pins_used[index].pin;
        }

    };

}

#endif