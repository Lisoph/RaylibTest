#pragma once

#include "SceneGraph.h"
#include "App.h"
#include "Train.h"

typedef struct TrooperNode {
    GraphNode node;
    PathTrain *train;
    float move_progress;
    float move_sign;
} TrooperNode;

TrooperNode Trooper_NewNode(App const *app);