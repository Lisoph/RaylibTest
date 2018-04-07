#pragma once

#include <stdlib.h>
#include <stdbool.h>
#include "raylib.h"

#define MENU_MAX_PAGES 10
#define MENU_MAX_ENTRIES 10

typedef struct MenuPage MenuPage;
typedef struct Menu Menu;

typedef struct MenuEntry {
    char const *text;
    void (*click)(Menu *menu, MenuPage *page, void *user);
    Rectangle computed_bbox;
    float highlight_timer;
} MenuEntry;

typedef struct MenuPage {
    MenuEntry entries[MENU_MAX_ENTRIES];
    size_t num_entries;
    size_t highlighted_entry;
    char const *title;
} MenuPage;

typedef struct Menu {
    MenuPage pages[MENU_MAX_PAGES];
    size_t page_nav[MENU_MAX_PAGES];
    size_t page_depth;
    Camera camera;
    float camera_path;
} Menu;

void Menu_GoForward(Menu *menu, size_t page);
void Menu_GoBack(Menu *menu);

static inline MenuPage const *Menu_CurPage(Menu const *menu) {
    if (menu->page_depth >= 0 && menu->page_depth < MENU_MAX_PAGES) {
        return &menu->pages[menu->page_nav[menu->page_depth]];
    } else {
        return NULL;
    }
}

static inline MenuPage *Menu_CurPageMut(Menu *menu) {
    return (MenuPage*)Menu_CurPage(menu);
}

static inline void Menu_Restart(Menu *menu, size_t page) {
    if (page >= 0 && page < MENU_MAX_PAGES) {
        menu->page_depth = 0;
        menu->page_nav[0] = page;
    }
}

void MenuPage_AddEntry(MenuPage *page, MenuEntry const *entry);
void Menu_Update(Menu *menu, float delta_time, void *on_click_user);
void Menu_Draw(Menu const *menu);

void MainMenu_StartGame_Clicked(Menu *menu, MenuPage *page, void *user);
void MainMenu_Quit_Clicked(Menu *menu, MenuPage *page, void *user);
void MainMenu_Options_Clicked(Menu *menu, MenuPage *page, void *user);
void OptionsMenu_Back_Clicked(Menu *menu, MenuPage *page, void *user);
void OptionsMenu_Toggle_Waypoints_Clicked(Menu *menu, MenuPage *page, void *user);