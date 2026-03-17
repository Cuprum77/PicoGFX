#include "driver.h"

/**
 * @brief Initialize the hardware interface
*/
void hardware_driver::init()
{
#if defined(LCD_PIN_RST)
    gpio_init(LCD_PIN_RST);
    gpio_set_dir(LCD_PIN_RST, GPIO_OUT);
#if defined(LCD_PIN_INV)
    gpio_put(LCD_PIN_RST, 0);
#else
    gpio_put(LCD_PIN_RST, 1);
#endif
#endif

    hardware_driver::protocol_init();
}

/**
 * @brief Reset the display using the reset signal
 * @param time_ms The time to hold the reset signal low in milliseconds on both flanks
*/
void hardware_driver::reset(uint32_t time_ms)
{
#if defined(LCD_PIN_RST)
#if defined(LCD_PIN_INV)
    gpio_put(LCD_PIN_RST, 1);
    sleep_ms(20);
    gpio_put(LCD_PIN_RST, 0);
    sleep_ms(time_ms);
#else
    gpio_put(LCD_PIN_RST, 0);
    sleep_ms(20);
    gpio_put(LCD_PIN_RST, 1);
    sleep_ms(time_ms);
#endif
#endif
}

/**
 * @brief Write data to the display
 * @param command The command to send
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::writeData(uint8_t command, const uint8_t *data, size_t length)
{
    this->protocol_write_data(command, data, length);
}

/**
 * @brief Write data mode to the display
 * @param command The command to send
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::setDataMode(uint8_t command)
{
    this->protocol_set_data_mode(command);
}

/** 
 * @brief Switch from slower to faster mode (e.g. SPI to QSPI) for pixel data transmission
 * @param data Whether to switch to data mode (true) or command mode (false)
 * @return void
 * @note Does absolutely nothing in most drivers!
 * @note But it is necessary for some protocols like QSPI that initializes in SPI
 */
void hardware_driver::switchTransmissionMode(bool data)
{
    this->speed_mode = data;
}
/**
 * @brief Write data to the display
 * @param data The data to send
 * @param length The length of the data
 * @return bytes written on success, -1 on failure
*/
void hardware_driver::writePixels(const color_t *data, size_t length)
{
    this->protocol_write_pixels(data, length);
}

