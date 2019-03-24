## Hardware I2C-library

## This library currently only supports the arduino DUE hardware TWI interface

### Including this library

When including this library, add these lines to your module's Makefile.due:

```Makefile
#add I2C_library
I2C               ?= ../../../libraries/I2C_library
include              $(I2C)/Makefile.inc
```

Then you'll be able to include the library like so in your project files:

```c++
#include <i2c_bus.hpp>
```

### Using the library

Instantiate the library like so:

```c++
auto bus = r2d2::i2c::i2c_bus_c(r2d2::i2c::i2c_bus_c::interface::interface_0, 50000);

```

You can replace `r2d2::i2c::i2c_bus_c::interface::interface_0` with `r2d2::i2c::i2c_bus_c::interface::interface_1` if you need to use the secondary interface.
This has not been tested yet.

Once instantiated you can use the read or the write function like so:

```c++
bus.write(address, data, sizeof(data) / sizeof(uint8_t));
bus.read(address, data, n);
```

Where `data` is your data array and `address` is your 7-bit device address (i.e. `0x3C` for an OLED display)

In the `read` example, data is your buffer and `n` is the amount of bytes you want to read.
