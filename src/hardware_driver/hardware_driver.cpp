#include "hardware_driver.hpp"

/**
 * @brief Constructor for hardware_driver
 * @param config The struct containing the display configuration
*/
hardware_driver::hardware_driver(display_config_t* config)
{
    this->config = config;
    this->interface = config->interface;
}

/**
 * @brief Initialize the hardware interface
*/
void hardware_driver::init()
{
    switch(this->interface)
    {
        case display_interface_t::DISPLAY_SPI:
            if(this->config->spi.pio)
            {
                this->initSPIwPIO();
                this->pioMode = true;
            }
            else
            {
                this->initSPI();
            }
            break;

        case display_interface_t::DISPLAY_8080:
            this->init8080();
            break;

        default:
            printf("Unknown display interface: %d\n", this->interface);
            return;
    }
}

/**
 * @brief Reset the display using the reset signal
 * @param time_ms The time to hold the reset signal low in milliseconds on both flanks
*/
void hardware_driver::reset(uint32_t time_ms)
{
    switch(this->interface)
    {
        case display_interface_t::DISPLAY_SPI:
            gpio_put(this->config->spi.rst, 0);
            sleep_ms(time_ms);
            gpio_put(this->config->spi.rst, 1);
            sleep_ms(time_ms);
            break;

        case display_interface_t::DISPLAY_8080:
            gpio_put(this->config->b8080.rst, 0);
            sleep_ms(time_ms);
            gpio_put(this->config->b8080.rst, 1);
            sleep_ms(time_ms);
            break;

        default:
            printf("Unknown display interface: %d\n", this->interface);
            return;
    }
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

    switch (this->interface)
    {
        case display_interface_t::DISPLAY_SPI:
            if(this->pioMode)
            {
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
            }
            else
            {
                // configure the hw to 8 bit mode
                spi_set_format(this->config->spi.spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
                // set the display to write mode
                gpio_put(this->config->spi.dc, 0);
                // send the command
                spi_write_blocking(this->config->spi.spi_instance, &command, 1);
                // send the data
                gpio_put(this->config->spi.dc, 1);
                // if there is data to send, send it
                if (length)
                    spi_write_blocking(this->config->spi.spi_instance, data, length);
                spi_set_format(this->config->spi.spi_instance, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            }
            break;

        case display_interface_t::DISPLAY_8080:
            this->write8080(command, true, false);

            for (size_t i = 0; i < length; i++)
                this->write8080(data[i], false, false);
            break;

        default:
            printf("Unknown display interface: %d (Command Function)\n", this->interface);
            return;
        }
}

/**
 * @brief Write data mode to the display
 * @param command The command to send
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::setDataMode(uint8_t command)
{
    // printf("CMD: %x\n", command);

    switch (this->interface)
    {
        case display_interface_t::DISPLAY_SPI:
            if(pioMode)
            {
                this->writeData(command, nullptr, 0);
                this->setSPIdataCommandPins(1, 0);
            }
            else
            {
                gpio_put(this->config->spi.dc, 0);
                spi_set_format(this->config->spi.spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
                spi_write_blocking(this->config->spi.spi_instance, &command, sizeof(command));
                gpio_put(this->config->spi.dc, 1);
                spi_set_format(this->config->spi.spi_instance, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
            }
            break;

        case display_interface_t::DISPLAY_8080:
            this->write8080(command, true, false);
            break;

        default:
            printf("Unknown display interface: %d (Command Function)\n", this->interface);
            return;
    }
}

/**
 * @brief Write data to the display
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::writePixels(const uint16_t* data, size_t length)
{
    switch(this->interface)
    {
        case display_interface_t::DISPLAY_SPI:
            if(this->pioMode)
            {
                while(length--)
                    pio_spi_transmit_16(this->pio, this->sm, *data++);
            }
            else
            {
                // send the data
                spi_write16_blocking(this->config->spi.spi_instance, data, length);
            }
            break;

        case display_interface_t::DISPLAY_8080:
            for (size_t i = 0; i < length; i++)
                this->write8080(data[i], false, true);
            break;

        default:
            printf("Unknown display interface: %d (Pixel Function)\n", this->interface);
            return;
    }
    // printf("FINISHED WRITING %d PIXELS\n", length);
}

/**
 * @private
 * @brief Initialize the hw bus
*/
void hardware_driver::initSPI(void)
{
    // enable the hw bus
    spi_init(this->config->spi.spi_instance, this->config->spi.baudrate);

    // set the pins to hw function
    gpio_set_function(this->config->spi.sda, GPIO_FUNC_SPI);
    gpio_set_function(this->config->spi.scl, GPIO_FUNC_SPI);
    gpio_set_function(this->config->spi.cs, GPIO_FUNC_SPI);

    gpio_init(this->config->spi.dc);
    gpio_set_dir(this->config->spi.dc, GPIO_OUT);
    gpio_put(this->config->spi.dc, 1);
}

/**
 * @private
 * @brief Initialize the hw bus with PIO
*/
void hardware_driver::initSPIwPIO(void)
{
    // create the first hw state machine
    this->pio = pio0;
    this->sm = pio_claim_unused_sm(this->pio, true);
    this->offset = pio_add_program(this->pio, &pio_spi_program);
    this->clkdiv = (float)clock_get_hz(clk_sys) / (float)this->config->spi.baudrate;
    pio_spi_init(this->pio, this->sm, this->offset, this->config->spi.sda, 
        this->config->spi.scl, this->clkdiv, (int)BITS_8);

    // set the pins to hw function
    gpio_init(this->config->spi.cs);
    gpio_init(this->config->spi.dc);
    gpio_set_dir(this->config->spi.cs, GPIO_OUT);
    gpio_set_dir(this->config->spi.dc, GPIO_OUT);
    gpio_put(this->config->spi.cs, 1);
    gpio_put(this->config->spi.dc, 1);
}

/**
 * @private
 * @brief Initialize the 8080 interface
*/
void hardware_driver::init8080(void)
{
    // Set the pins to output
    gpio_init(this->config->b8080.rst);
    gpio_set_dir(this->config->b8080.rst, GPIO_OUT);
    gpio_put(this->config->b8080.rst, 1);
    gpio_pull_down(this->config->b8080.rst);

    gpio_init(this->config->b8080.csx);
    gpio_set_dir(this->config->b8080.csx, GPIO_OUT);
    gpio_put(this->config->b8080.csx, 0);
    gpio_pull_down(this->config->b8080.csx);

    gpio_init(this->config->b8080.dcx);
    gpio_set_dir(this->config->b8080.dcx, GPIO_OUT);
    gpio_put(this->config->b8080.dcx, 1);
    gpio_pull_down(this->config->b8080.dcx);

    gpio_init(this->config->b8080.rdx);
    gpio_set_dir(this->config->b8080.rdx, GPIO_OUT);
    gpio_put(this->config->b8080.rdx, 1);
    gpio_pull_down(this->config->b8080.rdx);

    gpio_init(this->config->b8080.wrx);
    gpio_set_dir(this->config->b8080.wrx, GPIO_OUT);
    gpio_put(this->config->b8080.wrx, 1);
    gpio_pull_down(this->config->b8080.wrx);

    gpio_init(this->config->b8080.im0);
    gpio_set_dir(this->config->b8080.im0, GPIO_OUT);
    gpio_put(this->config->b8080.im0, 1);
    gpio_pull_down(this->config->b8080.im0);

    this->parallel_interface_in_sequence = this->isSequential(this->config->b8080.db, this->config->b8080.db_size);

    if (this->parallel_interface_in_sequence)
    {
        // Set the data pins to output
        uint32_t start = this->config->b8080.db[0];
        uint32_t end = this->config->b8080.db[this->config->b8080.db_size - 1];

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

        for (size_t i = 0; i < this->config->b8080.db_size; i++)
            gpio_pull_down(this->config->b8080.db[i]);
    }
    else
    {
        for (size_t i = 0; i < this->config->b8080.db_size; i++)
        {
            uint32_t pin = this->config->b8080.db[i];
            printf("INIT DB%d (GPIO%d)\n", i, pin);
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_OUT);
            gpio_put(pin, 1);
            gpio_pull_down(pin);
        }
    }
}

/**
 * @private
 * @brief Change the hw bit length
 * @param bits The bit length to change to
*/
void hardware_driver::changeSPIbits(spi_bit_length_t bits)
{
    // Stop the state machine
    pio_sm_set_enabled(this->pio, this->sm, false);
    // Remove the program
    pio_remove_program(this->pio, &pio_spi_program, this->offset);
    
    // Re-add the program
    this->offset = pio_add_program(this->pio, &pio_spi_program);
    // Re-initialize the state machine
    pio_spi_init(this->pio, this->sm, this->offset, this->config->spi.sda, 
        this->config->spi.scl, this->clkdiv, (int)bits);
}

/**
 * @private
 * @brief Write a byte to the 8080 interface
 * @param data The data to write
 * @param command True if the byte is a command, false if it is data
*/
void hardware_driver::write8080(uint32_t data, bool command, bool bit16)
{
    gpio_put(this->config->b8080.dcx, !command);
    gpio_put(this->config->b8080.wrx, 0);

    if (this->parallel_interface_in_sequence)
    {
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
        gpio_put(this->config->b8080.wrx, 1);
    }
    else
    {
        size_t end = bit16 ? 16 : 8;

        for (size_t i = 0; i < end; ++i)
            gpio_put(this->config->b8080.db[i], (data >> i) & 1);

        gpio_put(this->config->b8080.wrx, 1);
    }
}

/**
 * @private
 * @brief Set the data/command pin and/or the chip select pin
 * @param dc The data/command pin
 * @param cs The chip select pin
*/
inline void hardware_driver::setSPIdataCommandPins(bool dc, bool cs)
{
    sleep_us(1);
    gpio_put_masked((1u << this->config->spi.dc) | (1u << this->config->spi.cs), 
        !!dc << this->config->spi.dc | !!cs << this->config->spi.cs);
    sleep_us(1);
}

/**
 * @private
 * @brief Check if the array is sequential
 * @param arr The array to check
 * @param size The size of the array
 * @return true if the array is sequential, false otherwise
*/
bool hardware_driver::isSequential(uint32_t* arr, size_t size)
{
    if (size < 2) return true; // An array with less than 2 elements is considered sequential

    int32_t forward = (arr[1] - arr[0]) & 31; // Use bitwise AND to handle wrap-around
    if (forward != 1 && forward != 31) return false; // Check for valid step

    for (size_t i = 0; i < size - 1; i++)
    {
        int32_t diff = (arr[i + 1] - arr[i]) & 31; // Use bitwise AND to handle wrap-around
        if (diff != forward) return false;
    }
    return true;
}