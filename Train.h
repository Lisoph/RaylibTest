#pragma once

#include "raymath.h"

#include <stdbool.h>

typedef struct PathTrainPoint {
    Vector3 position;
    struct PathTrainPoint *next;
    struct PathTrainPoint *previous;
} PathTrainPoint;

typedef struct PathTrain
{
    PathTrainPoint *first_point;
    PathTrainPoint *last_point;
} PathTrain;

Vector3 PathTrain_PositionAt(PathTrain const *train, float t);
Vector3 PathTrain_DirectionAt(PathTrain const *train, float t, bool reverse);
void PathTrain_AddPosition(PathTrain *train, Vector3 position);
void PathTrain_Destroy(PathTrain *train);
void PathTrain_Draw(PathTrain const *train);