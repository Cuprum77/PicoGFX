#include "HardwareSPI.hpp"

/**
 * @brief Construct a new HardwareSPI::HardwareSPI object
 * @param spi The SPI instance to use
 * @param baudrate The baudrate to use
 * @param dc Data/Command pin
 * @param cs Chip select pin
 * @param scl Clock pin
 * @param sda Data pin
 * @param dma Use DMA for transfers, defaults to false
*/
HardwareSPI::HardwareSPI(spi_inst_t* spi, uint baudrate, uint8_t dc, uint8_t cs, uint8_t scl, uint8_t sda, bool dma)
{
    this->spi = spi;
    this->baudrate = baudrate;
    this->scl = scl;
    this->sda = sda;
    this->dc = dc;
    this->cs = cs;
    this->dma = dma;

    if(dma)
        this->initDMA();
    else
        this->initSPI();
}

/**
 * @brief Write data to the SPI bus
 * @param spi The SPI instance to use
 * @param command The command to send
 * @param data The data to send
 * @param length The length of the data
 * @param dma Use DMA for transfers, defaults to false
 * @return bytes written on success, -1 on failure
*/
int HardwareSPI::spi_write_data(uint8_t command, const uint8_t* data, size_t length)
{
    // create a counter to track the number of bytes written
    int counter = 0;

    // configure the spi to 8 bit mode
    spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    // set the display to write mode
    gpio_put(this->dc, 0);
    // send the command
    counter += spi_write_blocking(this->spi, &command, 1);
    // send the data
    gpio_put(this->dc, 1);
    // if there is data to send, send it
    if (length)
        counter += spi_write_blocking(this->spi, data, length);

    // return the number of bytes written
    return counter;
}

/**
 * @brief Write data mode to the SPI bus
 * @param spi The SPI instance to use
 * @param dc Data/Command pin
 * @param command The command to send
 * @param dma Use DMA for transfers, defaults to false
 * @return bytes written on success, -1 on failure
*/
int HardwareSPI::spi_set_data_mode(uint8_t command)
{
    // create a counter to track the number of bytes written
    int counter = 0;

    gpio_put(this->dc, 0);
    spi_set_format(this->spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    counter += spi_write_blocking(this->spi, &command, sizeof(command));
    gpio_put(this->dc, 1);
    spi_set_format(this->spi, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);

    // return the number of bytes written
    return counter;
}

/**
 * @brief Write data to the SPI bus
 * @param spi The SPI instance to use
 * @param data The data to send
 * @param length The length of the data
 * @param dma Use DMA for transfers, defaults to false
 * @return bytes written on success, -1 on failure
*/
int HardwareSPI::spi_write_pixels(const uint16_t* data, size_t length)
{
    // create a counter to track the number of bytes written
    int counter = 0;

    if(this->dma)
    {
        dma_channel_configure(
            this->dma_tx, // Channel to be configured
            &this->dma_config, // The configuration we just created
            &spi_get_hw(this->spi)->dr, // The write address
            data, // Pointer to the data we want to transmit
            length >> 1, // Number of bytes to transmit
            true // Start immediately
        );
    }
    else
    {
        // send the data
        counter += spi_write16_blocking(this->spi, data, length >> 1);
    }

    // return the number of bytes written
    return counter;
}

/**
 * @brief Check if the DMA is busy
 * @return true if busy, false if not
*/
bool HardwareSPI::dma_busy()
{
    if(!this->dma)
        return false;
    
    return dma_channel_is_busy(this->dma_tx);
}

/**
 * @private
 * @brief Initialize the SPI bus
*/
void HardwareSPI::initSPI()
{
    // enable the SPI bus
    spi_init(this->spi, this->baudrate);

    // set the pins to SPI function
    gpio_set_function(this->sda, GPIO_FUNC_SPI);
    gpio_set_function(this->scl, GPIO_FUNC_SPI);
    gpio_set_function(this->cs, GPIO_FUNC_SPI);

    gpio_init(this->dc);
    gpio_set_dir(this->dc, GPIO_OUT);
    gpio_put(this->dc, 1);
}

/**
 * @private
 * @brief Initialize the SPI bus with DMA
*/
void HardwareSPI::initDMA()
{
    // enable the SPI bus
    this->initSPI();

    // grab some unused DMA channels
    this->dma_tx = dma_claim_unused_channel(true);

    // setup the control channel
    this->dma_config = dma_channel_get_default_config(this->dma_tx);
    channel_config_set_transfer_data_size(&this->dma_config, DMA_SIZE_16); // 16 bit transfers (1/2 word)
    channel_config_set_dreq(&this->dma_config, DREQ_SPI0_TX); // SPI TX DREQ
    channel_config_set_read_increment(&this->dma_config, true); // increment the read address
    channel_config_set_write_increment(&this->dma_config, false); // don't increment the write address
}