#if defined(LCD_PROTOCOL_SPI) && defined(LCD_HARDWARE_PIO)
    inline void hardware_driver::protocol_init()
    {
        // create the first hw state machine
        this->pio = pio0;
        this->sm = pio_claim_unused_sm(this->pio, true);
        this->offset = pio_add_program(this->pio, &pio_spi_program);
        this->clkdiv = (float)clock_get_hz(clk_sys) / (float)LCD_BAUD_RATE;
        pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_SDA, 
            LCD_PIN_SCL, this->clkdiv, (int)8);

        // set the pins to hw function
        gpio_init(LCD_PIN_CS);
        gpio_init(LCD_PIN_DC);
        gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
        gpio_set_dir(LCD_PIN_DC, GPIO_OUT);
        gpio_put(LCD_PIN_CS, 1);
        gpio_put(LCD_PIN_DC, 1);
    }
    
    inline void hardware_driver::protocol_write_data(uint8_t command, const uint8_t *data, size_t length)
    {
        this->pio_set_bits(8);
        pio_spi_wait_idle(this->pio, this->sm);
        this->set_spi_dc_cs(0, 0);
        pio_spi_transmit_8(this->pio, this->sm, command);
        if(length)
        {
            pio_spi_wait_idle(this->pio, this->sm);
            this->set_spi_dc_cs(1, 0);
            for(size_t i = 0; i < length; i++)
                pio_spi_transmit_8(this->pio, this->sm, *data++);
        }
        pio_spi_wait_idle(this->pio, this->sm);
        this->set_spi_dc_cs(1, 1);
    }

    inline void hardware_driver::protocol_set_data_mode(uint8_t command)
    {
        this->protocol_write_data(command, nullptr, 0);
        this->set_spi_dc_cs(1, 0);
    }

    inline void hardware_driver::protocol_write_pixels(const color_t *data, size_t length)
    {
#if defined(LCD_COLOR_DEPTH_16)
        this->pio_set_bits(16);
        while(length--)
            pio_spi_transmit_16(this->pio, this->sm, *data++);
        pio_spi_wait_idle(this->pio, this->sm);
        this->set_spi_dc_cs(1, 1);

#elif defined(LCD_COLOR_DEPTH_18)
        this->pio_set_bits(8);

        const uint32_t *pixels = data;
        for (size_t i = 0; i < length; i++) 
        {
            uint8_t words[3] = {
                (uint8_t)((pixels[i] >> 12) & 0x3f) << 2,
                (uint8_t)((pixels[i] >>  6) & 0x3f) << 2,
                (uint8_t)((pixels[i] >>  0) & 0x3f) << 2,
            };
            
            for (uint32_t j = 0; j < 3; j++)
                pio_spi_transmit_8(this->pio, this->sm, words[j]);
        }
        pio_spi_wait_idle(this->pio, this->sm);
        this->set_spi_dc_cs(1, 1);

#elif defined(LCD_COLOR_DEPTH_24)
        this->pio_set_bits(24);
        while(length--)
            pio_spi_transmit_24(this->pio, this->sm, *data++);
        pio_spi_wait_idle(this->pio, this->sm);
        this->set_spi_dc_cs(1, 1);

#else
        this->pio_set_bits(8);
        while(length--)
            pio_spi_transmit_8(this->pio, this->sm, *(uint8_t *)data++);
        pio_spi_wait_idle(this->pio, this->sm);
        this->set_spi_dc_cs(1, 1);
#endif
    }

    inline void hardware_driver::pio_set_bits(uint32_t bits)
    {
        // Stop the state machine
        pio_sm_set_enabled(this->pio, this->sm, false);
        // Remove the program
        pio_remove_program(this->pio, &pio_spi_program, this->offset);
        
        // Re-add the program
        this->offset = pio_add_program(this->pio, &pio_spi_program);
        // Re-initialize the state machine
        pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_SDA, 
            LCD_PIN_SCL, this->clkdiv, (int)bits);
    }

    inline void hardware_driver::set_spi_dc_cs(bool dc, bool cs)
    {
        sleep_us(1);
        gpio_put_masked((1u << LCD_PIN_DC) | (1u << LCD_PIN_CS), 
            !!dc << LCD_PIN_DC | !!cs << LCD_PIN_CS);
        sleep_us(1);
    }

