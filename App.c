#include "App.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <raylib.h>
#include "raymath.h"
#include "SceneGraph.h"
#include "Trooper.h"

#define VIDEO_WIDTH 1280
#define VIDEO_HEIGHT 720
#define FPS 144

static void App_Update(App *app);
static void App_Draw(App *app);
static void LoadModels(App *app);
static void UnloadModels(App *app);

static void App_Init(App *app) {
    app->quit = false;
    app->menu_open = true;
    app->delta_time = 1.0;

    app->menu.camera = (Camera) {
        .position = (Vector3) { 0.0, 0.0, -5.0 },
        .target = (Vector3) { 0.0, 0.0, 0.0 },
        .up = (Vector3) { 0.0, 1.0, 0.0 },
        .fovy = 60.0f,
    };
    app->game_camera.position = (Vector3) { 0.0, 2.0, 10.0 };
    app->game_camera.target = (Vector3) { 0.0, 0.0, 0.0 };
    app->game_camera.up = (Vector3) { 0.0, 1.0, 0.0 };
    app->game_camera.fovy = 60.0f;

    app->menu.pages[APP_MENU_MAIN].title = "Main Menu";
    MenuPage_AddEntry(&app->menu.pages[APP_MENU_MAIN], &(MenuEntry){.text = "Start game", .click = MainMenu_StartGame_Clicked });
    MenuPage_AddEntry(&app->menu.pages[APP_MENU_MAIN], &(MenuEntry){.text = "Options", .click = MainMenu_Options_Clicked });
    MenuPage_AddEntry(&app->menu.pages[APP_MENU_MAIN], &(MenuEntry){.text = "Quit", .click = MainMenu_Quit_Clicked });

    app->menu.pages[APP_MENU_OPTIONS].title = "Options";
    MenuPage_AddEntry(&app->menu.pages[APP_MENU_OPTIONS], &(MenuEntry){.text = "No options yet", .click = NULL });
    MenuPage_AddEntry(&app->menu.pages[APP_MENU_OPTIONS], &(MenuEntry){.text = "Display waypoints (Toggle)", .click = OptionsMenu_Toggle_Waypoints_Clicked });
    MenuPage_AddEntry(&app->menu.pages[APP_MENU_OPTIONS], &(MenuEntry){.text = "Back", .click = OptionsMenu_Back_Clicked });

    LoadModels(app);

    app->root_node = malloc(sizeof(GraphNode));
    *app->root_node = Node_New(NULL, MatrixIdentity());
    void RotateNode(GraphNode *node, void *user);
    //app->root_node->update = RotateNode;

    GraphNode *world = malloc(sizeof(GraphNode));
    *world = Node_New(&app->models[APP_MODEL_MAP], MatrixIdentity());
    Node_AddChild(app->root_node, world);

    TrooperNode *trooper = malloc(sizeof(TrooperNode));
    *trooper = Trooper_NewNode(app);
    Node_AddChild(world, (GraphNode*)trooper);

    GraphNode *trooper_hat = malloc(sizeof(GraphNode));
    *trooper_hat = Node_New(&app->models[APP_MODEL_DAISYHAT], MatrixIdentity());
    Node_AddChild((GraphNode*)trooper, trooper_hat);

    PathTrain_AddPosition(&app->map_navigation, (Vector3) { -16.167286f, -1.5f, 13.490955f });
    PathTrain_AddPosition(&app->map_navigation, (Vector3) { -0.071678f, -1.5f, 13.499234f });
    PathTrain_AddPosition(&app->map_navigation, (Vector3) { 0.909596f, -1.5f, 11.413073f });
    PathTrain_AddPosition(&app->map_navigation, (Vector3) { 1.277305f, -1.5f, 5.983274f });
    PathTrain_AddPosition(&app->map_navigation, (Vector3) { 2.515599f, -1.5f, 4.818539f });
    PathTrain_AddPosition(&app->map_navigation, (Vector3) { 7.745001f, -1.5f, 4.527215f });

    trooper->train = &app->map_navigation;
}

