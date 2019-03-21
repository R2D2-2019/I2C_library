#include "i2c_bus.hpp"

namespace r2d2::i2c {
    void i2c_bus_c::pin_init() {
        auto config_pin = [](uint32_t pin) {
            PIOA->PIO_ABSR &= (~pin & PIOA->PIO_ABSR);
            PIOA->PIO_PDR = pin;
            PIOA->PIO_IDR = pin;
            PIOA->PIO_PUER = pin;
        };

        if (_selected == TWI0) {
            config_pin(PIO_PA18A_TWCK0);
            config_pin(PIO_PA17A_TWD0);
        } else if (_selected == TWI1) {
            config_pin(PIO_PB13A_TWCK1);
            config_pin(PIO_PB12A_TWD1);
        }
    }
    void i2c_bus_c::clock_init() {
        if (_selected == TWI0) {
            if ((PMC->PMC_PCSR0 & (1 << ID_TWI0)) != (1 << ID_TWI0)) {
                PMC->PMC_PCER0 = 1 << ID_TWI0;
            }
        } else if (_selected == TWI1) {
            if ((PMC->PMC_PCSR0 & (1 << ID_TWI1)) != (1 << ID_TWI1)) {
                PMC->PMC_PCER0 = 1 << ID_TWI1;
            }
        }

        _selected->TWI_CR = TWI_CR_SVEN;
        _selected->TWI_CR = TWI_CR_SWRST;
        _selected->TWI_CR = TWI_CR_SVDIS;
        _selected->TWI_CR = TWI_CR_MSDIS;
        _selected->TWI_CR = TWI_CR_MSEN;

        static constexpr uint32_t masterClock =
            84000000;        ///< Master clock rate 84MHz
        uint32_t ckdiv = 0;  ///< Clock divider
        uint32_t cLHDiv = 0; ///< Clock low and high divider
        if (SPEED > 400000) {
            return;
        }
        cLHDiv = masterClock / (SPEED * 2) - 4;

        while ((cLHDiv > 0xFF) &&
               (ckdiv <
                7)) { ///< cldiv must fit in 8 bits, ckdiv must fit in 3 bits
            ckdiv++;  ///< Increase clock devider
            cLHDiv /= 2;
        }
        _selected->TWI_CWGR = TWI_CWGR_CLDIV(cLHDiv) | TWI_CWGR_CHDIV(cLHDiv) |
                              TWI_CWGR_CKDIV(ckdiv);
    }
    void i2c_bus_c::init() {
        static bool init_done = false;
        if (init_done) {
            return;
        }
        if (_selected == nullptr) {
            return;
        }
        pin_init();
        clock_init();
    }
    void i2c_bus_c::write_byte(const uint8_t data) {
        _selected->TWI_THR = data;
    }
    uint8_t i2c_bus_c::read_byte() {
        return _selected->TWI_RHR;
    }
    i2c_bus_c::i2c_bus_c(const interface &selected_interface,
                         const uint32_t &SPEED)
        : SPEED(SPEED) {
        if (selected_interface == interface::interface_0) {
            _selected = TWI0;
        } else if (selected_interface == interface::interface_1) {
            _selected = TWI1;
        }
        init();
    }
    void i2c_bus_c::write(const uint_fast8_t address, const uint8_t data[],
                          const size_t &n) {
        _selected->TWI_MMR = 0; ///< Reset master mode register
        _selected->TWI_MMR = 0 << 12 | address << 16; ///< Set write and address
        _selected->TWI_IADR = 0; ///< Clear internal address

        uint32_t status = 0;

        for (size_t i = 0; i < n; i++) {
            status = _selected->TWI_SR;
            if (status & TWI_SR_NACK) {
                hwlib::cout << "status & NACK" << hwlib::endl;
            }

            if (status & TWI_SR_TXRDY) {
                write_byte(data[i]);
            }
        }

        while (1) {
            status = _selected->TWI_SR;
            if (status & TWI_SR_TXRDY) {
                break;
            }
        }

        _selected->TWI_CR = TWI_CR_STOP;
        while (!(_selected->TWI_SR & TWI_SR_TXCOMP)) {
        };
    }
    void i2c_bus_c::read(const uint8_t address, uint8_t *data,
                         const uint32_t &n) {

        _selected->TWI_MMR = 0; ///< Reset master mode register
        _selected->TWI_MMR = 1 << 12 | address << 16; ///< Set read and address
        _selected->TWI_IADR = 0; ///< Clear internal address

        uint32_t status = 0; ///< Variable for holding status register
        uint32_t count = n;
        uint8_t stopTransaction =
            0; ///< Byte to indicate that transaction should be stopped

        if (count == 1) { ///< When only one byte needs to be read, transaction
                          ///< should be started and stopped at once.
            _selected->TWI_CR = TWI_CR_START | TWI_CR_STOP;
            stopTransaction = 1;
        } else {
            _selected->TWI_CR = TWI_CR_START;
        }

        while (count > 0) {
            status = _selected->TWI_SR;
            if (status & TWI_SR_NACK) {
                return;
            }

            if (count == 1 && !stopTransaction) {
                _selected->TWI_CR = TWI_CR_STOP;
                stopTransaction = 1;
            }

            if (!(status & TWI_SR_RXRDY)) {
                continue;
            }
            data[n - count] = read_byte();

            count--;
        }
    }
} // namespace r2d2::i2c