#elif defined(LCD_PROTOCOL_SPI) && !defined(LCD_HARDWARE_PIO)
    inline void hardware_driver::protocol_init()
    {
        spi_init(this->spi_instance, LCD_BAUD_RATE);

        // set the pins to hw function
        gpio_set_function(LCD_PIN_SDA, GPIO_FUNC_SPI);
        gpio_set_function(LCD_PIN_SCL, GPIO_FUNC_SPI);
        
        gpio_init(LCD_PIN_CS);
        gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
        gpio_put(LCD_PIN_CS, 1);

        gpio_init(LCD_PIN_DC);
        gpio_set_dir(LCD_PIN_DC, GPIO_OUT);
        gpio_put(LCD_PIN_DC, 1);
    }
    
    inline void hardware_driver::protocol_write_data(uint8_t command, const uint8_t *data, size_t length)
    {
        // configure the hw to 8 bit mode
        spi_set_format(this->spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
        // set the display to write mode
        gpio_put(LCD_PIN_DC, 0);
        gpio_put(LCD_PIN_CS, 0);
        spi_write_blocking(this->spi_instance, &command, 1);
        // if there is data to send, send it
        if (length)
        {
            gpio_put(LCD_PIN_DC, 1);
            for (size_t i = 0; i < length; i++)
                spi_write_blocking(this->spi_instance, &data[i], 1);
        }
        gpio_put(LCD_PIN_DC, 1);
        gpio_put(LCD_PIN_CS, 1);
    }

    inline void hardware_driver::protocol_set_data_mode(uint8_t command)
    {
        this->protocol_write_data(command, nullptr, 0);
        gpio_put(LCD_PIN_CS, 0);
        gpio_put(LCD_PIN_DC, 1);
    }

    inline void hardware_driver::protocol_write_pixels(const color_t *data, size_t length)
    {
#if defined(LCD_COLOR_DEPTH_16)
    spi_set_format(this->spi_instance, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(LCD_PIN_CS, 0);
    spi_write16_blocking(this->spi_instance, (const uint16_t *)data, length);
    gpio_put(LCD_PIN_CS, 1);
#elif defined(LCD_COLOR_DEPTH_18)
    spi_set_format(this->spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(LCD_PIN_CS, 0);
    const uint32_t *pixels = data;
    for (size_t i = 0; i < length; i++) 
    {
        uint8_t words[3] = {
            (uint8_t)((pixels[i] >> 12) & 0x3f) << 2,
            (uint8_t)((pixels[i] >>  6) & 0x3f) << 2,
            (uint8_t)((pixels[i] >>  0) & 0x3f) << 2,
        };
        
        spi_write_blocking(this->spi_instance, words, 3);
    }
    gpio_put(LCD_PIN_CS, 1);
#elif defined(LCD_COLOR_DEPTH_24)
    spi_set_format(this->spi_instance, 12, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    gpio_put(LCD_PIN_CS, 0);
    const uint32_t *pixels = data;
    for (size_t i = 0; i < length; i++) 
    {
        uint16_t words[2] = {
            (uint16_t)((pixels[i] >> 12) & 0xfff),
            (uint16_t)( pixels[i]        & 0xfff)
        };

        spi_write16_blocking(this->spi_instance, words, 2);
    }
    gpio_put(LCD_PIN_CS, 1);
#else
    spi_set_format(this->spi_instance, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    
    gpio_put(LCD_PIN_CS, 0);
    spi_write_blocking(this->spi_instance, (const uint8_t *)data, length);
    gpio_put(LCD_PIN_CS, 1);
#endif
    }

#elif defined(LCD_PROTOCOL_QSPI) 
    inline void hardware_driver::protocol_init()
    {
        // set the pins to hw function
        gpio_init(LCD_PIN_CS);
        gpio_set_dir(LCD_PIN_CS, GPIO_OUT);
        gpio_put(LCD_PIN_CS, 1);
        this->reset(100);

#if defined(LCD_HARDWARE_PIO)
        // create the first hw state machine
        if (!this->speed_mode)
        {
            this->pio = pio0;
            this->sm = pio_claim_unused_sm(this->pio, true);
            this->offset = pio_add_program(this->pio, &pio_spi_program);
            this->clkdiv = (float)clock_get_hz(clk_sys) / (float)LCD_BAUD_RATE;
            pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT0, 
                LCD_PIN_SCL, this->clkdiv, (int)8);
        }
        else
        {
            this->pio = pio0;
            this->sm = pio_claim_unused_sm(this->pio, true);
            this->offset = pio_add_program(this->pio, &pio_qspi_program);
            this->clkdiv = (float)clock_get_hz(clk_sys) / (float)LCD_BAUD_RATE;
#if defined(LCD_PIN_DAT_REVERSED)
            pio_qspi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT3,
                LCD_PIN_SCL, this->clkdiv, (int)8);
#else
            pio_qspi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT0,
                LCD_PIN_SCL, this->clkdiv, (int)8);
#endif
        }

#elif defined(LCD_HARDWARE_GPIO)
#error "GPIO-based QSPI is not supported yet"
#endif
    }
    
    inline void hardware_driver::protocol_write_data(uint8_t command, const uint8_t *data, size_t length, bool keep_cs)
    {        
#if defined(LCD_HARDWARE_PIO)
        if (!this->speed_mode)
        {
            this->pio_set_bits(8);
            pio_spi_wait_idle(this->pio, this->sm);
            gpio_put(LCD_PIN_CS, 0);
            int header = command == 0x2c ? 0x32 : 0x02;
            pio_spi_transmit_8(this->pio, this->sm, header);
            pio_spi_transmit_8(this->pio, this->sm, 0x00);
            pio_spi_transmit_8(this->pio, this->sm, command);
            pio_spi_transmit_8(this->pio, this->sm, 0x00);

            if(length)
            {
                for(size_t i = 0; i < length; i++)
                    pio_spi_transmit_8(this->pio, this->sm, *data++);
            }
            pio_spi_wait_idle(this->pio, this->sm);

            // Force the data pins to 0 to ensure that the command is sent correctly
            pio_sm_set_pins_with_mask(this->pio, this->sm, 0, (0xf << LCD_PIN_DAT0));
        }
        else
        {            
            gpio_put(LCD_PIN_CS, 0);
            this->send_command_over_spi(0x12);

            // Address bytes
            this->pio_set_bits(8);
            pio_qspi_transmit_8(this->pio, this->sm, 0x00);
            pio_qspi_transmit_8(this->pio, this->sm, command);
            pio_qspi_transmit_8(this->pio, this->sm, 0x00);
            pio_spi_wait_idle(this->pio, this->sm);
            if(length)
            {
                for(size_t i = 0; i < length; i++)
                    pio_qspi_transmit_8(this->pio, this->sm, *data++);
            }
            pio_qspi_wait_idle(this->pio, this->sm);

            // Force the data pins to 0 to ensure that the command is sent correctly
            pio_sm_set_pins_with_mask(this->pio, this->sm, 0, (0xf << LCD_PIN_DAT0));
        }
#elif defined(LCD_HARDWARE_GPIO)
        gpio_put(LCD_PIN_CS, 0);
#error "GPIO-based QSPI is not supported yet"
#endif
        if (!keep_cs)
            gpio_put(LCD_PIN_CS, 1);
    }

    inline void hardware_driver::protocol_set_data_mode(uint8_t command)
    {
        this->protocol_write_data(command, nullptr, 0, true);
        gpio_put(LCD_PIN_CS, 0);
    }

    inline void hardware_driver::protocol_write_pixels(const color_t *data, size_t length)
    {
#if defined(LCD_HARDWARE_PIO)
        this->pio_set_bits(LCD_COLOR_DEPTH);

        for (size_t i = 0; i < length; i++)
            pio_qspi_transmit(this->pio, this->sm, data[i], LCD_COLOR_DEPTH);

        // Force the data pins to 0 to ensure that the command is sent correctly
        pio_sm_set_pins_with_mask(this->pio, this->sm, 0, (0xf << LCD_PIN_DAT0));

#elif defined(LCD_HARDWARE_GPIO)
#endif
        gpio_put(LCD_PIN_CS, 1);

    }

    inline void hardware_driver::pio_set_bits(uint32_t bits)
    {
        if (!this->speed_mode)
        {
            // Stop the state machine
            pio_sm_set_enabled(this->pio, this->sm, false);
            // Remove the program and handle the case where the program was switched in the meantime
            pio_remove_program(this->pio, &pio_spi_program, this->offset);
            
            // Re-add the program
            this->offset = pio_add_program(this->pio, &pio_spi_program);
            // Re-initialize the state machine
            pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT0, 
                LCD_PIN_SCL, this->clkdiv, (int)bits);
        }
        else
        {
            // Stop the state machine
            pio_sm_set_enabled(this->pio, this->sm, false);
            // Remove the program and handle the case where the program was switched in the meantime
            pio_remove_program(this->pio, &pio_qspi_program, this->offset);
            
            // Re-add the program
            this->offset = pio_add_program(this->pio, &pio_qspi_program);
            // Re-initialize the state machine
#if defined(LCD_PIN_DAT_REVERSED)
            pio_qspi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT3,
                LCD_PIN_SCL, this->clkdiv, (int)bits);
#else
            pio_qspi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT0,
                LCD_PIN_SCL, this->clkdiv, (int)bits);
#endif
        }
    }

    inline void hardware_driver::send_command_over_spi(uint8_t command)
    {
        if (!this->speed_mode)
            return;
        pio_spi_wait_idle(this->pio, this->sm);

        // Stop the state machine
        pio_sm_set_enabled(this->pio, this->sm, false);
        // Remove the qspi program and handle the case where the program was switched in the meantime
        pio_remove_program(this->pio, &pio_qspi_program, this->offset);

        // Re-add the spi program
        this->offset = pio_add_program(this->pio, &pio_spi_program);
        // Re-initialize the state machine
        pio_spi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT0, 
            LCD_PIN_SCL, this->clkdiv, (int)8);

        pio_spi_transmit_8(this->pio, this->sm, command);
        pio_spi_wait_idle(this->pio, this->sm);

        // Stop the state machine
        pio_sm_set_enabled(this->pio, this->sm, false);
        // Remove the spi program and handle the case where the program was switched in the meantime
        pio_remove_program(this->pio, &pio_spi_program, this->offset);

        // Re-add the qspi program
        this->offset = pio_add_program(this->pio, &pio_qspi_program);
        // Re-initialize the state machine
