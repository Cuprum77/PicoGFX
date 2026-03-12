#include "driver.h"

/**
 * @brief Initialize the hardware interface
*/
void hardware_driver::init()
{
#if defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
    spi_init(LCD_SPI_INSTANCE, this->config->spi.baudrate);

    // set the pins to hw function
    gpio_set_function(LCD_PIN_SDA, GPIO_FUNC_SPI);
    gpio_set_function(LCD_PIN_SCL, GPIO_FUNC_SPI);
    gpio_set_function(LCD_PIN_CS, GPIO_FUNC_SPI);

    gpio_init(LCD_PIN_DC);
    gpio_set_dir(LCD_PIN_DC, GPIO_OUT);
    gpio_put(LCD_PIN_DC, 1);

#elif defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    // create the first hw state machine
    this->pio = pio0;
    this->sm = pio_claim_unused_sm(this->pio, true);
    this->offset = pio_add_program(this->pio, &pio_spi_program);
    this->clkdiv = (float)clock_get_hz(clk_sys) / (float)this->config->spi.baudrate;
    pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_SDA, 
        LCD_PIN_SCL, this->clkdiv, (int)BITS_8);

    // set the pins to hw function
    gpio_init(LCD_PIN_CS);
    gpio_init(LCD_PIN_DC);
    gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
    gpio_set_dir(LCD_PIN_DC, GPIO_OUT);
    gpio_put(LCD_PIN_CS, 1);
    gpio_put(LCD_PIN_DC, 1);

#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    // Set the pins to output
    gpio_init(LCD_PIN_RST);
    gpio_set_dir(LCD_PIN_RST, GPIO_OUT);
    gpio_put(LCD_PIN_RST, 1);
    gpio_pull_down(LCD_PIN_RST);

    gpio_init(LCD_PIN_CS);
    gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
    gpio_put(LCD_PIN_CS, 0);
    gpio_pull_down(LCD_PIN_CS);

    gpio_init(LCD_PIN_DC);
    gpio_set_dir(LCD_PIN_DC, GPIO_OUT);
    gpio_put(LCD_PIN_DC, 1);
    gpio_pull_down(LCD_PIN_DC);

    gpio_init(LCD_PIN_RD);
    gpio_set_dir(LCD_PIN_RD, GPIO_OUT);
    gpio_put(LCD_PIN_RD, 1);
    gpio_pull_down(LCD_PIN_RD);

    gpio_init(LCD_PIN_WR);
    gpio_set_dir(LCD_PIN_WR, GPIO_OUT);
    gpio_put(LCD_PIN_WR, 1);
    gpio_pull_down(LCD_PIN_WR);

    gpio_init(LCD_PIN_IM0);
    gpio_set_dir(LCD_PIN_IM0, GPIO_OUT);
    gpio_put(LCD_PIN_IM0, 1);
    gpio_pull_down(LCD_PIN_IM0);

#if defined(LCD_PIN_DB_SEQUENTIAL)
    // Set the data pins to output
    uint32_t start = this->parallel_data_pins[0];
    uint32_t end = this->parallel_data_pins[LCD_PIN_DB_COUNT - 1];

    // Inverse order?
    if (start > end)
    {
        this->parallel_interface_reverse_order = true;

        // Not supported yet due to the lack of hardware for reversing bit order
        // this->parallel_interface_in_sequence = false;
    }

    // Figure out the biggest pin of the two
    uint32_t max_pin = (start > end) ? start : end;
    uint32_t min_pin = (start < end) ? start : end;

    this->parallel_interface_max_pin = max_pin;
    this->parallel_interface_min_pin = min_pin;

    // Use booth to create a mask for the data pins
    if (max_pin == 31)
        this->parallel_interface_mask = 0xFFFFFFFF - ((1u << min_pin) - 1);
    else if (min_pin == 0)
        this->parallel_interface_mask = (1u << (max_pin + 1)) - 1;
    else
        this->parallel_interface_mask = (1u << (max_pin + 1)) - (1u << min_pin);

    // Initialize the GPIO pins in a masked way for performance
    gpio_init_mask(this->parallel_interface_mask);
    gpio_set_dir_out_masked(this->parallel_interface_mask);
    gpio_put_masked(this->parallel_interface_mask, 0);

    for (size_t i = 0; i < LCD_PIN_DB_COUNT; i++)
        gpio_pull_down(this->parallel_data_pins[i]);
#else
    for (size_t i = 0; i < LCD_PIN_DB_COUNT; i++)
    {
        uint32_t pin = this->parallel_data_pins[i];
        printf("INIT DB%d (GPIO%d)\n", i, pin);
        gpio_init(pin);
        gpio_set_dir(pin, GPIO_OUT);
        gpio_put(pin, 1);
        gpio_pull_down(pin);
    }
#endif

#else
    #error "Unsupported display protocol or hardware interface"
#endif
}

/**
 * @brief Reset the display using the reset signal
 * @param time_ms The time to hold the reset signal low in milliseconds on both flanks
*/
void hardware_driver::reset(uint32_t time_ms)
{
    gpio_put(LCD_PIN_RST, 0);
    sleep_ms(time_ms);
    gpio_put(LCD_PIN_RST, 1);
    sleep_ms(time_ms);
}

