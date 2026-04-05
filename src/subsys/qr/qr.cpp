#include "qr.h"

/**
 * @brief Construct a new qr_generator::qr_generator object
 * @param display_ptr Pointer to the display object, used to access the frame buffer and display properties
 */
qr_generator::qr_generator(display_obj *display_ptr)
{
    this->frameBuffer = display_ptr->getFrameBuffer();
    this->display_ptr = display_ptr;
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, center aligned and scaled to the maximum size that fits the screen
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 */
void qr_generator::generate(const char *data, qr_ecc_level ecc_lvl)
{
    uint32_t data_size = strlen(data);
    this->generate((const uint8_t *)data, data_size, ecc_lvl);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 */
void qr_generator::generate(const char *data, qr_ecc_level ecc_lvl, rect box)
{
    uint32_t data_size = strlen(data);
    this->generate((const uint8_t *)data, data_size, ecc_lvl, box.x(), box.y());
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box_start The top-left corner of the box area where the QR code should be rendered
 * @param box_end The bottom-right corner of the box area where the QR code should be rendered
 */
void qr_generator::generate(const char *data, qr_ecc_level ecc_lvl, 
        point box_start, point box_end)
{
    uint32_t data_size = strlen(data);
    this->generate((const uint8_t *)data, data_size, ecc_lvl, box_start, box_end);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, center aligned
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param scale How big the individual pixels should be, starting from 1 up to the max that fits the screen
 */
void qr_generator::generate(const char *data, qr_ecc_level ecc_lvl, uint32_t scale)
{
    uint32_t data_size = strlen(data);
    point center = this->display_ptr->getCenter();
    this->generate((const uint8_t *)data, data_size, ecc_lvl, center, 1, scale);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param location The center point where the QR code should be rendered on the display
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param min_version Minimum QR code version (1-40) to use, the generator will select the smallest version that can fit the data
 * @param scale How big the individual pixels should be, starting from 1 up to the max that fits the screen
 */
void qr_generator::generate(const char *data, qr_ecc_level ecc_lvl, 
    point location, uint32_t min_version, uint32_t scale)
{
    uint32_t data_size = strlen(data);
    this->generate((const uint8_t *)data, data_size, ecc_lvl, location, min_version, scale);
}

/**
 * @brief Generate a QR code from the input data and render it to the display, center aligned and scaled to the maximum size that fits the screen
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 */
void qr_generator::generate(const uint8_t *data, size_t data_size, qr_ecc_level ecc_lvl)
{
    uint32_t min_version = 1;
    uint32_t scale = 1;
    point location = this->display_ptr->getCenter();

    // Validate input parameters
    if (ecc_lvl > 3)
        return;
    if (data == nullptr || data_size == 0)
        return;

    uint32_t adj_version = this->get_adjusted_version(data_size, ecc_lvl, min_version);
    uint32_t size = this->qr_modules_size + (adj_version * 4);

    // Determine the maximum scale that can fit the QR code on the display
    uint32_t max_scale_x = this->display_ptr->getWidth() / size;
    uint32_t max_scale_y = this->display_ptr->getHeight() / size;
    scale = imin(max_scale_x, max_scale_y);

    if (scale == 0)
        return;
        
    this->generate(data, data_size, ecc_lvl, location, min_version, scale);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 */
void qr_generator::generate(const uint8_t *data, size_t data_size, qr_ecc_level ecc_lvl, rect box)
{
    this->generate((const uint8_t *)data, data_size, ecc_lvl, box.x(), box.y());
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box_start The top-left corner of the box area where the QR code should be rendered
 * @param box_end The bottom-right corner of the box area where the QR code should be rendered
 */
void qr_generator::generate(const uint8_t *data, size_t data_size, qr_ecc_level ecc_lvl, 
        point box_start, point box_end)
{
    uint32_t min_version = 1;
    uint32_t scale = 1;
    point center = rect(box_start, box_end).getCenter();

    // Attempt to scale the QR code to fit the box area
    uint32_t adj_version = this->get_adjusted_version(data_size, ecc_lvl, min_version);
    uint32_t size = this->qr_modules_size + (adj_version * 4);
    uint32_t max_scale_x = (box_end.x - box_start.x) / size;
    uint32_t max_scale_y = (box_end.y - box_start.y) / size;
    scale = imin(max_scale_x, max_scale_y);

    if (scale == 0)
        return;

    this->generate(data, data_size, ecc_lvl, center, min_version, scale);
}

/**
 * @brief Generate a QR code from the input data and render it to the display, center aligned
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param scale How big the individual pixels should be, starting from 1 up to the max that fits the screen
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param min_version Minimum QR code version (1-40) to use, the generator will select the smallest version that can fit the data
 */
void qr_generator::generate(const uint8_t *data, size_t data_size, 
    qr_ecc_level ecc_lvl, uint32_t scale)
{
    point center = this->display_ptr->getCenter();
    this->generate(data, data_size, ecc_lvl, center, 1, scale);
}

/**
 * @brief Generate a QR code from the input data and render it to the display
 * @param data Pointer to the input data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param location The center point where the QR code should be rendered on the display
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param min_version Minimum QR code version (1-40) to use, the generator will select the smallest version that can fit the data
 * @param scale How big the individual pixels should be, starting from 1 up to the max that fits the screen
 */
void qr_generator::generate(const uint8_t *data, size_t data_size, qr_ecc_level ecc_lvl, 
        point location, uint32_t min_version, uint32_t scale)
{
    // Validate input parameters
    if (min_version < 1 || min_version > 40)
        return;
    if (ecc_lvl > 3)
        return;
    if (scale == 0)
        return;
    if (data == nullptr || data_size == 0)
        return;

    uint32_t adj_version = this->get_adjusted_version(data_size, ecc_lvl, min_version);
    uint32_t size = this->qr_modules_size + (adj_version * 4);

    // Check that this does not render out of bounds
    if ((size * scale) > this->display_ptr->getWidth())
        return;
    if ((size * scale) > this->display_ptr->getHeight())
        return;

    bool mask[size * size] = { false };
    bool data_mask[size * size] = { false };
    bool buffer[size * size] = { false };

    uint32_t alignment_coordinates[46] = { 0 };
    uint32_t alignment_count = this->get_alignment_coordinates(adj_version, alignment_coordinates);

    // Dummy format pattern to reserve space for it in the mask
    this->create_dummy_format_pattern(size, buffer, mask);
    this->create_timing_pattern(size, buffer, mask);
    this->add_finder_patterns(size, buffer, mask);
    this->add_alignment_patterns(size, alignment_coordinates, alignment_count, buffer, mask);
    this->create_version_pattern(size, adj_version, buffer, mask);

    int success = this->encode_data(size, data, data_size, QR_MODE_BYTE, adj_version, ecc_lvl, data_mask, mask);
    if (success != 0)
        return;
    this->generate_data_mask(size, QR_MASK_TYPE_1, data_mask, mask);
    this->create_format_pattern(size, ecc_lvl, QR_MASK_TYPE_1, buffer, mask);

    // Add the masked data to the buffer
    for (size_t i = 0; i < size * size; i++)
    {
        if (!mask[i])
            buffer[i] = data_mask[i];
    }

    uint32_t x = location.x - (size * scale) / 2;
    uint32_t y = location.y - (size * scale) / 2;

    this->draw_qr_code(size, x, y, scale, buffer);
}

/**
 * @brief Generate a minimalistic QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 */
void qr_generator::generate_minimal(const char *data, qr_ecc_level ecc_lvl, rect box, uint32_t min_version)
{
    uint32_t data_size = strlen(data);
    this->generate_minimal((const uint8_t *)data, data_size, ecc_lvl, box, min_version);
}

/**
 * @brief Generate a minimalistic QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 */
void qr_generator::generate_minimal(const uint8_t *data, size_t data_size, 
    qr_ecc_level ecc_lvl, rect box, uint32_t min_version)
{
    if (min_version < 1 || min_version > 40)
        return;

    uint32_t scale = 1;
    point center = box.getCenter();

    // Attempt to scale the QR code to fit the box area
    uint32_t adj_version = this->get_adjusted_version(data_size, ecc_lvl, min_version);
    uint32_t size = this->qr_modules_size + (adj_version * 4);
    uint32_t max_scale_x = box.width() / size;
    uint32_t max_scale_y = box.height() / size;
    point location = box.getCenter();
    scale = imin(max_scale_x, max_scale_y);

    // Validate input parameters
    if (ecc_lvl > 3)
        return;
    if (data == nullptr || data_size == 0)
        return;
    if (scale == 0)
        return;

    // Check that this does not render out of bounds
    if ((size * scale) > this->display_ptr->getWidth())
        return;
    if ((size * scale) > this->display_ptr->getHeight())
        return;

    bool mask[size * size] = { false };
    bool buffer[size * size] = { false };

    uint32_t alignment_coordinates[46] = { 0 };
    uint32_t alignment_count = this->get_alignment_coordinates(adj_version, alignment_coordinates);

    // Dummy format pattern to reserve space for it in the mask
    this->create_dummy_format_pattern(size, buffer, mask);
    this->create_timing_pattern(size, buffer, mask);
    this->add_finder_patterns(size, buffer, mask);
    this->add_alignment_patterns(size, alignment_coordinates, alignment_count, buffer, mask);
    this->create_version_pattern(size, adj_version, buffer, mask);

    int success = this->encode_data(size, data, data_size, QR_MODE_BYTE, adj_version, ecc_lvl, buffer, mask);
    if (success != 0)
        return;
    this->generate_data_mask(size, QR_MASK_TYPE_1, buffer, mask);
    this->create_format_pattern(size, ecc_lvl, QR_MASK_TYPE_1, buffer, mask);
    this->remove_mask_format_pattern(size, mask);

    uint32_t x = location.x - (size * scale) / 2;
    uint32_t y = location.y - (size * scale) / 2;
    this->draw_minimal_qr_code(size, x, y, scale, buffer, mask);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @param artistic_bitmap A bitmap to use for the QR code pixels, must be the same size as the box area
 */
void qr_generator::generate_artistic(const char *data, qr_ecc_level ecc_lvl, 
    rect box, const uint32_t *artistic_bitmap, uint32_t min_version)
{
    uint32_t data_size = strlen(data);
    this->generate_artistic((const uint8_t *)data, data_size, ecc_lvl, box, artistic_bitmap, min_version);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @param artistic_bitmap A bitmap to use for the QR code pixels, must be the same size as the box area
 */
void qr_generator::generate_artistic(const uint8_t *data, size_t data_size, 
    qr_ecc_level ecc_lvl, rect box, const uint32_t *artistic_bitmap, uint32_t min_version)
{
    if (min_version < 1 || min_version > 40)
        return;
        
    uint32_t scale = 1;
    point center = box.getCenter();

    // Attempt to scale the QR code to fit the box area
    uint32_t adj_version = this->get_adjusted_version(data_size, ecc_lvl, min_version);
    uint32_t size = this->qr_modules_size + (adj_version * 4);
    uint32_t max_scale_x = box.width() / size;
    uint32_t max_scale_y = box.height() / size;
    point location = box.getCenter();
    scale = imin(max_scale_x, max_scale_y);

    // Validate input parameters
    if (ecc_lvl > 3)
        return;
    if (data == nullptr || data_size == 0)
        return;
    if (scale == 0)
        return;

    // Check that this does not render out of bounds
    if ((size * scale) > this->display_ptr->getWidth())
        return;
    if ((size * scale) > this->display_ptr->getHeight())
        return;

    bool mask[size * size] = { false };
    bool buffer[size * size] = { false };

    uint32_t alignment_coordinates[46] = { 0 };
    uint32_t alignment_count = this->get_alignment_coordinates(adj_version, alignment_coordinates);

    // Dummy format pattern to reserve space for it in the mask
    this->create_dummy_format_pattern(size, buffer, mask);
    this->create_timing_pattern(size, buffer, mask);
    this->add_finder_patterns(size, buffer, mask);
    this->add_alignment_patterns(size, alignment_coordinates, alignment_count, buffer, mask);
    this->create_version_pattern(size, adj_version, buffer, mask);

    int success = this->encode_data(size, data, data_size, QR_MODE_BYTE, adj_version, ecc_lvl, buffer, mask);
    if (success != 0)
        return;
    this->generate_data_mask(size, QR_MASK_TYPE_1, buffer, mask);
    this->create_format_pattern(size, ecc_lvl, QR_MASK_TYPE_1, buffer, mask);
    this->remove_mask_format_pattern(size, mask);

    uint32_t x = location.x - (size * scale) / 2;
    uint32_t y = location.y - (size * scale) / 2;
    this->draw_artistic_qr_code(size, x, y, scale, buffer, mask, artistic_bitmap, box.width(), box.height());
}

/**
 * @private
 * @brief Get the adjusted QR code version that can fit the input data size and error correction
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3)
 * @param min_version Minimum QR code version (1-40) to use
 * @return The adjusted QR code version (0-39)
 */
uint32_t qr_generator::get_adjusted_version(size_t data_size, qr_ecc_level ecc_lvl, uint32_t min_version)
{
    // Adjust version to be 0-indexed for array access
    min_version = min_version - 1;

    uint32_t adj_version = 0;
    for (size_t i = min_version; i < 40; i++)
    {
        // Subtract the ECC code words from the total capacity
        uint32_t capacity = qr_version_words[i] - qr_ecc_words[i][ecc_lvl];

        if (data_size < capacity)
        {
            adj_version = i;
            break;
        }
    }

    return adj_version;
}

/**
 * @private
 * @brief Multiply two numbers in the Galois Field GF(256) using the precomputed exponent and logarithm tables
 * @param a The first number
 * @param b The second number
 * @return The product of a and b in GF(256)
 */
uint8_t qr_generator::gf_mul(uint8_t a, uint8_t b)
{
    if (a == 0 || b == 0) 
        return 0;

    return gf_exp[gf_log[a] + gf_log[b]];
}

/**
 * @private
 * @brief Get the generator polynomial coefficients for a given degree
 * @param degree The degree of the generator polynomial (number of ECC codewords)
 * @param generator The buffer to write the generator coefficients to (must be at least degree + 1 bytes)
 */
void qr_generator::get_generator(uint32_t degree, uint8_t *generator)
{
    generator[0] = 1;
    int gen_len = 1;

    for (int i = 0; i < degree; i++) 
    {
        uint8_t next_gen[256] = {0};
        for (int j = 0; j < gen_len; j++) 
        {
            next_gen[j] ^= generator[j];
            next_gen[j + 1] ^= this->gf_mul(generator[j], gf_exp[i]);
        }
        
        gen_len++;
        for (int j = 0; j < gen_len; j++)
            generator[j] = next_gen[j];
    }
}

/**
 * @private
 * @brief Calculate the Reed-Solomon error correction codewords for the input data
 * @param data The input data to be encoded
 * @param data_size The size of the input data in bytes
 * @param ecc_size The number of ECC codewords to generate
 * @param ecc The buffer to write the ECC codewords to
 */
void qr_generator::calculate_ecc(const uint8_t *data, size_t data_size, uint32_t ecc_size, uint8_t *ecc)
{
    uint8_t gen[256] = {0};
    get_generator(ecc_size, gen);

    uint8_t ecc_buf[QR_MAX_BUFFER_SIZE] = {0};
    
    // Safety check to prevent stack smash
    if (data_size + ecc_size > QR_MAX_BUFFER_SIZE)
        return;

    // Copy original message into the local array
    memcpy(ecc_buf, data, data_size);

    // Perform synthetic division
    for (int i = 0; i < data_size; i++)
    {
        uint8_t coef = ecc_buf[i];

        if (coef != 0) 
        {
            for (int j = 1; j <= ecc_size; j++)
                ecc_buf[i + j] ^= this->gf_mul(gen[j], coef);
        }
    }

    // Copy the remainder directly to the output buffer
    memcpy(ecc, &ecc_buf[data_size], ecc_size);
}

/**
 * @private
 * @brief Get the alignment coordinates
 * @param version The QR version number
 * @param coordinates The array of coordinates
 * @returns The number of coordinates
 */
uint32_t qr_generator::get_alignment_coordinates(uint32_t version, uint32_t *coordinates)
{
    if (version == 0)
        return 0;

    // Change it from zero-index to version-index
    version = version + 1;

    uint32_t intervals = version / 7 + 1;
    uint32_t distance = 4 * version + 4;
    uint32_t step = lround((double)distance / (double)intervals);

    // Round to the next even number
    step += step & 0b1;
    // First coordinate is always 6 
    coordinates[0] = 6;

    // Start right/bottom and go left/up by step * k
    for (size_t i = 1; i <= intervals; i++)
        coordinates[i] = 6 + distance - step * (intervals - i);

    return intervals + 1;
}

/**
 * @private
 * @brief Generate the data mask for the QR code
 * @param module_size The size of the QR code in modules
 * @param mask_type The mask type
 * @param data_mask The buffer to write the data mask to
 */
void qr_generator::generate_data_mask(uint32_t module_size, qr_mask_type mask_type, bool *data_mask, bool *mask)
{
    // Copy the mask to the data mask, while inverting the data bits according to the mask type
    for (size_t y = 0; y < module_size; y++)
    {
        for (size_t x = 0; x < module_size; x++)
        {
            size_t index = x + y * module_size;

            if (mask[index])
                continue;

            bool mask_bit = false;
            switch (mask_type)
            {
                case QR_MASK_TYPE_0:
                    mask_bit = ((y + x) % 2) == 0;
                    break;
                case QR_MASK_TYPE_1:
                    mask_bit = (y % 2) == 0;
                    break;
                case QR_MASK_TYPE_2:
                    mask_bit = (x % 3) == 0;
                    break;
                case QR_MASK_TYPE_3:
                    mask_bit = ((y + x) % 3) == 0;
                    break;
                case QR_MASK_TYPE_4:
                    mask_bit = (((y / 2) + (x / 3)) % 2) == 0;
                    break;
                case QR_MASK_TYPE_5:
                    mask_bit = ((y * x) % 2) + ((y * x) % 3) == 0;
                    break;
                case QR_MASK_TYPE_6:
                    mask_bit = (((y * x) % 2) + ((y * x) % 3)) % 2 == 0;
                    break;
                case QR_MASK_TYPE_7:
                    mask_bit = (((y + x) % 2) + ((y * x) % 3)) % 2 == 0;
                    break;
            }

            data_mask[index] = data_mask[index] ^ mask_bit;
        }
    }
}

/**
 * @private
 * @brief Place encoded data into the QR buffer using zigzag pattern
 * @param module_size The size of the QR code in modules
 * @param bit_buffer The buffer containing encoded data with error correction
 * @param buffer_size The size of the bit buffer in bytes
 * @param data_mask The buffer to write the data to
 * @param mask The mask indicating reserved areas
 */
void qr_generator::place_data_zigzag(uint32_t module_size, uint8_t *bit_buffer, uint32_t buffer_size,
        bool *data_mask, bool *mask)
{
    uint32_t bit_index = 0;
    uint32_t total_bits = buffer_size * 8;
    
    // Start from bottom-right, moving in 2-column strips upward
    // Alternate between moving right-to-left and left-to-right
    for (int col = module_size - 1; col >= 0; col -= 2)
    {
        // Skip the timing column (column 6)
        if (col == 6)
            col--;
        
        // Direction: account for skipped column 6 in the calculation
        int adjusted_col = col > 6 ? col : col + 1;
        bool moving_up = (((module_size - adjusted_col) / 2) % 2) == 0;
        
        if (moving_up)
        {
            // Moving upward: from bottom to top
            for (int row = module_size - 1; row >= 0; row--)
            {
                // Right column then left column of the pair
                for (int c = 0; c < 2; c++)
                {
                    int x = col - c;
                    int y = row;
                    
                    // Skip timing column 6
                    if (x == 6)
                        continue;
                    
                    size_t index = x + y * module_size;
                    
                    // Only place data where there's no mask (reserved area)
                    if (!mask[index] && bit_index < total_bits)
                    {
                        uint32_t byte_index = bit_index / 8;
                        uint32_t bit_pos = 7 - (bit_index % 8);
                        
                        data_mask[index] = (bit_buffer[byte_index] >> bit_pos) & 0x1;
                        bit_index++;
                    }
                }
            }
        }
        else
        {
            // Moving downward: from top to bottom
            for (int row = 0; row < module_size; row++)
            {
                // Right column then left column of the pair
                for (int c = 0; c < 2; c++)
                {
                    int x = col - c;
                    int y = row;
                    
                    // Skip timing column 6
                    if (x == 6)
                        continue;
                    
                    size_t index = x + y * module_size;
                    
                    // Only place data where there's no mask (reserved area)
                    if (!mask[index] && bit_index < total_bits)
                    {
                        uint32_t byte_index = bit_index / 8;
                        uint32_t bit_pos = 7 - (bit_index % 8);
                        
                        data_mask[index] = (bit_buffer[byte_index] >> bit_pos) & 0x1;
                        bit_index++;
                    }
                }
            }
        }
    }
}

/**
 * @private
 * @brief Encode the input data into the data mask
 * @param data The input data to be encoded
 * @param data_size The size of the input data in bytes
 * @param mode The encoding mode
 * @param data_mask The buffer to write the encoded data to
 * @param mask The mask buffer to check for reserved areas
 */
int qr_generator::encode_data(uint32_t module_size, const uint8_t *data, size_t data_size, qr_mode mode, 
        uint32_t version, qr_ecc_level ecc_lvl, bool *data_mask, bool *mask)
{
    // Segment mode (4 bits)
    uint32_t total_count = 4;

    // Determine the character count indicator bits based on the mode and version
    uint32_t char_count_bits = 0;

    if (mode == QR_MODE_NUMERIC)
        char_count_bits = (version < 10) ? 10 : (version < 27) ? 12 : 14;
    else if (mode == QR_MODE_ALPHANUMERIC)
        char_count_bits = (version < 10) ? 9 : (version < 27) ? 11 : 13;
    else if (mode == QR_MODE_BYTE)
        char_count_bits = (version < 10) ? 8 : 16;
    else if (mode == QR_MODE_KANJI)
        char_count_bits = (version < 10) ? 8 : (version < 27) ? 10 : 12;

    // Character count indicator (variable bits)
    total_count += char_count_bits;

    // Data encoding (variable bits)
    if (mode == QR_MODE_NUMERIC)
        total_count += (data_size / 3) * 10 + ((data_size % 3) == 1 ? 4 : (data_size % 3) == 2 ? 7 : 0);
    else if (mode == QR_MODE_ALPHANUMERIC)
        total_count += (data_size / 2) * 11 + ((data_size % 2) == 1 ? 6 : 0);
    else if (mode == QR_MODE_BYTE)
        total_count += data_size * 8;
    else if (mode == QR_MODE_KANJI)
        total_count += data_size * 13;

    // Check if the total count exceeds the capacity of the QR code
    uint32_t capacity = qr_version_words[version];
    uint32_t ecc_capacity = qr_ecc_words[version][ecc_lvl];
    if (total_count > capacity * 8)
        return -1;

    // Add the terminator bits (4 bits)
    total_count += 4;

    // Check if padding is necessary
    if (total_count % 8 != 0)
        total_count += 8 - (total_count % 8);

    uint8_t bit_buffer[capacity + 1] = { 0 };
    uint32_t bit_offset = 0;

    // Add the mode indicator to the bit buffer (4 bits) - placed in upper nibble of byte 0
    bit_buffer[0] = mode << 4;
    bit_offset = 4;

    // Add the character count indicator to the bit buffer
    for (size_t i = 0; i < char_count_bits; i++)
    {
        uint32_t byte_index = (4 + i) / 8;
        uint32_t bit_position = 7 - ((4 + i) % 8);
        bit_buffer[byte_index] |= ((data_size >> (char_count_bits - 1 - i)) & 0b1) << bit_position;
        bit_offset++;
    }

    // Add the data encoding to the bit buffer
    if (mode != QR_MODE_BYTE)
        return -1; // Only byte mode is implemented for now

    for (size_t i = 0; i < data_size; i++)
    {
        for (size_t bit = 0; bit < 8; bit++)
        {
            uint32_t byte_index = (4 + char_count_bits + (i * 8) + bit) / 8;
            uint32_t bit_position = 7 - ((4 + char_count_bits + (i * 8) + bit) % 8);
            bit_buffer[byte_index] |= ((data[i] >> (7 - bit)) & 0b1) << bit_position;
            bit_offset++;
        }
    }

    // Add the terminator bits (4 bits)
    bit_offset += 4;

    // If the data does not fill the entire capacity, add the pad code words
    uint32_t pad_index = 0;
    while (bit_offset < (capacity - ecc_capacity) * 8) // Leave 4 bits for the terminator
    {
        uint32_t byte_index = bit_offset / 8;
        uint32_t bit_position = 7 - (bit_offset % 8);
        
        uint8_t pad_byte = pad_code_words[(pad_index / 8) % 2];  // Alternate pad bytes every 8 bits
        uint32_t bit_in_pad = pad_index % 8;  // Which bit (0-7) in the pad byte

        bit_buffer[byte_index] |= ((pad_byte >> (7 - bit_in_pad)) & 0b1) << bit_position;
        bit_offset++;
        pad_index++;
    }

    // Apply Reed-Solomon error correction
    if (version < 2)
    {
        this->calculate_ecc(bit_buffer, (capacity - ecc_capacity), ecc_capacity, &bit_buffer[capacity - ecc_capacity]);
    }
    else
    {
        uint32_t total_blocks = qr_total_blocks[version][ecc_lvl];
        uint32_t ecc_per_block = ecc_capacity / total_blocks;
        uint32_t total_data_bytes = capacity - ecc_capacity;

        // Infer lengths using your integer operations
        uint32_t data_len1 = total_data_bytes / total_blocks;
        uint32_t num_blocks2 = total_data_bytes % total_blocks;
        uint32_t num_blocks1 = total_blocks - num_blocks2;
        uint32_t data_len2 = data_len1 + 1;

        // Create interleaved buffer: [block1_byte0, block2_byte0, ..., block1_byte1, block2_byte1, ...]
        uint8_t interleaved[QR_MAX_BUFFER_SIZE] = {0};
        uint32_t write_pos = 0;
        
        for (uint32_t byte_offset = 0; byte_offset < data_len2; byte_offset++)
        {
            for (uint32_t block_idx = 0; block_idx < total_blocks; block_idx++)
            {
                uint32_t block_len = (block_idx < num_blocks1) ? data_len1 : data_len2;
                if (byte_offset < block_len)
                {
                    // Source position in original sequential buffer
                    uint32_t source_pos = (block_idx * data_len1) + byte_offset;
                    interleaved[write_pos++] = bit_buffer[source_pos];
                }
            }
        }
        
        // Copy interleaved data back to bit_buffer
        memcpy(bit_buffer, interleaved, total_data_bytes);

        uint8_t current_block[QR_MAX_BLOCK_SIZE];
        uint8_t current_ecc[QR_MAX_BLOCK_SIZE];

        for (size_t block_idx = 0; block_idx < total_blocks; block_idx++) 
        {
            uint32_t current_len = (block_idx < num_blocks1) ? data_len1 : data_len2;
            
            // Extract the block from the now-interleaved buffer
            for (size_t byte_idx = 0; byte_idx < current_len; byte_idx++) 
            {
                uint32_t buffer_index = byte_idx * total_blocks + block_idx;
                current_block[byte_idx] = bit_buffer[buffer_index];
            }

            this->calculate_ecc(current_block, current_len, ecc_per_block, current_ecc);

            // Write the results back to the designated spots
            for (size_t j = 0; j < ecc_per_block; j++) 
            {
                uint32_t target_index = total_data_bytes + (j * total_blocks) + block_idx;
                bit_buffer[target_index] = current_ecc[j];
            }
        }
    }
    
    // Place data using zigzag pattern
    this->place_data_zigzag(module_size, bit_buffer, capacity, data_mask, mask);
    return 0;
}

/**
 * @private
 * @brief Draw the QR code to the display using the buffer
 * @param module_size The size of the QR code in modules
 * @param x The x coordinate to start drawing the QR code
 * @param y The y coordinate to start drawing the QR code
 * @param scale The scale of the QR code, how big the individual pixels should be
 * @param buffer The buffer containing the QR code data
 */
void qr_generator::draw_qr_code(uint32_t module_size, uint32_t x, uint32_t y, 
    uint32_t scale, bool *buffer)
{
    uint32_t frame_ptr = 0;

    // Y loop
    for (size_t iy = 0; iy < module_size; iy++)
    {
        // X loop
        for (size_t ix = 0; ix < module_size; ix++)
        {
            color_t bc = buffer[ix + iy * module_size] ? colors::black : colors::white;

            // Loop scaled pixel y
            for (size_t sy = 0; sy < scale; sy++)
            {
                // Loop scaled pixel x
                for (size_t sx = 0; sx < scale; sx++)
                {
                    frame_ptr = ((ix * scale) + sx + x) + 
                        (((iy * scale) + sy + y) * this->display_ptr->getWidth());

                    if (bc == colors::black)
                        this->frameBuffer[frame_ptr] = bc;
                }
            }
        }
    }
}

/**
 * @private
 * @brief Draw the QR code to the display using the buffer, but use an artistic bitmap for the non-essential details
 * @param module_size The size of the QR code in modules
 * @param x The x coordinate to start drawing the QR code
 * @param y The y coordinate to start drawing the QR code
 * @param scale The scale of the QR code, how big the individual pixels should be
 * @param buffer The buffer containing the QR code data
 * @param mask The buffer containing the mask for reserved areas
 * @param artistic_bitmap The bitmap to use for the QR code pixels
 * @param bitmap_width The width of the artistic bitmap in pixels
 * @param bitmap_height The height of the artistic bitmap in pixels
 */
void qr_generator::draw_minimal_qr_code(uint32_t module_size, uint32_t x, uint32_t y, 
        uint32_t scale, bool *buffer, bool* mask)
{
    uint32_t frame_ptr = 0;
    uint32_t disp_width = this->display_ptr->getWidth();
    uint32_t disp_height = this->display_ptr->getHeight();

    for (size_t iy = 0; iy < module_size; iy++)
    {
        for (size_t ix = 0; ix < module_size; ix++)
        {
            bool is_black_module = buffer[ix + iy * module_size];
            bool is_important_detail = mask[ix + iy * module_size];

            for (size_t sy = 0; sy < scale; sy++)
            {
                for (size_t sx = 0; sx < scale; sx++)
                {
                    frame_ptr = ((ix * scale) + sx + x) + 
                                (((iy * scale) + sy + y) * disp_width);

                    int screen_x = (ix * scale) + sx + x;
                    int screen_y = (iy * scale) + sy + y;

                    if (screen_x >= 0 && screen_x < (int)disp_width &&
                        screen_y >= 0 && screen_y < (int)disp_height)
                    {
                        if (is_important_detail)
                        {
                            this->frameBuffer[frame_ptr] = is_black_module ? colors::black : colors::white;
                        }
                        else
                        {
                            bool is_center_x = false;
                            bool is_center_y = false;

                            if (scale >= 6) {
                                uint32_t padding = scale / 3;
                                is_center_x = (sx >= padding) && (sx < scale - padding);
                                is_center_y = (sy >= padding) && (sy < scale - padding);
                            } 
                            else if (scale >= 4) {
                                uint32_t low = (scale / 2) - 1;
                                uint32_t high = scale / 2;
                                is_center_x = (sx == low || sx == high);
                                is_center_y = (sy == low || sy == high);
                            } 
                            else {
                                is_center_x = (sx == scale / 2);
                                is_center_y = (sy == scale / 2);
                            }

                            if (is_center_x && is_center_y)
                            {
                                if (is_black_module)
                                {
                                    this->frameBuffer[frame_ptr] = colors::black;
                                }
                                else
                                {
                                    uint32_t current_pixel = this->frameBuffer[frame_ptr];
                                    
                                    if (current_pixel < 0x808080) 
                                    {
                                        this->frameBuffer[frame_ptr] = colors::white;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @private
 * @brief Draw the QR code to the display using the buffer, but use an artistic bitmap for the non-essential details
 * @param module_size The size of the QR code in modules
 * @param x The x coordinate to start drawing the QR code
 * @param y The y coordinate to start drawing the QR code
 * @param scale The scale of the QR code, how big the individual pixels should be
 * @param buffer The buffer containing the QR code data
 * @param mask The buffer containing the mask for reserved areas
 * @param artistic_bitmap The bitmap to use for the QR code pixels
 * @param bitmap_width The width of the artistic bitmap in pixels
 * @param bitmap_height The height of the artistic bitmap in pixels
 */
void qr_generator::draw_artistic_qr_code(uint32_t module_size, uint32_t x, uint32_t y, 
        uint32_t scale, bool *buffer, bool* mask, 
        const uint32_t *artistic_bitmap, uint32_t bitmap_width, uint32_t bitmap_height)
{
    uint32_t frame_ptr = 0;
    const uint8_t* bitmap_bytes = (const uint8_t*)artistic_bitmap;

    uint32_t disp_width = this->display_ptr->getWidth();
    uint32_t disp_height = this->display_ptr->getHeight();

    for (size_t iy = 0; iy < module_size; iy++)
    {
        for (size_t ix = 0; ix < module_size; ix++)
        {
            bool is_black_module = buffer[ix + iy * module_size];
            bool is_important_detail = mask[ix + iy * module_size];

            for (size_t sy = 0; sy < scale; sy++)
            {
                for (size_t sx = 0; sx < scale; sx++)
                {
                    frame_ptr = ((ix * scale) + sx + x) + 
                                (((iy * scale) + sy + y) * disp_width);

                    int bx = (ix * scale) + sx;
                    int by = (iy * scale) + sy;

                    if ((bx + (int)x) >= 0 && (bx + (int)x) < (int)disp_width &&
                        (by + (int)y) >= 0 && (by + (int)y) < (int)disp_height &&
                        bx >= 0 && bx < (int)bitmap_width &&
                        by >= 0 && by < (int)bitmap_height)
                    {
                        uint32_t colorWord = 0;
                        for (int i = 0; i < 3; ++i)
                        {
                            uint8_t colorByte = bitmap_bytes[(by * bitmap_width + bx) * 3 + i];
                            colorWord |= (uint32_t)colorByte << ((2 - i) * 8);
                        }
                        color c = color(colorWord);
                        uint32_t img_pixel = c.toWord();

                        if (is_important_detail)
                        {
                            this->frameBuffer[frame_ptr] = is_black_module ? colors::black : colors::white;
                        }
                        else
                        {
                            bool is_center_x = false;
                            bool is_center_y = false;

                            if (scale >= 6) {
                                uint32_t padding = scale / 3;
                                is_center_x = (sx >= padding) && (sx < scale - padding);
                                is_center_y = (sy >= padding) && (sy < scale - padding);
                            } 
                            else if (scale >= 4) {
                                uint32_t low = (scale / 2) - 1;
                                uint32_t high = scale / 2;
                                is_center_x = (sx == low || sx == high);
                                is_center_y = (sy == low || sy == high);
                            } 
                            else {
                                is_center_x = (sx == scale / 2);
                                is_center_y = (sy == scale / 2);
                            }

                            if (is_center_x && is_center_y)
                            {
                                this->frameBuffer[frame_ptr] = is_black_module ? colors::black : colors::white;
                            }
                            else
                            {
                                this->frameBuffer[frame_ptr] = img_pixel;
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * @private
 * @brief Draw the fixed timing pattern of the QR code at the specified position
 */
void qr_generator::create_timing_pattern(uint32_t module_size, bool *buffer, bool *mask)
{
    // These are always at row 6 and column 6
    uint32_t offset = 6;
    uint32_t frame_ptr = 0;

    // Draw the timing pattern
    for (size_t i = 0; i < module_size; i++)
    {
        buffer[offset + i * module_size] = (i % 2) == 0;
        buffer[i + offset * module_size] = (i % 2) == 0;

        mask[offset + i * module_size] = true;
        mask[i + offset * module_size] = true;
    }
}

/**
 * @private
 * @brief Draw the finder pattern of the QR code at the specified position
 */
void qr_generator::create_finder_pattern(uint32_t module_size, uint32_t x, uint32_t y, 
        qr_finder_corner corner, bool *buffer, bool *mask)
{
    if ((uint32_t)corner > 2)
        return;

    uint32_t x_offset = (corner == QR_FINDER_TOP_RIGHT)   ? 1 : 0;
    uint32_t y_offset = (corner == QR_FINDER_BOTTOM_LEFT) ? 1 : 0;

    uint32_t buffer_offset = 0;
    uint32_t full_width = module_size + 8;
    for (size_t i = 0; i < 7; i++)
    {
        for (size_t j = 0; j < 7; j++)
        {
            buffer_offset = (j + x + x_offset) + ((i + y + y_offset) * full_width);
            buffer[buffer_offset] = this->finder_pattern[j + i * 7];
        }
    }

    // Create the mask for the finder pattern, which is 9x9 including the separator
    uint32_t mask_offset = 0;
    for (size_t i = 0; i < 8; i++)
    {
        for (size_t j = 0; j < 8; j++)
        {
            mask_offset = (j + x) + ((i + y) * full_width);
            mask[mask_offset] = true;
        }
    }
}

/**
 * @private
 * @brief Add the finder pattern to the QR code
 */
void qr_generator::add_finder_patterns(uint32_t module_size, bool *buffer, bool *mask)
{
    module_size = module_size - 8;

    this->create_finder_pattern(module_size, 0, 0, QR_FINDER_TOP_LEFT, buffer, mask);
    this->create_finder_pattern(module_size, module_size, 0, QR_FINDER_TOP_RIGHT, buffer, mask);
    this->create_finder_pattern(module_size, 0, module_size, QR_FINDER_BOTTOM_LEFT, buffer, mask);
}

/**
 * @private
 * @brief Create the format pattern of the QR code, which contains the error correction level and mask type information
 * @param module_size The size of the QR code in modules
 * @param ecc_lvl The error correction level (0-3)
 * @param mask_type The mask type (0-7)
 * @param buffer The buffer to write the format pattern to
 * @param mask The mask to write the format pattern to
 */
void qr_generator::create_format_pattern(uint32_t module_size, qr_ecc_level ecc_lvl, 
        qr_mask_type mask_type, bool *buffer, bool *mask)
{
    // Format information lookup table [ECC level (2 bits) | Mask type (3 bits)]
    // Generated using ISO/IEC 18004:2015 BCH polynomial (x^10 + x^8 + x^5 + x^4 + x^2 + x + 1)
    uint16_t format_info_table[32] = {
        0x77c4, 0x72f3, 0x7daa, 0x789d, 0x662f, 0x6318, 0x6c41, 0x6976,
        0x5412, 0x5125, 0x5e7c, 0x5b4b, 0x45f9, 0x40ce, 0x4f97, 0x4aa0,
        0x355f, 0x3068, 0x3f31, 0x3a06, 0x24b4, 0x2183, 0x2eda, 0x2bed,
        0x1689, 0x13be, 0x1ce7, 0x19d0, 0x0762, 0x0255, 0x0d0c, 0x083b
    };
    
    uint32_t format_info = ((ecc_lvl & 0b11) << 3) | (mask_type & 0b111);
    uint32_t value = format_info_table[format_info];

    // First 0-5 bits on the top-left finder
    for (size_t i = 0; i < 6; i++)
    {
        buffer[8 + i * module_size] = ((value >> i) & 0x1);
        mask[8 + i * module_size] = true;
    }

    // 6-7 bits on the top-left finder
    for (size_t i = 0; i < 2; i++)
    {
        buffer[8 + (i + 7) * module_size] = ((value >> (i + 6)) & 0x1);
        mask[8 + (i + 7) * module_size] = true;
    }

    // 8 bit on the top-left finder
    buffer[7 + 8 * module_size] = ((value >> 8) & 0x1);
    mask[7 + 8 * module_size] = true;

    // 9-14 bits on the top-left finder
    for (size_t i = 0; i < 6; i++)
    {
        buffer[(5 - i) + 8 * module_size] = ((value >> (i + 9)) & 0x1);
        mask[(5 - i) + 8 * module_size] = true;
    }

    // 0-7 bits on the top-right finder
    for (size_t i = 0; i < 8; i++)
    {
        buffer[(module_size - 8 + i) + (8 * module_size)] = ((value >> (7 - i)) & 0x1);
        mask[(module_size - 8 + i) + (8 * module_size)] = true;
    }

    // 8-14 bits on the bottom-left finder
    for (size_t i = 0; i < 7; i++)
    {
        buffer[8 + ((module_size - i - 1) * module_size)] = ((value >> (14 - i)) & 0x1);
        mask[8 + ((module_size - i - 1) * module_size)] = true;
    }

    // Dark module
    buffer[8 + (module_size - 8) * module_size] = true;
    mask[8 + (module_size - 8) * module_size] = true;
}

/**
 * @private
 * @brief Create the dummy format pattern of the QR code, which reserves space for the format information in the mask without actually encoding it
 * @param module_size The size of the QR code in modules
 * @param buffer The buffer to write the format pattern to
 * @param mask The mask to write the format pattern to
 */
void qr_generator::create_dummy_format_pattern(uint32_t module_size, bool *buffer, bool *mask)
{
    // First 0-5 bits on the top-left finder
    for (size_t i = 0; i < 6; i++)
    {
        buffer[8 + i * module_size] = false;
        mask[8 + i * module_size] = true;
    }

    // 6-7 bits on the top-left finder
    for (size_t i = 0; i < 2; i++)
    {
        buffer[8 + (i + 7) * module_size] = false;
        mask[8 + (i + 7) * module_size] = true;
    }

    // 8 bit on the top-left finder
    buffer[7 + 8 * module_size] = false;
    mask[7 + 8 * module_size] = true;

    // 9-14 bits on the top-left finder
    for (size_t i = 0; i < 6; i++)
    {
        buffer[(5 - i) + 8 * module_size] = false;
        mask[(5 - i) + 8 * module_size] = true;
    }

    // 0-7 bits on the top-right finder
    for (size_t i = 0; i < 8; i++)
    {
        buffer[(module_size - 8 + i) + (8 * module_size)] = false;
        mask[(module_size - 8 + i) + (8 * module_size)] = true;
    }

    // 8-14 bits on the bottom-left finder
    for (size_t i = 0; i < 7; i++)
    {
        buffer[8 + ((module_size - i - 1) * module_size)] = false;
        mask[8 + ((module_size - i - 1) * module_size)] = true;
    }

    // Dark module
    buffer[8 + (module_size - 8) * module_size] = true;
    mask[8 + (module_size - 8) * module_size] = true;
}

/**
 * @private
 * @brief Add the alignment pattern to the QR code
 */
void qr_generator::create_alignment_pattern(uint32_t module_size, uint32_t x, uint32_t y, bool *buffer, bool *mask)
{
    uint32_t buffer_offset = 0;

    // Loop y
    for (size_t i = 0; i < 5; i++)
    {
        // Loop x
        for (size_t j = 0; j < 5; j++)
        {
            buffer_offset = (j + x) + ((i + y) * module_size);
            buffer[buffer_offset] = this->alignment_pattern[j + i * 5];
            mask[buffer_offset] = true;
        }
    }
}

/**
 * @private
 * @brief Add the alignment pattern to the QR code
 */
void qr_generator::add_alignment_patterns(uint32_t module_size, uint32_t *coordinates, 
        uint32_t count, bool *buffer, bool *mask)
{
    if (count == 0)
        return;

    uint32_t x_coord = 0;
    uint32_t y_coord = 0;
    
    uint32_t alt_x = 0;
    uint32_t alt_y = 0;

    for (size_t i = 0; i < count; i++)
    {
        y_coord = coordinates[i];

        for (size_t j = 0; j < count; j++)
        {
            x_coord = coordinates[j];

            // Remove the coordinates that end in the positions of our finders
            if (x_coord == coordinates[0] && y_coord == coordinates[0])
                continue;
            if (x_coord == coordinates[count - 1] && y_coord == coordinates[0])
                continue;
            if (x_coord == coordinates[0] && y_coord == coordinates[count - 1])
                continue;

            alt_x = x_coord - 2;
            alt_y = y_coord - 2;
            this->create_alignment_pattern(module_size, alt_x, alt_y, buffer, mask);
        }
    }
}

/**
 * @private
 * @brief Add the version pattern to the QR code
 */
void qr_generator::create_version_pattern(uint32_t module_size, uint32_t version, bool *buffer, bool *mask)
{
    if (version < 6)
        return;

    // Change from zero-index to normal version numbers
    version++;

    uint32_t frame_ptr = 0;
    uint32_t value = (version & 0x3f) << 12;
    uint32_t adj_x = (module_size - 8) - 3;
    uint32_t adj_y = (module_size - 8) - 3;

    // G(x) = x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
    uint32_t generator = 0x1f25;
    
    for (int i = 17; i >= 12; i--)
    {
        if ((value >> i) & 0x1)
            value ^= generator << (i - 12);
    }

    value = (version << 12) | (value & 0xfff);

    // value = 0b1 << 17;

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 6; j++)
        {
            uint32_t ptr = i + (j * 3);
            bool bit = ((value >> ptr) & 0x1);
            buffer[j + ((i + adj_y) * module_size)] = bit;
            mask[j + ((i + adj_y) * module_size)] = true;
            
            buffer[(i + adj_x) + (j * module_size)] = bit;
            mask[(i + adj_x) + (j * module_size)] = true;
        }
    }
}

/**
 * @private
 * @brief Remove the format pattern mask, which allows the information to be down sampled for artistic QR codes
 * @param module_size The size of the QR code in modules
 * @param mask The mask to remove the format pattern from
 */
void qr_generator::remove_mask_format_pattern(uint32_t module_size, bool *mask)
{
    // First 0-5 bits on the top-left finder
    for (size_t i = 0; i < 6; i++)
        mask[8 + i * module_size] = false;

    // 6-7 bits on the top-left finder
    for (size_t i = 0; i < 2; i++)
        mask[8 + (i + 7) * module_size] = false;

    // 8 bit on the top-left finder
    mask[7 + 8 * module_size] = true;

    // 9-14 bits on the top-left finder
    for (size_t i = 0; i < 6; i++)
        mask[(5 - i) + 8 * module_size] = false;

    // 0-7 bits on the top-right finder
    for (size_t i = 0; i < 8; i++)
        mask[(module_size - 8 + i) + (8 * module_size)] = false;

    // 8-14 bits on the bottom-left finder
    for (size_t i = 0; i < 7; i++)
        mask[8 + ((module_size - i - 1) * module_size)] = false;

    // Dark module
    mask[8 + (module_size - 8) * module_size] = false;
}