#if defined(LCD_PIN_DAT_REVERSED)
            pio_qspi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT3,
                LCD_PIN_SCL, this->clkdiv, (int)8);
#else
            pio_qspi_init(this->pio, this->sm, this->offset, LCD_PIN_DAT0,
                LCD_PIN_SCL, this->clkdiv, (int)8);
#endif
    }

#elif defined(LCD_PROTOCOL_I2C) 
    inline void hardware_driver::protocol_init()
    {
#error "I2C hardware_driver::protocol is not supported yet"
    }
    
    inline void hardware_driver::protocol_write_data(uint8_t command, const uint8_t *data, size_t length)
    {
        
    }

    inline void hardware_driver::protocol_set_data_mode(uint8_t command)
    {
        
    }

    inline void hardware_driver::protocol_write_pixels(const color_t *data, size_t length)
    {

    }

#elif defined(LCD_PROTOCOL_PARALLEL_24) \
    || defined(LCD_PROTOCOL_PARALLEL_16) \
    || defined(LCD_PROTOCOL_PARALLEL_8)
    inline void hardware_driver::protocol_init()
    {
        this->reset(100);
        
    #if defined(LCD_HARDWARE_GPIO)
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

        // Figure out the biggest pin of the two
        uint32_t max_pin = (start > end) ? start : end;
        uint32_t min_pin = (start < end) ? start : end;

        this->parallel_interface_max_pin = max_pin;
        this->parallel_interface_min_pin = min_pin;

        // Use booth to create a mask for the data pins
        if (max_pin == 31)
            this->parallel_interface_mask = 0xffffffff - ((1u << min_pin) - 1);
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
            gpio_init(pin);
            gpio_set_dir(pin, GPIO_OUT);
            gpio_put(pin, 1);
            gpio_pull_down(pin);
        }
    #endif
    #elif defined(LCD_HARDWARE_RGB)
        // Enable the LCD if there is an enable pin
    #if defined(LCD_PIN_EN)
        gpio_init(LCD_PIN_EN);
        gpio_set_dir(LCD_PIN_EN, GPIO_OUT);
        gpio_put(LCD_PIN_EN, 1);
        gpio_pull_down(LCD_PIN_EN);
    #endif

        this->off_hsync = pio_add_program(this->pio, &hsync_program);
        this->off_vsync = pio_add_program(this->pio, &vsync_program);
        this->off_rgb   = pio_add_program(this->pio, &rgb_program);
        this->off_de    = pio_add_program(this->pio, &rgb_de_program);

        pio_rgb_init(this->pio, this->sm_hsync, this->sm_vsync, this->sm_rgb, this->sm_de,
            this->off_hsync, this->off_vsync, this->off_rgb, this->off_de, LCD_PIN_DB0, LCD_PIN_PCLK,
            LCD_PIN_HSYNC, LCD_PIN_VSYNC, LCD_PIN_DE
        );
    #endif
    }
    
    inline void hardware_driver::protocol_write_data(uint8_t command, const uint8_t *data, size_t length)
    {
    #if defined(LCD_HARDWARE_GPIO)
        this->write8080(command, true, false);

        for (size_t i = 0; i < length; i++)
            this->write8080(data[i], false, false);
    #elif defined(LCD_HARDWARE_RGB)
        // No need to write command/data for RGB interface, just write pixels
        return;
    #endif
    }

    inline void hardware_driver::protocol_set_data_mode(uint8_t command)
    {
        this->protocol_write_data(command, nullptr, 0);
    }

    inline void hardware_driver::protocol_write_pixels(const color_t *data, size_t length)
    {
        const color_t *pixels = data;

    #if defined(LCD_HARDWARE_RGB)
        if (length != LCD_WIDTH * LCD_HEIGHT)
            return;

        pio_rgb_set_width(this->pio, this->sm_hsync, this->sm_rgb, this->sm_de, LCD_WIDTH);
        pio_rgb_set_height(this->pio, this->sm_vsync, LCD_HEIGHT);

        for (int y = 0; y < LCD_HEIGHT; y++) 
        {
            for (int x = 0; x < LCD_WIDTH; x++) 
            {
                color_t pixel = data[x + y * LCD_WIDTH];
                pio_rgb_write(pio, sm_rgb, pixel);
            }

            // wait for line to finish before pushing next
            pio_rgb_wait_line(pio);
        }
    #else
        for (size_t i = 0; i < length; i++)
            this->write_parallel_pixels(pixels[i]);
    #endif
    }

