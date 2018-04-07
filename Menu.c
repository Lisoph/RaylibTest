#pragma once

#include "Menu.h"

#include <string.h>
#include <math.h>
#include "raylib.h"
#include "App.h"

#define PAGE_FONT_SIZE 40
#define ENTRY_FONT_SIZE 20

void Menu_GoForward(Menu *menu, size_t page) {
    if (menu->page_depth >= 0 && menu->page_depth < MENU_MAX_PAGES) {
        if (page >= 0 && page < MENU_MAX_PAGES) {
            ++menu->page_depth;
            menu->page_nav[menu->page_depth] = page;
            menu->pages[page].highlighted_entry = 0;
        }
    }
}

void Menu_GoBack(Menu *menu) {
    if (menu->page_depth > 0) {
        --menu->page_depth;
    }
}

void MenuPage_AddEntry(MenuPage *page, MenuEntry const *entry) {
    if (page->num_entries >= 0 && page->num_entries < MENU_MAX_ENTRIES) {
        memcpy(&page->entries[page->num_entries], entry, sizeof(MenuEntry));
        ++page->num_entries;
    }
}

void Menu_Update(Menu *menu, float delta_time, void *on_click_user) {
    menu->camera_path += 0.05f * delta_time;
    menu->camera.position.z = cosf(menu->camera_path / 4.0f) * 2.0f;
    menu->camera.position.x = sinf(menu->camera_path / 8.0f) * 2.0f;
    menu->camera.position.y = sinf(menu->camera_path) * 2.0f;

    MenuPage *cur_page = Menu_CurPageMut(menu);
    if (cur_page) {
        int const width = GetScreenWidth();
        int const height = GetScreenHeight();
        Vector2 const mouse_pos = GetMousePosition();

        int const title_width = MeasureText(cur_page->title, PAGE_FONT_SIZE);

        for (size_t i = 0; i < cur_page->num_entries; ++i) {
            MenuEntry *cur_entry = &cur_page->entries[i];

            int const text_width = MeasureText(cur_entry->text, ENTRY_FONT_SIZE);
            cur_entry->computed_bbox.x = width / 2 - text_width / 2;
            cur_entry->computed_bbox.y = PAGE_FONT_SIZE * 2 + (int)i * ENTRY_FONT_SIZE;
            cur_entry->computed_bbox.width = text_width;
            cur_entry->computed_bbox.height = ENTRY_FONT_SIZE;

            cur_entry->highlight_timer = cur_entry->highlight_timer > 0.0f ? cur_entry->highlight_timer - delta_time : 0.0f;

            if (CheckCollisionPointRec(mouse_pos, cur_entry->computed_bbox)) {
                cur_page->highlighted_entry = i;

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    cur_entry->highlight_timer = 1.0f;
                }

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && cur_entry->click) {
                    cur_entry->click(menu, cur_page, on_click_user);
                }
            }
        }

        if (IsKeyPressed(KEY_DOWN)) {
            if (cur_page->highlighted_entry >= cur_page->num_entries - 1) {
                cur_page->highlighted_entry = 0;
            } else {
                ++cur_page->highlighted_entry;
            }
        }
        if (IsKeyPressed(KEY_UP)) {
            if (cur_page->highlighted_entry <= 0) {
                cur_page->highlighted_entry = cur_page->num_entries - 1;
            } else {
                --cur_page->highlighted_entry;
            }
        }
        if (IsKeyPressed(KEY_ENTER)) {
            if (cur_page->highlighted_entry >= 0 && cur_page->highlighted_entry < cur_page->num_entries) {
                MenuEntry *cur_entry = &cur_page->entries[cur_page->highlighted_entry];
                if (cur_entry->click) {
                    cur_entry->highlight_timer = 1.0f;
                    cur_entry->click(menu, cur_page, on_click_user);
                }
            }
        }
    }
}

void Menu_Draw(Menu const *menu) {
    Begin3dMode(menu->camera);
    DrawSphereWires((Vector3) { 0.0, 0.0, 0.0 }, 1.0, 18, 32, LIGHTGRAY);
    End3dMode();

    MenuPage const *cur_page = Menu_CurPage(menu);
    if (cur_page) {
        int const width = GetScreenWidth();
        int const height = GetScreenHeight();

        int const title_width = MeasureText(cur_page->title, PAGE_FONT_SIZE);
        DrawText(cur_page->title, width / 2 - title_width / 2, 10, PAGE_FONT_SIZE, MAROON);

        for (size_t i = 0; i < cur_page->num_entries; ++i) {
            MenuEntry const *cur_entry = &cur_page->entries[i];
            Color color = cur_page->highlighted_entry == i ? MAROON : GRAY;
            Rectangle const *rect = &cur_entry->computed_bbox;

            if (cur_entry->highlight_timer > 0.0f) {
                Color color = DARKGRAY;
                color.a = (unsigned char)roundf(cur_entry->highlight_timer * 50.0f);
                DrawRectangle(rect->x, rect->y, rect->width, rect->height, color);
            }

            DrawText(cur_entry->text, rect->x, rect->y, ENTRY_FONT_SIZE, color);
        }
    }
}

void MainMenu_StartGame_Clicked(Menu *menu, MenuPage *page, void *user) {
    App *app = (App*)user;
    app->menu_open = false;
    SetCameraMode(app->game_camera, CAMERA_FIRST_PERSON);
}

void MainMenu_Quit_Clicked(Menu *menu, MenuPage *page, void *user) {
    App *app = (App*)user;
    app->quit = true;
}

void MainMenu_Options_Clicked(Menu *menu, MenuPage *page, void *user) {
    Menu_GoForward(menu, APP_MENU_OPTIONS);
}

void OptionsMenu_Back_Clicked(Menu *menu, MenuPage *page, void *user) {
    Menu_GoBack(menu);
}

void OptionsMenu_Toggle_Waypoints_Clicked(Menu *menu, MenuPage *page, void *user) {
    App *app = (App*)user;
    app->draw_navigation = !app->draw_navigation;
}