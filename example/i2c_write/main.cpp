#include <hwlib.hpp>
#include <i2c_bus.hpp>

int main(void) {
    // kill the watchdog
    WDT->WDT_MR = WDT_MR_WDDIS;
    hwlib::wait_ms(1000);
    hwlib::cout << "this works via arduino";

    using i2c_bus = r2d2::i2c::i2c_bus_c;

    // create the bus we want to use
    i2c_bus bus(i2c_bus::interface::interface_1, 100'000);

    // the address we want to write to
    constexpr uint8_t addr = 0x2A;

    // create some data
    uint8_t data[3] = {0x0F, 0xF0, 0xFF};

    for (;;) {
        // write the data to the bus on address
        bus.write(addr, data, 3);

        hwlib::wait_ms(100);
    }
}