#if defined(LCD_HARDWARE_GPIO)
    inline void hardware_driver::write8080data(uint8_t data, bool command)
    {
        gpio_put(LCD_PIN_DC, 1);
        gpio_put(LCD_PIN_WR, 0);

    #if defined(LCD_PIN_DB_SEQUENTIAL)
    #if defined(LCD_PIN_DB_REVERSED)
        data = ((data & 0xaa) >> 1) | ((data & 0x55) << 1);
        data = ((data & 0xcc) >> 2) | ((data & 0x33) << 2);
        data = (data >> 4) | (data << 4);
        data <<= 8;
    #endif

        uint32_t mask = data << this->parallel_interface_min_pin;
        gpio_put_masked(this->parallel_interface_mask, mask);
        gpio_put(LCD_PIN_WR, 1);
    #else
        size_t end = bit16 ? 16 : 8;

        for (size_t i = 0; i < end; ++i)
            gpio_put(this->parallel_data_pins[i], (data >> i) & 1);

        gpio_put(LCD_PIN_WR, 1);
    #endif
    }
#endif

    inline void hardware_driver::write_parallel_pixels(const color_t data)
    {
    #if defined(LCD_HARDWARE_GPIO)
        // This will be 1, 2, or 4 depending on the color depth
        size_t end = sizeof(data);
        end *= 8; // Convert to bits

        gpio_put(LCD_PIN_DC, 0);
        gpio_put(LCD_PIN_WR, 0);

    #if defined(LCD_PIN_DB_SEQUENTIAL)
        uint32_t mask = data << this->parallel_interface_min_pin;
        gpio_put_masked(this->parallel_interface_mask, mask);
        gpio_put(LCD_PIN_WR, 1);
    #else
        for (size_t i = 0; i < end; ++i)
            gpio_put(this->parallel_data_pins[i], (data >> i) & 1);

        gpio_put(LCD_PIN_WR, 1);
    #endif
    #endif
    }

#else
#error "Unsupported display hardware_driver::protocol or hardware interface"

#endif