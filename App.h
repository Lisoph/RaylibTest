#pragma once

#include "Menu.h"

#include <stdbool.h>
#include <raylib.h>
#include "SceneGraph.h"
#include "Train.h"

#define APP_MENU_MAIN 0
#define APP_MENU_OPTIONS 1

enum {
    APP_MODEL_MAP = 0,
    APP_MODEL_STORMTROOPER,
    APP_MODEL_DAISYHAT,
    APP_NUM_MODELS,
};

typedef struct App {
    bool quit;
    Menu menu;
    bool menu_open;
    float delta_time;
    Model models[APP_NUM_MODELS];
    Material materials[APP_NUM_MODELS];
    Camera game_camera;
    GraphNode *root_node;
    PathTrain map_navigation;
    bool draw_navigation;
} App;