/**
 * @brief Write data to the display
 * @param command The command to send
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::writeData(uint8_t command, const uint8_t* data, size_t length)
{
    uint8_t mask = 0;

#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    // We will to a bit of tomfoolery to solve the issue of switching between 8 and 16 bits
    this->changeSPIbits(BITS_8);
    pio_spi_wait_idle(this->pio, this->sm);
    this->setSPIdataCommandPins(0, 0);
    pio_spi_transmit_8(this->pio, this->sm, command);
    if(length)
    {
        pio_spi_wait_idle(this->pio, this->sm);
        this->setSPIdataCommandPins(1, 0);
        for(size_t i = 0; i < length; i++)
            pio_spi_transmit_8(this->pio, this->sm, *data++);
    }
    pio_spi_wait_idle(this->pio, this->sm);
    this->changeSPIbits(BITS_16);
    this->setSPIdataCommandPins(1, 1);

#elif defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
    // configure the hw to 8 bit mode
    spi_set_format(this->config->spi.spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // set the display to write mode
    gpio_put(LCD_PIN_DC, 0);
    // send the command
    spi_write_blocking(this->config->spi.spi_instance, &command, 1);
    // send the data
    gpio_put(LCD_PIN_DC, 1);
    // if there is data to send, send it
    if (length)
        spi_write_blocking(this->config->spi.spi_instance, data, length);
    spi_set_format(this->config->spi.spi_instance, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    this->write8080(command, true, false);

    for (size_t i = 0; i < length; i++)
        this->write8080(data[i], false, false);
#endif
}

/**
 * @brief Write data mode to the display
 * @param command The command to send
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::setDataMode(uint8_t command)
{
#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    this->writeData(command, nullptr, 0);
    this->setSPIdataCommandPins(1, 0);

#elif defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
    gpio_put(LCD_PIN_DC, 0);
    spi_set_format(this->config->spi.spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    spi_write_blocking(this->config->spi.spi_instance, &command, sizeof(command));
    gpio_put(LCD_PIN_DC, 1);
    spi_set_format(this->config->spi.spi_instance, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    this->write8080(command, true, false);
#endif
}

/**
 * @brief Write data to the display
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::writePixels(const uint16_t* data, size_t length)
{
#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    while(length--)
        pio_spi_transmit_16(this->pio, this->sm, *data++);

#elif defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
    spi_write16_blocking(this->config->spi.spi_instance, data, length);

#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    for (size_t i = 0; i < length; i++)
        this->write8080(data[i], false, true);
#endif
}

/**
 * @private
 * @brief Change the hw bit length
 * @param bits The bit length to change to
*/
void hardware_driver::changeSPIbits(spi_bit_length_t bits)
{
#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    // Stop the state machine
    pio_sm_set_enabled(this->pio, this->sm, false);
    // Remove the program
    pio_remove_program(this->pio, &pio_spi_program, this->offset);
    
    // Re-add the program
    this->offset = pio_add_program(this->pio, &pio_spi_program);
    // Re-initialize the state machine
    pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_SDA, 
        LCD_PIN_SCL, this->clkdiv, (int)bits);
#endif
}

/**
 * @private
 * @brief Write a byte to the 8080 interface
 * @param data The data to write
 * @param command True if the byte is a command, false if it is data
*/
void hardware_driver::write8080(uint32_t data, bool command, bool bit16)
{
#if defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    gpio_put(LCD_PIN_DC, !command);
    gpio_put(LCD_PIN_WR, 0);

#if defined(LCD_PIN_DB_SEQUENTIAL)
    uint32_t _data = data;

    if (this->parallel_interface_reverse_order && !bit16)
    {
        _data = ((_data & 0xaa) >> 1) | ((_data & 0x55) << 1);
        _data = ((_data & 0xcc) >> 2) | ((_data & 0x33) << 2);
        _data = (_data >> 4) | (_data << 4);
        _data <<= 8;
    }

    uint32_t mask = _data << this->parallel_interface_min_pin;
    gpio_put_masked(this->parallel_interface_mask, mask);
    gpio_put(LCD_PIN_WR, 1);
#else
    size_t end = bit16 ? 16 : 8;

    for (size_t i = 0; i < end; ++i)
        gpio_put(this->parallel_data_pins[i], (data >> i) & 1);

    gpio_put(LCD_PIN_WR, 1);
#endif
#endif
}

/**
 * @private
 * @brief Set the data/command pin and/or the chip select pin
 * @param dc The data/command pin
 * @param cs The chip select pin
*/
inline void hardware_driver::setSPIdataCommandPins(bool dc, bool cs)
{
#if defined(LCD_PROTOCOL_SPI)
    sleep_us(1);
    gpio_put_masked((1u << LCD_PIN_DC) | (1u << LCD_PIN_CS), 
        !!dc << LCD_PIN_DC | !!cs << LCD_PIN_CS);
    sleep_us(1);
#endif
}