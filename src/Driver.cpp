#include "Driver.hpp"

/**
 * @brief Constructor for Driver
 * @param pins The struct containing the pins to use
 * @param hw_params The struct containing the hardware parameters to use
 * @param spi The SPI bus to use, can be ignored if using DMA or PIO
 * @note If SPI is in use, and the instance is not specified, spi0 will be used
*/
Driver::Driver(display_config_t* config)
{
    this->config = config;
    this->interface = config->interface;
}

/**
 * @brief Initialize the hardware interface
*/
void Driver::init()
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
        case display_interface_t::DISPLAY_PISS:
            this->initPiss();
            break;
        case display_interface_t::DISPLAY_RGB:
            this->initRGB();
            break;
    }
}

/**
 * @brief Write data to the display
 * @param command The command to send
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void Driver::writeData(uint8_t command, const uint8_t* data, size_t length)
{
    // These displays don't operate with internal drivers with buffers
    if(this->interface == display_interface_t::DISPLAY_PISS 
        || this->interface == display_interface_t::DISPLAY_RGB)
        return;

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
        // configure the spi to 8 bit mode
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
}

/**
 * @brief Write data mode to the display
 * @param command The command to send
 * @return bytes written on success, -1 on failure
*/
void Driver::setDataMode(uint8_t command)
{
    // These displays don't operate with internal drivers with buffers
    if(this->interface == display_interface_t::DISPLAY_PISS 
        || this->interface == display_interface_t::DISPLAY_RGB)
        return;

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
}

/**
 * @brief Write data to the display
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void Driver::writePixels(const uint16_t* data, size_t length)
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
        case display_interface_t::DISPLAY_PISS:
            break;
        case display_interface_t::DISPLAY_RGB:
            break;
    }
}

/**
 * @private
 * @brief Initialize the SPI bus
*/
void Driver::initSPI(void)
{
    // enable the SPI bus
    spi_init(this->config->spi.spi_instance, this->config->spi.baudrate);

    // set the pins to SPI function
    gpio_set_function(this->config->spi.sda, GPIO_FUNC_SPI);
    gpio_set_function(this->config->spi.scl, GPIO_FUNC_SPI);
    gpio_set_function(this->config->spi.cs, GPIO_FUNC_SPI);

    gpio_init(this->config->spi.dc);
    gpio_set_dir(this->config->spi.dc, GPIO_OUT);
    gpio_put(this->config->spi.dc, 1);
}

/**
 * @private
 * @brief Initialize the SPI bus with PIO
*/
void Driver::initSPIwPIO(void)
{
    // create the first spi state machine
    this->pio = pio0;
    this->sm = pio_claim_unused_sm(this->pio, true);
    this->offset = pio_add_program(this->pio, &pio_spi_program);
    this->clkdiv = (float)clock_get_hz(clk_sys) / (float)this->config->spi.baudrate;
    pio_spi_init(this->pio, this->sm, this->offset, this->config->spi.sda, 
        this->config->spi.scl, this->clkdiv, (int)BITS_8);

    // set the pins to SPI function
    gpio_init(this->config->spi.cs);
    gpio_init(this->config->spi.dc);
    gpio_set_dir(this->config->spi.cs, GPIO_OUT);
    gpio_set_dir(this->config->spi.dc, GPIO_OUT);
    gpio_put(this->config->spi.cs, 1);
    gpio_put(this->config->spi.dc, 1);
}

void Driver::initPiss()
{

}

void Driver::initRGB()
{

}

void Driver::changeSPIbits(spi_bit_length_t bits)
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
 * @brief Set the data/command pin and/or the chip select pin
 * @param dc The data/command pin
 * @param cs The chip select pin
*/
inline void Driver::setSPIdataCommandPins(bool dc, bool cs)
{
    sleep_us(1);
    gpio_put_masked((1u << this->config->spi.dc) | (1u << this->config->spi.cs), 
        !!dc << this->config->spi.dc | !!cs << this->config->spi.cs);
    sleep_us(1);
}