#include "Train.h"

#include <stdlib.h>
#include "raylib.h"

static float PathTrain_TotalDistance(PathTrain const *train) {
    float total = 0.0f;
    PathTrainPoint const *previous = train->first_point;
    for (PathTrainPoint *next = train->first_point->next; next && next != train->first_point; next = next->next) {
        total += Vector3Distance(previous->position, next->position);
        previous = next;
    }
    return total;
}

static bool PathTrain_SegmentAt(PathTrain const *train, float t, PathTrainPoint const **p1, PathTrainPoint const **p2, float *total_distance, float *distance, float *previous_distance) {
    if (!train->first_point->next) {
        return false;
    }
    t = Clamp(t, 0.0f, 1.0f);

    PathTrainPoint const *previous = train->first_point;
    float const total_dist = PathTrain_TotalDistance(train);
    float const target_distance = total_dist * t;

    float cur_distance = 0.0f, prev_distance = 0.0f;

    for (PathTrainPoint *next = train->first_point->next; next && next != train->first_point; next = next->next) {
        float const dist = Vector3Distance(previous->position, next->position);
        cur_distance += dist;
        if (cur_distance >= target_distance) {
            if (p1) { *p1 = previous; }
            if (p2) { *p2 = next; }
            if (total_distance) { *total_distance = total_dist; }
            if (distance) { *distance = dist; }
            if (previous_distance) { *previous_distance = prev_distance; }
            return true;
        }

        previous = next;
        prev_distance = cur_distance;
    }

    return false;
}

Vector3 PathTrain_PositionAt(PathTrain const *train, float t) {
    PathTrainPoint const *p1, *p2;
    float total_distance, distance, prev_distance;
    if (PathTrain_SegmentAt(train, t, &p1, &p2, &total_distance, &distance, &prev_distance)) {
        float const target_distance = total_distance * t;
        return Vector3Lerp(p1->position, p2->position, (target_distance - prev_distance) / distance);
    } else {
        return (Vector3) { NAN, NAN, NAN };
    }
}

Vector3 PathTrain_DirectionAt(PathTrain const *train, float t, bool reverse) {
    PathTrainPoint const *p1, *p2;
    if (PathTrain_SegmentAt(train, t, &p1, &p2, NULL, NULL, NULL)) {
        if (reverse) {
            PathTrainPoint const *tmp = p1;
            p1 = p2;
            p2 = tmp;
        }
        return Vector3Normalize(Vector3Subtract(p2->position, p1->position));
    } else {
        return (Vector3) { NAN, NAN, NAN };
    }
}

void PathTrain_AddPosition(PathTrain *train, Vector3 position) {
    PathTrainPoint *new_point = calloc(1, sizeof(PathTrainPoint));
    new_point->position = position;
    
    if (!train->first_point) {
        train->first_point = new_point;
    }

    if (train->last_point) {
        train->last_point->next = new_point;
    }

    new_point->previous = train->last_point;
    train->last_point = new_point;
}

void PathTrain_Destroy(PathTrain *train) {
    PathTrainPoint *point = train->first_point;
    for (;;) {
        PathTrainPoint *next = point->next;
        free(point);
        if (next && next != train->first_point) {
            point = next;
        } else {
            break;
        }
    }
}
void PathTrain_Draw(PathTrain const *train) {
    PathTrainPoint *previous = train->first_point;
    for (PathTrainPoint *point = train->first_point->next; point; point = point->next) {
        DrawLine3D(previous->position, point->position, GREEN);

        if (point->next == train->first_point) {
            break;
        }

        previous = point;
    }
}