SET(SOURCES ui_Screen1.c
    ui.c
    ui_comp_hook.c
    ui_helpers.c
    ui_img_info_png.c
    fonts/ui_font_bold.c
    fonts/ui_font_kanit.c)

add_library(ui ${SOURCES})
