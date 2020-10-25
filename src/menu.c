#include <libopencm3/cm3/scb.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "oled.h"
#include "font.h"
#include "menu.h"
#include "time.h"

#define SETTING_TEMP_MAX 380
#define SETTING_TEMP_MIN 120
#define SETTING_TEMP_STEP 5

struct {
    enum {
        MENU_STATE_START,

        MENU_STATE_TEMP_SET,
        MENU_STATE_PD_INFO,
        MENU_STATE_STANDBY,
        MENU_STATE_DFU,

        MENU_STATE_END,
    } menu_state;
    bool menu_selected;
    union selected_info {
        uint16_t temp_set;
        uint16_t standby_temp;
    } selected_info;
    struct {
        bool open;
        uint32_t timeout;
        char text[17];
    } popup;
    // todo: move this to a different file
    struct {
        uint16_t tip_temp;
        uint16_t set_temp;
        bool tip_connected;
    } status;
    bool changed;
} ui = {
    .menu_state = MENU_STATE_PD_INFO,
    .menu_selected = false,
    .popup = {
        .open = true,
        .text = "OtterIron Pro",
        .timeout = 1200,
    },
    .status = {
        .tip_connected = true,
        .tip_temp = 244,
        .set_temp = 250,
    },
    .changed = true,
};

void menu_draw(void) {
    if (ui.popup.open && sys_ms > ui.popup.timeout) {
        ui.popup.open = false;
        ui.changed = true;
    }
    if (!ui.changed) {
        return;
    }
    ui.changed = false;
    char buffer[17];
    disp_clear();

    // draw top line
    // this always shows status information, regardless of the state
    if (ui.status.tip_connected) {
        snprintf(buffer, sizeof buffer, "T%3d/%3d ", ui.status.tip_temp, ui.status.set_temp);
    } else {
        snprintf(buffer, sizeof buffer, "DISC ");
    }

    disp_string_line(0, 0, buffer);

    // draw bottom line
    // this contains the menu
    switch (ui.menu_state) {
        case MENU_STATE_TEMP_SET: {
            if (ui.menu_selected) {
                // todo: invert colors
                snprintf(buffer, sizeof buffer, "set temp <%d>", ui.selected_info.temp_set);
                disp_string_line(0, 1, buffer);
            } else {
                disp_string_line(0, 1, "set temp");
            }
            break;
        }
        case MENU_STATE_PD_INFO: {
            disp_string_line(0, 1, "todo: PD info");
            break;
        }
        case MENU_STATE_STANDBY: {
            if (ui.menu_selected) {
                disp_string_line(0, 1, "standby mode on");
            } else {
                disp_string_line(0, 1, "standby mode off");
            }
            break;
        }
        case MENU_STATE_DFU: {
            if (ui.menu_selected) {
                disp_string_line(0, 1, "yes (-) / no (+)");
            } else {
                disp_string_line(0, 1, "enter DFU mode");
            }
            break;
        }
        default: {
            ui.menu_state = MENU_STATE_PD_INFO;
            ui.changed = true;
            break;
        }
    }

    if (ui.popup.open) {
        // todo: add fancy box around text
        uint16_t offset = OLED_WIDTH / 2 - strlen(ui.popup.text) * FONT_WIDTH / 2;
        disp_string_y(offset, 4, ui.popup.text);
    }

    disp_refresh();
}

void menu_button(bool up, bool down) {
    // no buttons pressed, this shouldn't happen
    if (!up && !down) {
        return;
    }
    bool both = up & down;
    // go up / down one menu item
    if (!ui.menu_selected && !both) {
        if (up && (ui.menu_state - 1) > MENU_STATE_START) {
            ui.menu_state--;
            ui.changed = true;
        }
        if (down && (ui.menu_state + 1) < MENU_STATE_END) {
            ui.menu_state++;
            ui.changed = true;
        }
        return;
    }
    // process individual menu item
    switch (ui.menu_state) {
        case MENU_STATE_TEMP_SET: {
            if (ui.menu_selected) {
                if (both) {
                    ui.status.set_temp = ui.selected_info.temp_set;
                    ui.menu_selected = false;
                } else if (up && (ui.selected_info.temp_set + SETTING_TEMP_STEP) <= SETTING_TEMP_MAX) {
                    ui.selected_info.temp_set += SETTING_TEMP_STEP;
                    ui.changed = true;
                } else if (down && (ui.selected_info.temp_set - SETTING_TEMP_STEP) >= SETTING_TEMP_MIN) {
                    ui.selected_info.temp_set -= SETTING_TEMP_STEP;
                    ui.changed = true;
                }
            } else if (both) {
                ui.menu_selected = true;
                ui.selected_info.temp_set = ui.status.set_temp;
                ui.changed = true;
            }
            break;
        }
        case MENU_STATE_PD_INFO: {
            break;
        }
        case MENU_STATE_STANDBY: {
            if (ui.menu_selected && both) {
                ui.menu_selected = false;
                ui.status.set_temp = ui.selected_info.standby_temp;
                ui.changed = true;
            } else if (!ui.menu_selected && both) {
                ui.menu_selected = true;
                ui.selected_info.standby_temp = ui.status.set_temp;
                ui.status.set_temp = 0;
                ui.changed = true;
            }
            break;
        }
        case MENU_STATE_DFU: {
            if (ui.menu_selected) {
                if (up) {
                    ui.menu_selected = false;
                    ui.changed = true;
                } else if (down) {
                    disp_clear();
                    disp_string_line(0, 0, "DFU mode");
                    disp_refresh();

                    // todo: power off tip
                    scb_reset_system();
                }
            } else if (both) {
                ui.menu_selected = true;
                ui.changed = true;
            }
            break;
        }
        default: {
            ui.menu_state = MENU_STATE_PD_INFO;
            ui.changed = true;
            break;
        }
    }
}