#pragma once

#include "hwlib.hpp"

namespace r2d2::i2c {
    class i2c_bus_c {
    private:
        Twi *_selected;
        uint32_t SPEED = 0; ///< Clock>
        void pin_init();
        void clock_init();
        void init();
        void write_byte(const uint8_t data);
        uint8_t read_byte();

    public:
        enum class interface { interface_0, interface_1 };
        i2c_bus_c(const interface &selected_interface, const uint32_t &SPEED);

        void write(const uint_fast8_t address, const uint8_t data[], size_t &n);
        void read(const uint8_t address, uint8_t *data, const uint32_t &n);
    };
}; // namespace r2d2::i2c