#include "qr.h"

/**
 * @brief Construct a new qr_generator::qr_generator object
 * @param display_ptr Pointer to the display object, used to access the frame buffer and display properties
 */
qr_generator::qr_generator(display_obj *display_ptr)
{
    this->frameBuffer = display_ptr->getFrameBuffer();
    this->display_ptr = display_ptr;

    static qr_data_t qr_data = { 0 };
    this->qr_data = &qr_data;
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @return The rect area where the QR code was rendered on the display
 * @note Must run render_qr_code() after this to render the QR code to the display
 */
rect qr_generator::generate(const char *data, qr_ecc_level_t ecc_lvl, rect box, qr_version_t min_version)
{
    uint32_t data_size = strlen(data);
    return this->generate((const uint8_t *)data, data_size, ecc_lvl, box, min_version);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @return The rect area where the QR code was rendered on the display
 * @note Must run render_qr_code() after this to render the QR code to the display
 */
rect qr_generator::generate(const uint8_t *data, size_t data_size, 
    qr_ecc_level_t ecc_lvl, rect box, qr_version_t min_version)
{
    this->qr_data->data = data;
    this->qr_data->data_size = data_size;
    this->qr_data->ecc_lvl = ecc_lvl;
    this->qr_data->version = min_version;
    this->qr_data->box = box;

    return this->create_qr_code();
}

/**
 * @brief Generate a minimalistic QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @return The rect area where the QR code was rendered on the display
 * @note Must run render_qr_code() after this to render the QR code to the display
 */
rect qr_generator::generate_minimal(const char *data, qr_ecc_level_t ecc_lvl, rect box, qr_version_t min_version)
{
    uint32_t data_size = strlen(data);
    return this->generate_minimal((const uint8_t *)data, data_size, ecc_lvl, box, min_version);
}

/**
 * @brief Generate a minimalistic QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @return The rect area where the QR code was rendered on the display
 * @note Must run render_qr_code() after this to render the QR code to the display
 */
rect qr_generator::generate_minimal(const uint8_t *data, size_t data_size, 
    qr_ecc_level_t ecc_lvl, rect box, qr_version_t min_version)
{
    this->qr_data->data = data;
    this->qr_data->data_size = data_size;
    this->qr_data->ecc_lvl = ecc_lvl;
    this->qr_data->version = min_version;
    this->qr_data->box = box;
    this->qr_data->minimal = true;

    return this->create_qr_code();
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @param artistic_bitmap A bitmap to use for the QR code pixels, must be the same size as the box area
 * @return The rect area where the QR code was rendered on the display
 * @note Must run render_qr_code() after this to render the QR code to the display
 */
rect qr_generator::generate_artistic(const char *data, qr_ecc_level_t ecc_lvl, 
    rect box, const uint32_t *artistic_bitmap, qr_version_t min_version)
{
    uint32_t data_size = strlen(data);
    return this->generate_artistic((const uint8_t *)data, data_size, ecc_lvl, box, artistic_bitmap, min_version);
}

/**
 * @brief Generate a QR code from the input string data and render it to the display, within a specified box area
 * @param data Pointer to the null-terminated string data to be encoded in the QR code
 * @param data_size Size of the input data in bytes
 * @param ecc_lvl Error correction level (0-3), determines the amount of error correction in the QR code
 * @param box The box area where the QR code should be rendered 
 * @param artistic_bitmap A bitmap to use for the QR code pixels, must be the same size as the box area
 * @return The rect area where the QR code was rendered on the display
 * @note Must run render_qr_code() after this to render the QR code to the display
 */
rect qr_generator::generate_artistic(const uint8_t *data, size_t data_size, 
    qr_ecc_level_t ecc_lvl, rect box, const uint32_t *artistic_bitmap, qr_version_t min_version)
{        
    this->qr_data->data = data;
    this->qr_data->data_size = data_size;
    this->qr_data->ecc_lvl = ecc_lvl;
    this->qr_data->version = min_version;
    this->qr_data->box = box;
    this->qr_data->minimal = true;
    this->qr_data->bitmap = artistic_bitmap;
    this->qr_data->bitmap_width = box.width();
    this->qr_data->bitmap_height = box.height();

    return this->create_qr_code();
}

/**
 * @brief Render the generated QR code to the display
 */
void qr_generator::render_qr_code(void)
{
    if (this->qr_data->minimal)
    {
        if (this->qr_data->bitmap != nullptr)
            this->draw_artistic_qr_code();
        else
            this->draw_minimal_qr_code();   
    }
    else
    {
        this->draw_qr_code();
    }
}

/**
 * @private
 * @brief Create the QR code
 * @return The rect area where the QR code was rendered on the display
 */
rect qr_generator::create_qr_code()
{
    // Validate input parameters
    if (this->qr_data->data == nullptr || this->qr_data->data_size == 0)
        return rect();

    if (this->qr_data->ecc_lvl > QR_ECC_LVL_H)
        this->qr_data->ecc_lvl = QR_ECC_LVL_H;

    if (this->qr_data->version > QR_V40)
        this->qr_data->version = QR_V40;
    else if (this->qr_data->version < QR_M1)
        this->qr_data->version = QR_M1;

    this->get_adjusted_version();
    this->get_module_size();

    // Account for the quiet zone by adding modules of padding on each side, as per the QR code specification
    uint32_t quiet_zone = this->qr_data->minimal ? 0 : this->qr_data->version >= QR_V1 ? 4 : 2; // Micro QR codes have a smaller quiet zone
    this->qr_data->dummy_zone = quiet_zone;
    this->qr_data->qr_size = this->qr_data->module_size + (quiet_zone * 2);

    // Adjust the scale of the QR code to fit within the specified box area, while maintaining the aspect ratio
    uint32_t max_scale_x = this->qr_data->box.width() / this->qr_data->qr_size;
    uint32_t max_scale_y = this->qr_data->box.height() / this->qr_data->qr_size;
    this->qr_data->scale = imin(max_scale_x, max_scale_y);

    // Check that this does not render out of bounds
    if ((this->qr_data->qr_size * this->qr_data->scale) > this->display_ptr->getWidth())
        return rect();
    if ((this->qr_data->qr_size * this->qr_data->scale) > this->display_ptr->getHeight())
        return rect();

    // Calculate the top left corner of the QR code based on the center location and the size of the QR code
    this->qr_data->x = this->qr_data->box.getCenter().x - (this->qr_data->qr_size * this->qr_data->scale) / 2;
    this->qr_data->y = this->qr_data->box.getCenter().y - (this->qr_data->qr_size * this->qr_data->scale) / 2;

    this->qr_data->mask = this->mask;
    this->qr_data->buffer = this->buffer;

    uint32_t alignment_coordinates[46] = { 0 };
    this->qr_data->alignment_patterns = alignment_coordinates;
    this->get_alignment_coordinates();
    
    // We ignore this part of the spec if the QR code is minimal, as most readers are very forgiving
    if (!this->qr_data->minimal)
        this->mask_dummy_zone();

    // Dummy format pattern to reserve space for it in the mask
    this->create_dummy_format_pattern(true);
    this->create_timing_pattern();
    this->add_finder_patterns();
    this->add_alignment_patterns();
    this->create_version_pattern();

    int success = this->encode_data();
    if (success != 0)
        return rect();
    // Place data using zigzag pattern
    this->place_data_zigzag();
    this->get_best_mask();

    if (this->qr_data->minimal)
    {
        this->create_dummy_format_pattern(false);
    }

    return rect(point(this->qr_data->x, this->qr_data->y), 
        point(this->qr_data->x + this->qr_data->module_size * this->qr_data->scale, 
            this->qr_data->y + this->qr_data->module_size * this->qr_data->scale)
        );
}

/**
 * @private
 * @brief Get the adjusted QR code version that can fit the input data size and error correction
 */
void qr_generator::get_adjusted_version()
{
    uint32_t adj_version = 0;
    // Check for micro QR code versions first if the minimum version is in that range
    for (int i = this->qr_data->version; i < ((int)QR_V40 + 1); i++)
    {
        if (i == 0)
            continue;

        // Subtract the ECC code words from the total capacity
        uint32_t capacity = 0;

        // micro QR codes are indexed from -4 to -1 where the smallest (M1) is -4 and the largest (M4) is -1
        if (i < 0)
            // But they are stored in the array from 0 to 3, so we need to add 4 to the index
            capacity = qr_m_version_words[4 + i] - qr_ecc_mwords[4 + i][this->qr_data->ecc_lvl];
        else
            capacity = qr_version_words[i - 1] - qr_ecc_words[i - 1][this->qr_data->ecc_lvl];

        if (this->qr_data->data_size < capacity)
        {
            adj_version = i;
            break;
        }
    }

    this->qr_data->version = (qr_version_t)adj_version;
}

/**
 * @private
 * @brief Get the size of the QR code in modules for a given version
 * @param version The QR code version number
 * @return The size of the QR code in modules (number of modules per side)
 */
void qr_generator::get_module_size()
{
    uint32_t module_size = 0;

    if (this->qr_data->version >= QR_M1 && this->qr_data->version <= QR_M4)
        module_size = this->qr_m_module_size + ((this->qr_data->version + 4) * 2);
    else
        module_size = this->qr_modules_size + ((this->qr_data->version - 1) * 4);

    this->qr_data->module_size = module_size;
}

/**
 * @private
 * @brief Determine the encoding mode for the input data based on its content
 * @return The determined QR code encoding mode (numeric, alphanumeric or byte)
 */
void qr_generator::determine_mode()
{
    bool is_numeric[this->qr_data->data_size] = { false };
    bool is_alphanumeric[this->qr_data->data_size] = { false };
    // No point in checking for bytes as that is the "default" mode
    // Kanji mode is also not supported for now

    for (size_t i = 0; i < this->qr_data->data_size; i++)
    {
        uint8_t byte = this->qr_data->data[i];
        
        if (byte >= 0x30 && byte <= 0x39)
            is_numeric[i] = true;
        else if ((byte >= 0x30 && byte <= 0x39) || // 0-9
                 (byte >= 0x41 && byte <= 0x5A) || // A-Z
                 byte == 0x20 || byte == 0x24 || byte == 0x25 || byte == 0x2A || // Space, $, %, *
                 byte == 0x2B || byte == 0x2D || byte == 0x2E || byte == 0x2F || // +, -, ., /
                 byte == 0x3A) // :
            is_alphanumeric[i] = true;
    }
    
    bool all_numeric = true;
    bool all_alphanumeric = true;

    for (size_t i = 0; i < this->qr_data->data_size; i++)
    {
        if (!is_numeric[i])
            all_numeric = false;
        if (!is_alphanumeric[i])
            all_alphanumeric = false;
    }

    if (all_numeric)
        this->qr_data->mode = QR_MODE_NUMERIC;
    else if (all_alphanumeric)
        this->qr_data->mode = QR_MODE_ALPHANUMERIC;
    else
        this->qr_data->mode = QR_MODE_BYTE;
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
 */
void qr_generator::get_alignment_coordinates()
{
    if (this->qr_data->version < QR_V2)
        return;

    uint32_t intervals = (uint32_t)this->qr_data->version / 7 + 1;
    uint32_t distance = 4 * (uint32_t)this->qr_data->version + 4;
    uint32_t step = lround((double)distance / (double)intervals);

    // Round to the next even number
    step += step & 0b1;
    // First coordinate is always 6 
    this->qr_data->alignment_patterns[0] = 6;

    // Start right/bottom and go left/up by step * k
    for (size_t i = 1; i <= intervals; i++)
        this->qr_data->alignment_patterns[i] = 6 + distance - step * (intervals - i);

    this->qr_data->alignment_pattern_count = intervals + 1;
}

/**
 * @private
 * @brief Place encoded data into the QR buffer using zigzag pattern
 */
void qr_generator::place_data_zigzag()
{
    uint32_t bit_index = 0;
    uint32_t total_bits = this->qr_data->bit_size * 8;
    
    // Start from bottom-right, moving in 2-column strips upward
    // Alternate between moving right-to-left and left-to-right
    for (int col = this->qr_data->module_size - 1; col >= 0; col -= 2)
    {
        // Skip the timing column (column 6)
        if (col == 6)
            col--;
        
        // Direction: account for skipped column 6 in the calculation
        int adjusted_col = col > 6 ? col : col + 1;
        bool moving_up = (((this->qr_data->module_size - adjusted_col) / 2) % 2) == 0;
        
        // Determine row iteration direction
        int row_start = moving_up ? this->qr_data->module_size - 1 : 0;
        int row_end = moving_up ? -1 : this->qr_data->module_size;
        int row_step = moving_up ? -1 : 1;
        
        for (int row = row_start; row != row_end; row += row_step)
        {
            // Right column then left column of the pair
            for (int c = 0; c < 2; c++)
            {
                int x = col - c;
                int y = row;
                
                // Skip timing column 6
                if (x == 6)
                    continue;
                
                size_t index = (x + this->qr_data->dummy_zone) + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size);
                
                // Only place data where there's no mask (reserved area)
                if (!this->qr_data->mask[index] && bit_index < total_bits)
                {
                    uint32_t byte_index = bit_index / 8;
                    uint32_t bit_pos = 7 - (bit_index % 8);
                    
                    this->qr_data->buffer[index] = (this->qr_data->bits[byte_index] >> bit_pos) & 0x1;
                    bit_index++;
                }
            }
        }
    }
}

/**
 * @private
 * @brief Encode the input data into the data mask
 */
int qr_generator::encode_data()
{
    // Enforce byte mode for now
    this->qr_data->mode = QR_MODE_BYTE;

    // Add the data encoding to the bit buffer
    if (this->qr_data->mode != QR_MODE_BYTE)
        return -1; // Only byte mode is implemented for now

    // Keep track of the total bits to be encoded
    uint32_t total_count = 0;

    // Determine the character count indicator bits based on the mode and version
    uint32_t char_count_bits = 0;

    if (this->qr_data->version == 0)
        return -1; // Invalid version

    if (this->qr_data->version == QR_MODE_ALPHANUMERIC && this->qr_data->version == QR_M1)
        return -1; // Alphanumeric mode is not supported in M1 version
    if (this->qr_data->mode == QR_MODE_BYTE && this->qr_data->version < QR_M3)
        return -1; // Byte mode is not supported in M1 and M2 versions
    if (this->qr_data->mode == QR_MODE_KANJI && this->qr_data->version < QR_M3)
        return -1; // Kanji mode is not supported in M1 and M2 versions

    // Handle micro QR codes
    if (this->qr_data->version < 0)
    {
        if (this->qr_data->mode == QR_MODE_NUMERIC)
            char_count_bits = (this->qr_data->version == QR_M1) ? 3 : 
                (this->qr_data->version == QR_M2) ? 4 : (this->qr_data->version == QR_M3) ? 5 : 6;
        else if (this->qr_data->mode == QR_MODE_ALPHANUMERIC)
            char_count_bits = (this->qr_data->version == QR_M1) ? 2 : 
                (this->qr_data->version == QR_M2) ? 3 : (this->qr_data->version == QR_M3) ? 4 : 5;
        else if (this->qr_data->mode == QR_MODE_BYTE)
            char_count_bits = (this->qr_data->version == QR_M3) ? 4 : 5;
        else if (this->qr_data->mode == QR_MODE_KANJI)
            char_count_bits = (this->qr_data->version == QR_M3) ? 12 : 14;
    }
    // Handle standard QR codes
    else
    {
        if (this->qr_data->mode == QR_MODE_NUMERIC)
            char_count_bits = (this->qr_data->version < 11) ? 10 : (this->qr_data->version < 28) ? 12 : 14;
        else if (this->qr_data->mode == QR_MODE_ALPHANUMERIC)
            char_count_bits = (this->qr_data->version < 11) ? 9 : (this->qr_data->version < 28) ? 11 : 13;
        else if (this->qr_data->mode == QR_MODE_BYTE)
            char_count_bits = (this->qr_data->version < 11) ? 8 : 16;
        else if (this->qr_data->mode == QR_MODE_KANJI)
            char_count_bits = (this->qr_data->version < 11) ? 8 : (this->qr_data->version < 28) ? 10 : 12;
    }

    // Character count indicator (variable bits)
    total_count += char_count_bits;

    // Data encoding (variable bits)
    if (this->qr_data->mode == QR_MODE_NUMERIC)
        total_count += (this->qr_data->data_size / 3) * 10 + 
            ((this->qr_data->data_size % 3) == 1 ? 4 : (this->qr_data->data_size % 3) == 2 ? 7 : 0);
    else if (this->qr_data->mode == QR_MODE_ALPHANUMERIC)
        total_count += (this->qr_data->data_size / 2) * 11 + ((this->qr_data->data_size % 2) == 1 ? 6 : 0);
    else if (this->qr_data->mode == QR_MODE_BYTE)
        total_count += this->qr_data->data_size * 8;
    else if (this->qr_data->mode == QR_MODE_KANJI)
        total_count += this->qr_data->data_size * 13;

    // Check if the total count exceeds the capacity of the QR code
    uint32_t capacity = qr_version_words[(uint32_t)(this->qr_data->version - 1)];
    uint32_t ecc_capacity = qr_ecc_words[(uint32_t)(this->qr_data->version - 1)][(uint32_t)this->qr_data->ecc_lvl];
    if (total_count > capacity * 8)
        return -1;

    uint8_t bit_buffer[capacity + 1] = { 0 };
    uint32_t bit_offset = 0;
    this->qr_data->bits = bit_buffer;
    this->qr_data->bit_size = capacity;

    // Add the terminator bits (4 bits)
    switch (this->qr_data->version)
    {
        case QR_M1:
            total_count += 0; // M1 does not use a mode indicator
            total_count += 3; // M1 has a 3-bit terminator
            break;
        case QR_M2:
            total_count += 1; // M2 has a 1-bit mode indicator
            total_count += 5; // M2 has a 5-bit terminator

            // Add the mode indicator to the bit buffer (numeric or alphanumeric)
            bit_buffer[0] = (this->qr_data->mode == QR_MODE_NUMERIC) ? 1 << 7 : 0;
            bit_offset = 1;
            break;
        case QR_M3:
            total_count += 2; // M3 has a 2-bit mode indicator
            total_count += 7; // M3 has a 7-bit terminator

            // Add the mode indicator to the bit buffer
            bit_buffer[0] = this->qr_data->mode & 0x3 << 6;
            bit_offset = 2;
            break;
        case QR_M4:
            total_count += 3; // M4 has a 3-bit mode indicator
            total_count += 9; // M4 has a 9-bit terminator

            // Add the mode indicator to the bit buffer
            bit_buffer[0] = this->qr_data->mode & 0x7 << 5;
            bit_offset = 3;
            break;
        default:
            total_count += 4; // Standard QR codes have a 4-bit mode indicator
            total_count += 4; // Standard QR codes have a 4-bit terminator

            // Add the mode indicator to the bit buffer (4 bits) - placed in upper nibble of byte 0
            bit_buffer[0] = this->qr_data->mode << 4;
            bit_offset = 4;
            break;
    }

    // Check if padding is necessary
    if (total_count % 8 != 0)
        total_count += 8 - (total_count % 8);

    // Add the character count indicator to the bit buffer
    for (size_t i = 0; i < char_count_bits; i++)
    {
        uint32_t byte_index = (4 + i) / 8;
        uint32_t bit_position = 7 - ((4 + i) % 8);
        bit_buffer[byte_index] |= ((this->qr_data->data_size >> (char_count_bits - 1 - i)) & 0b1) << bit_position;
        bit_offset++;
    }

    for (size_t i = 0; i < this->qr_data->data_size; i++)
    {
        for (size_t bit = 0; bit < 8; bit++)
        {
            uint32_t byte_index = (4 + char_count_bits + (i * 8) + bit) / 8;
            uint32_t bit_position = 7 - ((4 + char_count_bits + (i * 8) + bit) % 8);
            bit_buffer[byte_index] |= ((this->qr_data->data[i] >> (7 - bit)) & 0b1) << bit_position;
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
    if (this->qr_data->version < QR_V3)
    {
        this->calculate_ecc(bit_buffer, (capacity - ecc_capacity), ecc_capacity, &bit_buffer[capacity - ecc_capacity]);
    }
    else
    {
        uint32_t total_blocks = qr_total_blocks[(uint32_t)(this->qr_data->version - 1)][this->qr_data->ecc_lvl];
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

    return 0;
}

/**
 * @private
 * @brief Draw the QR code to the display using the buffer
 */
void qr_generator::draw_qr_code()
{
    uint32_t frame_ptr = 0;

    // Y loop
    for (size_t iy = 0; iy < this->qr_data->qr_size; iy++)
    {
        // X loop
        for (size_t ix = 0; ix < this->qr_data->qr_size; ix++)
        {
            color_t color = this->qr_data->buffer[ix + iy * this->qr_data->qr_size] ? colors::black : colors::white;

            // Loop scaled pixel y
            for (size_t sy = 0; sy < this->qr_data->scale; sy++)
            {
                // Loop scaled pixel x
                for (size_t sx = 0; sx < this->qr_data->scale; sx++)
                {
                    frame_ptr = ((ix * this->qr_data->scale) + sx + this->qr_data->x) + 
                        (((iy * this->qr_data->scale) + sy + this->qr_data->y) * this->display_ptr->getWidth());

                    this->frameBuffer[frame_ptr] = color;
                }
            }
        }
    }
}

/**
 * @private
 * @brief Draw the QR code to the display using the buffer, but use an artistic bitmap for the non-essential details
 */
void qr_generator::draw_minimal_qr_code()
{
    uint32_t frame_ptr = 0;
    uint32_t disp_width = this->display_ptr->getWidth();
    uint32_t disp_height = this->display_ptr->getHeight();

    for (size_t iy = 0; iy < this->qr_data->qr_size; iy++)
    {
        for (size_t ix = 0; ix < this->qr_data->qr_size; ix++)
        {
            bool is_black_module = this->qr_data->buffer[ix + iy * this->qr_data->qr_size];
            bool is_important_detail = this->qr_data->mask[ix + iy * this->qr_data->qr_size];

            for (size_t sy = 0; sy < this->qr_data->scale; sy++)
            {
                for (size_t sx = 0; sx < this->qr_data->scale; sx++)
                {
                    frame_ptr = ((ix * this->qr_data->scale) + sx + this->qr_data->x) + 
                                (((iy * this->qr_data->scale) + sy + this->qr_data->y) * disp_width);

                    int screen_x = (ix * this->qr_data->scale) + sx + this->qr_data->x;
                    int screen_y = (iy * this->qr_data->scale) + sy + this->qr_data->y;

                    if (screen_x >= 0 && screen_x < (int)disp_width &&
                        screen_y >= 0 && screen_y < (int)disp_height)
                    {
                        if (is_important_detail)
                        {
                            if (is_black_module)
                                this->frameBuffer[frame_ptr] = colors::black;
                        }
                        else
                        {
                            bool is_center_x = false;
                            bool is_center_y = false;

#if defined(LCD_EINK_DRIVER)
                            if (this->qr_data->scale >= 4) 
                            {
                                uint32_t low = (this->qr_data->scale / 2) - 1;
                                uint32_t high = this->qr_data->scale / 2;
                                is_center_x = (sx == low || sx == high);
                                is_center_y = (sy == low || sy == high);
                            } 
                            else 
                            {
                                is_center_x = (sx == this->qr_data->scale / 2);
                                is_center_y = (sy == this->qr_data->scale / 2);
                            }
#else
                            if (this->qr_data->scale >= 6) 
                            {
                                uint32_t padding = this->qr_data->scale / 3;
                                is_center_x = (sx >= padding) && (sx < this->qr_data->scale - padding);
                                is_center_y = (sy >= padding) && (sy < this->qr_data->scale - padding);
                            } 
                            else if (this->qr_data->scale >= 4) 
                            {
                                uint32_t low = (this->qr_data->scale / 2) - 1;
                                uint32_t high = this->qr_data->scale / 2;
                                is_center_x = (sx == low || sx == high);
                                is_center_y = (sy == low || sy == high);
                            } 
                            else 
                            {
                                is_center_x = (sx == this->qr_data->scale / 2);
                                is_center_y = (sy == this->qr_data->scale / 2);
                            }
#endif

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
 */
void qr_generator::draw_artistic_qr_code()
{
    uint32_t frame_ptr = 0;
    const uint8_t* bitmap_bytes = (const uint8_t*)this->qr_data->bitmap;

    uint32_t disp_width = this->display_ptr->getWidth();
    uint32_t disp_height = this->display_ptr->getHeight();

    for (size_t iy = 0; iy < this->qr_data->qr_size; iy++)
    {
        for (size_t ix = 0; ix < this->qr_data->qr_size; ix++)
        {
            bool is_black_module = this->qr_data->buffer[ix + iy * this->qr_data->qr_size];
            bool is_important_detail = this->qr_data->mask[ix + iy * this->qr_data->qr_size];

            for (size_t sy = 0; sy < this->qr_data->scale; sy++)
            {
                for (size_t sx = 0; sx < this->qr_data->scale; sx++)
                {
                    frame_ptr = ((ix * this->qr_data->scale) + sx + this->qr_data->x) + 
                                (((iy * this->qr_data->scale) + sy + this->qr_data->y) * disp_width);

                    int bx = (ix * this->qr_data->scale) + sx;
                    int by = (iy * this->qr_data->scale) + sy;

                    if ((bx + (int)this->qr_data->x) >= 0 && (bx + (int)this->qr_data->x) < (int)disp_width &&
                        (by + (int)this->qr_data->y) >= 0 && (by + (int)this->qr_data->y) < (int)disp_height &&
                        bx >= 0 && bx < (int)this->qr_data->bitmap_width &&
                        by >= 0 && by < (int)this->qr_data->bitmap_height)
                    {
                        uint32_t colorWord = 0;
                        for (int i = 0; i < 3; ++i)
                        {
                            uint8_t colorByte = bitmap_bytes[(by * this->qr_data->bitmap_width + bx) * 3 + i];
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

#if defined(LCD_EINK_DRIVER)
                            if (this->qr_data->scale >= 4) 
                            {
                                uint32_t low = (this->qr_data->scale / 2) - 1;
                                uint32_t high = this->qr_data->scale / 2;
                                is_center_x = (sx == low || sx == high);
                                is_center_y = (sy == low || sy == high);
                            } 
                            else 
                            {
                                is_center_x = (sx == this->qr_data->scale / 2);
                                is_center_y = (sy == this->qr_data->scale / 2);
                            }
#else
                            if (this->qr_data->scale >= 6) 
                            {
                                uint32_t padding = this->qr_data->scale / 3;
                                is_center_x = (sx >= padding) && (sx < this->qr_data->scale - padding);
                                is_center_y = (sy >= padding) && (sy < this->qr_data->scale - padding);
                            } 
                            else if (this->qr_data->scale >= 4) 
                            {
                                uint32_t low = (this->qr_data->scale / 2) - 1;
                                uint32_t high = this->qr_data->scale / 2;
                                is_center_x = (sx == low || sx == high);
                                is_center_y = (sy == low || sy == high);
                            } 
                            else 
                            {
                                is_center_x = (sx == this->qr_data->scale / 2);
                                is_center_y = (sy == this->qr_data->scale / 2);
                            }
#endif

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
 * @brief Mask the dummy zone around the QR code to prevent data placement and ensure it is not rendered
 */
void qr_generator::mask_dummy_zone()
{
    uint32_t padded_size = this->qr_data->qr_size;

    for (size_t y = 0; y < padded_size; y++)
    {
        for (size_t x = 0; x < padded_size; x++)
        {
            if (x < this->qr_data->dummy_zone || x >= (this->qr_data->module_size + this->qr_data->dummy_zone) ||
                y < this->qr_data->dummy_zone || y >= (this->qr_data->module_size + this->qr_data->dummy_zone))
            {
                size_t index = x + y * padded_size;
                this->qr_data->mask[index] = true; // Mark as reserved to prevent data placement
            }
        }
    }
}

/**
 * @private
 * @brief Draw the fixed timing pattern of the QR code at the specified position
 */
void qr_generator::create_timing_pattern()
{
    // These are always at row 6 and column 6
    uint32_t offset = 6 + this->qr_data->dummy_zone;
    uint32_t module_size = this->qr_data->module_size + this->qr_data->dummy_zone;

    // Draw the timing pattern
    if (this->qr_data->version >= QR_M1 && this->qr_data->version <= QR_M4)
    {
        // Only draw at the edges for micro QR codes
        for (size_t i = this->qr_data->dummy_zone; i < module_size; i++)
        {
            this->qr_data->buffer[i * this->qr_data->qr_size] = (i % 2) == 0;
            this->qr_data->buffer[i] = (i % 2) == 0;

            this->qr_data->mask[i * this->qr_data->qr_size] = true;
            this->qr_data->mask[i] = true;
        }
    }
    else
    {
        for (size_t i = this->qr_data->dummy_zone; i < module_size; i++)
        {
            this->qr_data->buffer[offset + i * this->qr_data->qr_size] = (i % 2) == 0;
            this->qr_data->buffer[i + offset * this->qr_data->qr_size] = (i % 2) == 0;

            this->qr_data->mask[offset + i * this->qr_data->qr_size] = true;
            this->qr_data->mask[i + offset * this->qr_data->qr_size] = true;
        }
    }
}

/**
 * @private
 * @brief Add the finder pattern to the QR code
 */
void qr_generator::add_finder_patterns()
{
    uint32_t module_size = this->qr_data->qr_size - this->qr_data->dummy_zone - 8;
    uint32_t count = this->qr_data->version >= QR_V1 ? 3 : 1;
    uint32_t mask_offset = 0;

    for (size_t corner = 0; corner < count; corner++)
    {
        // For standard QR codes, the finder patterns are at the top-left, top-right, and bottom-left corners
        uint32_t x_offset = (corner == 1) ? 1 : 0; // Apply x offset for the top-right corner
        uint32_t y_offset = (corner == 2) ? 1 : 0; // Apply y offset for the bottom-left corner

        uint32_t x = (corner == 1) ? module_size : this->qr_data->dummy_zone; // X position for top-right corner
        uint32_t y = (corner == 2) ? module_size : this->qr_data->dummy_zone; // Y position for bottom-left corner

        uint32_t buffer_offset = 0;
        for (size_t iy = 0; iy < 7; iy++)
        {
            for (size_t ix = 0; ix < 7; ix++)
            {
                buffer_offset = (ix + x + x_offset) + ((iy + y + y_offset) * this->qr_data->qr_size);
                this->qr_data->buffer[buffer_offset] = this->finder_pattern[ix + iy * 7];
            }
        }

        // Create the mask for the finder pattern, which is 9x9 including the separator
        for (size_t iy = 0; iy < 8; iy++)
        {
            for (size_t ix = 0; ix < 8; ix++)
            {
                mask_offset = (ix + x) + ((iy + y) * this->qr_data->qr_size);
                this->qr_data->mask[mask_offset] = true;
            }
        }
    }
}

/**
 * @private
 * @brief Create the format pattern of the QR code, which contains the error correction level and mask type information
 */
void qr_generator::create_format_pattern()
{
    uint32_t module_size = this->qr_data->qr_size;

    // Format information lookup table [ECC level (2 bits) | Mask type (3 bits)]
    // Generated using ISO/IEC 18004:2015 BCH polynomial (x^10 + x^8 + x^5 + x^4 + x^2 + x + 1)
    uint16_t format_info_table[32] = {
        0x77c4, 0x72f3, 0x7daa, 0x789d, 0x662f, 0x6318, 0x6c41, 0x6976,
        0x5412, 0x5125, 0x5e7c, 0x5b4b, 0x45f9, 0x40ce, 0x4f97, 0x4aa0,
        0x355f, 0x3068, 0x3f31, 0x3a06, 0x24b4, 0x2183, 0x2eda, 0x2bed,
        0x1689, 0x13be, 0x1ce7, 0x19d0, 0x0762, 0x0255, 0x0d0c, 0x083b
    };
    
    uint32_t format_info = ((this->qr_data->ecc_lvl & 0b11) << 3) | (this->qr_data->mask_type & 0b111);
    uint32_t value = format_info_table[format_info];
    uint32_t ptr = 0;

    // Micro QR codes have their format pattern in a single location
    if (this->qr_data->module_size < 21)
    {
        // First 0-7 bits on the top-left finder
        for (size_t i = 1; i < 9; i++)
        {
            ptr = 8 + this->qr_data->dummy_zone + (i + this->qr_data->dummy_zone) * module_size;
            this->qr_data->buffer[ptr] = ((value >> i) & 0x1);
            this->qr_data->mask[ptr] = true;
        }

        // 8-14 bits on the top-left finder
        for (size_t i = 0; i < 7; i++)
        {
            ptr = (8 - i) + this->qr_data->dummy_zone + (8 + this->qr_data->dummy_zone) * module_size;
            this->qr_data->buffer[ptr] = ((value >> (i + 8)) & 0x1);
            this->qr_data->mask[ptr] = true;
        }
    }
    // Standard QR codes have the format code split into two locations, where one is divided into two sections
    else
    {
        // First 0-5 bits on the top-left finder
        for (size_t i = 0; i < 6; i++)
        {
            ptr = 8 + this->qr_data->dummy_zone + (i + this->qr_data->dummy_zone) * module_size;
            this->qr_data->buffer[ptr] = ((value >> i) & 0x1);
            this->qr_data->mask[ptr] = true;
        }

        // 6-7 bits on the top-left finder
        for (size_t i = 0; i < 2; i++)
        {
            ptr = 8 + this->qr_data->dummy_zone + (i + 7 + this->qr_data->dummy_zone) * module_size;
            this->qr_data->buffer[ptr] = ((value >> (i + 6)) & 0x1);
            this->qr_data->mask[ptr] = true;
        }

        // 8 bit on the top-left finder
        ptr = 7 + this->qr_data->dummy_zone + (8 + this->qr_data->dummy_zone) * module_size;
        this->qr_data->buffer[ptr] = ((value >> 8) & 0x1);
        this->qr_data->mask[ptr] = true;

        // 9-14 bits on the top-left finder
        for (size_t i = 0; i < 6; i++)
        {
            ptr = (5 - i) + this->qr_data->dummy_zone + (8 + this->qr_data->dummy_zone) * module_size;
            this->qr_data->buffer[ptr] = ((value >> (i + 9)) & 0x1);
            this->qr_data->mask[ptr] = true;
        }

        // 0-7 bits on the top-right finder
        for (size_t i = 0; i < 8; i++)
        {
            ptr = (module_size - this->qr_data->dummy_zone - 8 + i) + ((8 + this->qr_data->dummy_zone) * module_size);
            this->qr_data->buffer[ptr] = ((value >> (7 - i)) & 0x1);
            this->qr_data->mask[ptr] = true;
        }

        // 8-14 bits on the bottom-left finder
        for (size_t i = 0; i < 7; i++)
        {
            ptr = 8 + this->qr_data->dummy_zone + ((module_size - i - 1 - this->qr_data->dummy_zone) * module_size);
            this->qr_data->buffer[ptr] = ((value >> (14 - i)) & 0x1);
            this->qr_data->mask[ptr] = true;
        }

        // Dark module
        ptr = 8 + this->qr_data->dummy_zone + ((module_size - 8 - this->qr_data->dummy_zone) * module_size);
        this->qr_data->buffer[ptr] = true;
        this->qr_data->mask[ptr] = true;
    }
}

/**
 * @private
 * @brief Create the dummy format pattern of the QR code, which reserves space for the format information in the mask without actually encoding it
 */
void qr_generator::create_dummy_format_pattern(bool mask_bit)
{
    uint32_t module_size = this->qr_data->qr_size;

    // Micro QR codes have their format pattern in a single location
    if (module_size < 21)
    {
        // First 0-7 bits on the top-left finder
        for (size_t i = 1; i < 9; i++)
            this->qr_data->mask[8 + this->qr_data->dummy_zone + (i + this->qr_data->dummy_zone) * module_size] = mask_bit;

        // 8-14 bits on the top-left finder
        for (size_t i = 0; i < 7; i++)
            this->qr_data->mask[(8 - i) + this->qr_data->dummy_zone + (8 + this->qr_data->dummy_zone) * module_size] = mask_bit;

    }
    // Standard QR codes have the format code split into two locations, where one is divided into two sections
    else
    {
        // First 0-5 bits on the top-left finder
        for (size_t i = 0; i < 6; i++)
            this->qr_data->mask[8 + this->qr_data->dummy_zone + (i + this->qr_data->dummy_zone) * module_size] = mask_bit;

        // 6-7 bits on the top-left finder
        for (size_t i = 0; i < 2; i++)
            this->qr_data->mask[8 + this->qr_data->dummy_zone + (i + 7 + this->qr_data->dummy_zone) * module_size] = mask_bit;

        // 8 bit on the top-left finder
        this->qr_data->mask[7 + this->qr_data->dummy_zone + (8 + this->qr_data->dummy_zone) * module_size] = mask_bit;

        // 9-14 bits on the top-left finder
        for (size_t i = 0; i < 6; i++)
            this->qr_data->mask[(5 - i) + this->qr_data->dummy_zone + (8 + this->qr_data->dummy_zone) * module_size] = mask_bit;

        // 0-7 bits on the top-right finder
        for (size_t i = 0; i < 8; i++)
            this->qr_data->mask[(module_size - this->qr_data->dummy_zone - 8 + i) + 
                ((8 + this->qr_data->dummy_zone) * module_size)] = mask_bit;

        // 8-14 bits on the bottom-left finder
        for (size_t i = 0; i < 7; i++)
            this->qr_data->mask[8 + this->qr_data->dummy_zone + 
                ((module_size - i - 1 - this->qr_data->dummy_zone) * module_size)] = mask_bit;

        // Dark module
        this->qr_data->mask[8 + this->qr_data->dummy_zone + 
            ((module_size - 8 - this->qr_data->dummy_zone) * module_size)] = mask_bit;
    }
}

/**
 * @private
 * @brief Add the alignment pattern to the QR code
 */
void qr_generator::add_alignment_patterns()
{
    if (this->qr_data->alignment_pattern_count == 0)
        return;

    uint32_t x_coord = 0;
    uint32_t y_coord = 0;
    
    uint32_t alt_x = 0;
    uint32_t alt_y = 0;

    uint32_t buffer_offset = 0;

    for (size_t i = 0; i < this->qr_data->alignment_pattern_count; i++)
    {
        y_coord = this->qr_data->alignment_patterns[i];

        for (size_t j = 0; j < this->qr_data->alignment_pattern_count; j++)
        {
            x_coord = this->qr_data->alignment_patterns[j];

            // Remove the coordinates that end in the positions of our finders
            if (x_coord == this->qr_data->alignment_patterns[0] 
                && y_coord == this->qr_data->alignment_patterns[0])
                continue;
            if (x_coord == this->qr_data->alignment_patterns[this->qr_data->alignment_pattern_count - 1] 
                && y_coord == this->qr_data->alignment_patterns[0])
                continue;
            if (x_coord == this->qr_data->alignment_patterns[0] && 
                y_coord == this->qr_data->alignment_patterns[this->qr_data->alignment_pattern_count - 1])
                continue;

            alt_x = x_coord - 2 + this->qr_data->dummy_zone;
            alt_y = y_coord - 2 + this->qr_data->dummy_zone;
            buffer_offset = 0;

            // Loop y
            for (size_t i = 0; i < 5; i++)
            {
                // Loop x
                for (size_t j = 0; j < 5; j++)
                {
                    buffer_offset = (j + alt_x) + 
                        ((i + alt_y) * this->qr_data->qr_size);
                    this->qr_data->buffer[buffer_offset] = this->alignment_pattern[j + i * 5];
                    this->qr_data->mask[buffer_offset] = true;
                }
            }
        }
    }
}

/**
 * @private
 * @brief Add the version pattern to the QR code
 */
void qr_generator::create_version_pattern()
{
    if (this->qr_data->version < QR_V7)
        return;

    uint32_t frame_ptr = 0;
    uint32_t value = (this->qr_data->version & 0x3f) << 12;
    uint32_t adj_x = (this->qr_data->module_size - 8) - 3;
    uint32_t adj_y = (this->qr_data->module_size - 8) - 3;

    // G(x) = x^12 + x^11 + x^10 + x^9 + x^8 + x^5 + x^2 + 1
    uint32_t generator = 0x1f25;
    
    for (int i = 17; i >= 12; i--)
    {
        if ((value >> i) & 0x1)
            value ^= generator << (i - 12);
    }

    value = (this->qr_data->version << 12) | (value & 0xfff);

    // value = 0b1 << 17;

    for (size_t i = 0; i < 3; i++)
    {
        for (size_t j = 0; j < 6; j++)
        {
            uint32_t ptr = i + (j * 3);
            bool bit = ((value >> ptr) & 0x1);
            this->qr_data->buffer[j + this->qr_data->dummy_zone + 
                ((i + adj_y) * (this->qr_data->module_size + this->qr_data->dummy_zone * 2))] = bit;
            this->qr_data->mask[j + this->qr_data->dummy_zone + 
                ((i + adj_y) * (this->qr_data->module_size + this->qr_data->dummy_zone * 2))] = true;
            
            this->qr_data->buffer[(i + this->qr_data->dummy_zone + adj_x) + 
                (j * (this->qr_data->module_size + this->qr_data->dummy_zone * 2))] = bit;
            this->qr_data->mask[(i + this->qr_data->dummy_zone + adj_x) + 
                (j * (this->qr_data->module_size + this->qr_data->dummy_zone * 2))] = true;
        }
    }
}

/**
 * @private
 * @brief Get the best mask for the QR code by applying each mask and calculating the penalty score, then selecting the one with the lowest penalty
 */
void qr_generator::get_best_mask()
{
    // Save the original buffer state before any format pattern is applied
    bool original_buffer[this->qr_data->qr_size * this->qr_data->qr_size];
    memcpy(original_buffer, this->qr_data->buffer, this->qr_data->qr_size * this->qr_data->qr_size);
    
    uint32_t best_penalty = UINT32_MAX;
    uint32_t penalty = 0;
    qr_mask_type_t best_mask = QR_MASK_TYPE_0;

    for (size_t mask = 0; mask <= QR_MASK_TYPE_7; mask++)
    {
        penalty = 0;
        this->qr_data->mask_type = (qr_mask_type_t)mask;
        
        // Create the format pattern for this mask, which is needed for accurate penalty calculation
        this->create_format_pattern();

        // Apply the mask to the QR code data
        this->generate_data_mask();
        
        penalty += this->get_mask_run_penalty();
        penalty += this->get_mask_box_penalty();
        penalty += this->get_mask_finder_penalty();
        penalty += this->get_mask_balance_penalty();

        if (penalty < best_penalty)
        {
            best_penalty = penalty;
            best_mask = this->qr_data->mask_type;
        }

        // Restore the original buffer state for this mask test
        memcpy(this->qr_data->buffer, original_buffer, this->qr_data->qr_size * this->qr_data->qr_size);
    }

    this->qr_data->mask_type = best_mask;

    // Restore the original buffer and apply the best mask to the QR code
    memcpy(this->qr_data->buffer, original_buffer, this->qr_data->qr_size * this->qr_data->qr_size);
    this->create_format_pattern();
    this->generate_data_mask();
}

/**
 * @private
 * @brief Generate the data mask for the QR code
 */
void qr_generator::generate_data_mask()
{
    // Copy the mask to the data mask, while inverting the data bits according to the mask type
    for (size_t y = 0; y < this->qr_data->module_size; y++)
    {
        for (size_t x = 0; x < this->qr_data->module_size; x++)
        {
            size_t index = (x + this->qr_data->dummy_zone) + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size);

            if (this->qr_data->mask[index])
                continue;

            bool mask_bit = false;
            switch (this->qr_data->mask_type)
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

            this->qr_data->buffer[index] = this->qr_data->buffer[index] ^ mask_bit;
        }
    }
}

/**
 * @private
 * @brief Calculate the run penalty score for the QR code
 * @return The run penalty score
 */
int qr_generator::get_mask_run_penalty()
{
    int penalty = 0;
    uint32_t counter = 0;
    bool current_color = false;
    bool prev_color = false;
    uint32_t ptr = 0;

    // Start by finding the longest horizontal runs
    for (size_t y = 0; y < this->qr_data->module_size; y++)
    {
        counter = 1;
        prev_color = this->qr_data->buffer[this->qr_data->dummy_zone + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size)];

        for (size_t x = 1; x < this->qr_data->module_size; x++)
        {
            ptr = (x + this->qr_data->dummy_zone) + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size);
            current_color = this->qr_data->buffer[ptr];

            if (current_color == prev_color)
            {
                counter++;
            }
            else
            {
                if (counter >= 5)
                    penalty += (counter - 2);

                counter = 1;
                prev_color = current_color;
            }
        }

        if (counter >= 5)
            penalty += (counter - 2);
    }

    // Find the longest vertical runs
    for (size_t x = 0; x < this->qr_data->module_size; x++)
    {
        counter = 1;
        prev_color = this->qr_data->buffer[(x + this->qr_data->dummy_zone) + (this->qr_data->dummy_zone * this->qr_data->qr_size)];

        for (size_t y = 1; y < this->qr_data->module_size; y++)
        {
            ptr = (x + this->qr_data->dummy_zone) + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size);
            current_color = this->qr_data->buffer[ptr];

            if (current_color == prev_color)
            {
                counter++;
            }
            else
            {
                if (counter >= 5)
                    penalty += (counter - 2);

                counter = 1;
                prev_color = current_color;
            }
        }

        if (counter >= 5)
            penalty += (counter - 2);
    }

    return penalty;
}

/**
 * @private
 * @brief Calculate the box penalty score for the QR code
 * @return The box penalty score
 */
int qr_generator::get_mask_box_penalty()
{
    int penalty = 0;
    uint32_t ptr = 0;

    for (size_t y = 0; y < this->qr_data->module_size - 1; y++)
    {
        for (size_t x = 0; x < this->qr_data->module_size - 1; x++)
        {
            ptr = (x + this->qr_data->dummy_zone) + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size);
            bool color = this->qr_data->buffer[ptr];

            if (color == this->qr_data->buffer[ptr + 1] && 
                color == this->qr_data->buffer[ptr + this->qr_data->qr_size] && 
                color == this->qr_data->buffer[ptr + this->qr_data->qr_size + 1])
            {
                penalty += 3;
            }
        }
    }

    return penalty;
}

/**
 * @private
 * @brief Calculate the finder pattern penalty score for the QR code
 * @return The finder pattern penalty score
 */
int qr_generator::get_mask_finder_penalty()
{
    int penalty = 0;
    uint32_t ptr = 0;
    uint32_t bits = 0;
    
    uint32_t pattern_a = 0b010111010000;
    uint32_t pattern_b = 0b000010111010;

    // Check for horizontal finder-like patterns
    for (size_t y = 0; y < this->qr_data->qr_size; y++)
    {
        for (size_t x = 0; x < this->qr_data->qr_size - 11; x++)
        {
            ptr = x + (y * this->qr_data->qr_size);
            bits = 0;
            for (size_t i = 0; i < 12; i++)
            {
                bits <<= 1;
                bits |= this->qr_data->buffer[ptr + i] ? 1 : 0;
            }

            if (bits == pattern_a || bits == pattern_b)
                penalty++;
        }
    }

    // Check for vertical finder-like patterns
    for (size_t x = 0; x < this->qr_data->qr_size; x++)
    {
        for (size_t y = 0; y < this->qr_data->qr_size - 11; y++)
        {
            ptr = x + (y * this->qr_data->qr_size);
            bits = 0;
            for (size_t i = 0; i < 12; i++)
            {
                bits <<= 1;
                bits |= this->qr_data->buffer[ptr + i * this->qr_data->qr_size] ? 1 : 0;
            }

            if (bits == pattern_a || bits == pattern_b)
                penalty++;
        }
    }

    return penalty * 40;
}

/**
 * @private
 * @brief Calculate the balance penalty score for the QR code
 * @return The balance penalty score
 */
int qr_generator::get_mask_balance_penalty()
{
    uint32_t ptr = 0;
    uint32_t black_modules = 0;
    uint32_t total_modules = this->qr_data->module_size * this->qr_data->module_size;

    for (size_t y = 0; y < this->qr_data->module_size; y++)
    {
        for (size_t x = 0; x < this->qr_data->module_size; x++)
        {
            ptr = (x + this->qr_data->dummy_zone) + ((y + this->qr_data->dummy_zone) * this->qr_data->qr_size);
            if (this->qr_data->buffer[ptr])
                black_modules++;
        }
    }

    // Multiply by 100 twice to get two decimal places without using floating point
    int percent = (black_modules * 10000) / total_modules;
    int deviation = percent - 5000; // Deviation from 50% in hundredths of a percent

    // Absolute value of deviation
    if (deviation < 0)
        deviation = -deviation;

    return (deviation / 500) * 10; // Each 5% deviation adds 10 penalty points
}