static void LoadModels(App *app) {
    app->models[APP_MODEL_MAP] = LoadModel("castle\\castle_map_complete.obj");
    app->models[APP_MODEL_MAP].material = app->materials[APP_MODEL_MAP] = LoadMaterial("castle\\castle_map_complete.mtl");
    app->models[APP_MODEL_STORMTROOPER] = LoadModel("stormtrooper\\SW-RS_PC_ENEMY_Storm_Trooper.obj");
    app->models[APP_MODEL_STORMTROOPER].material = app->materials[APP_MODEL_MAP] = LoadMaterial("stormtrooper\\SW-RS_PC_ENEMY_Storm_Trooper.mtl");
    app->models[APP_MODEL_STORMTROOPER].transform = MatrixMultiply(MatrixRotateY(DEG2RAD * 180.0f), MatrixScale(2.0f, 2.0f, 2.0f));
    app->models[APP_MODEL_DAISYHAT] = LoadModel("Daisy Hat\\hat_daisy_model.obj");
    app->models[APP_MODEL_DAISYHAT].material = app->materials[APP_MODEL_DAISYHAT] = LoadMaterial("Daisy Hat\\hat_daisy_model.mtl");
    app->models[APP_MODEL_DAISYHAT].transform = MatrixMultiply(MatrixMultiply(MatrixRotateY(DEG2RAD * 180.0f), MatrixScale(0.011f, 0.011f, 0.011f)), MatrixTranslate(0.0f, 3.08201f, -0.02171f));
}

static void UnloadModels(App *app) {
    UnloadModel(app->models[APP_MODEL_MAP]);
    UnloadMaterial(app->materials[APP_MODEL_MAP]);
    UnloadModel(app->models[APP_MODEL_STORMTROOPER]);
    UnloadMaterial(app->materials[APP_MODEL_STORMTROOPER]);
    UnloadModel(app->models[APP_MODEL_DAISYHAT]);
    UnloadMaterial(app->materials[APP_MODEL_DAISYHAT]);
}

static void App_Destroy(App *app) {
    PathTrain_Destroy(&app->map_navigation);
    Node_Destroy(app->root_node);
    UnloadModels(app);
}

void App_Main() {
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_MSAA_4X_HINT);
    SetExitKey(0);
    InitWindow(VIDEO_WIDTH, VIDEO_HEIGHT, "App");

    App app = {0};
    App_Init(&app);

    while (!WindowShouldClose() && !app.quit)
    {
        App_Update(&app);
        if (!IsWindowMinimized()) {
            BeginDrawing();
            App_Draw(&app);
            EndDrawing();
        }

        float elapsed = GetFrameTime();
        app.delta_time = elapsed / (FPS / 1000.0f);
    }

    App_Destroy(&app);

    CloseWindow();
}

static void App_Update(App *app) {
    if (app->menu_open) {
        Menu_Update(&app->menu, app->delta_time, app);
    } else {
        if (IsKeyPressed(KEY_ESCAPE)) {
            Menu_Restart(&app->menu, APP_MENU_MAIN);
            app->menu_open = true;
            SetCameraMode(app->menu.camera, CAMERA_CUSTOM);
        }

        UpdateCamera(&app->game_camera);
        Node_Update(app->root_node, app);
    }
}

static void App_Draw(App *app) {
    ClearBackground(RAYWHITE);
    if (app->menu_open) {
        Menu_Draw(&app->menu);
    } else {
        Begin3dMode(app->game_camera);
        Node_Draw(app->root_node);
        if (app->draw_navigation) {
            PathTrain_Draw(&app->map_navigation);
        }
        End3dMode();

        {
            static char buf[100] = {0};
            Vector3 *v = &app->game_camera.position;
            sprintf_s(buf, 100, "%f\n%f\n%f\n", v->x, v->y, v->z);
            DrawText(buf, 10, 10, 20, MAROON);
        }
    }
}

static void RotateNode(GraphNode *node, void *user) {
    App *app = (App*)user;
    Matrix rot = MatrixRotateY(app->delta_time * 0.01f);
    node->transform = MatrixMultiply(node->transform, rot);
}