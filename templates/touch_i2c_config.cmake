# touch_i2c_config.cmake

# Set the driver for the touch controller, e.g. CST816, GT911, etc.
set(TOUCH_DRIVER        ?)

# Protocol used to communicate with the touch controller
set(TOUCH_PROTOCOL      I2C)
# Set I2C instance to use (I2C0 or I2C1)
set(TOUCH_I2C_INSTANCE  ?)

# Set the width of the touch controller in pixels
set(TOUCH_I2C_SDA_PIN   ?)
set(TOUCH_I2C_SCL_PIN   ?)
set(TOUCH_I2C_IRQ_PIN   ?)
set(TOUCH_I2C_RST_PIN   ?)
set(TOUCH_I2C_SPEED     ?)