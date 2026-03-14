# Set the extensions to PicoGFX, 0 to disable, 1 to enable
set(PICO_GFX_PRINT      ?)
set(PICO_GFX_GAUGE      ?)
set(PICO_GFX_GRADIENT   ?)
set(PICO_GFX_GRAPHICS   ?)

# List of desired fonts to include in the build (e.g. "RobotoMono.ttf")
# Can store multiple fonts by separating them with a semicolon.
# Failure to set this variable will result in the print extension being disabled.
set(PICO_GFX_FONTS      ?)
# List of font sizes to include in the build (e.g. "16;24;32;48")
# Can store multiple font sizes by separating them with a semicolon.
set(PICO_GFX_FONT_SIZES ?)