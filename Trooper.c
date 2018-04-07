#include "Trooper.h"

#include "raymath.h"

static inline PathTrainPoint PathTrain_NewPoint(Vector3 position) {
    PathTrainPoint point = {0};
    point.position = position;
    return point;
}

static inline void PathTrainPoint_AddPoint(PathTrainPoint *point, PathTrainPoint *new_point) {
    point->next = new_point;
    new_point->previous = point;
}

static void TrooperNode_Update(GraphNode *node, void *user) {
    TrooperNode *trooper = (TrooperNode*)node;
    App *app = (App*)user;

    if (trooper->train) {
        Vector3 pos = PathTrain_PositionAt(trooper->train, trooper->move_progress);
        Vector3 dir = PathTrain_DirectionAt(trooper->train, trooper->move_progress, trooper->move_sign < 0.0f);
        Vector3 const forward = (Vector3) { 0.0f, 0.0f, -1.0f };
        Vector3 axis = Vector3Normalize(Vector3CrossProduct(forward, dir));
        float angle = acosf(Vector3DotProduct(dir, forward));
        Matrix look_at = MatrixRotate(axis, angle);
        trooper->node.transform = MatrixMultiply(look_at, MatrixTranslate(pos.x, pos.y, pos.z));

        if (trooper->move_progress >= 1.0f) {
            trooper->move_sign = -1.0f;
        } else if (trooper->move_progress <= 0.0f) {
            trooper->move_sign = 1.0;
        }
        trooper->move_progress += 0.01f * trooper->move_sign * app->delta_time;
    }
}

TrooperNode Trooper_NewNode(App const *app) {
    TrooperNode trooper = {0};
    trooper.node = Node_New(&app->models[APP_MODEL_STORMTROOPER], MatrixMultiply(MatrixRotateY(DEG2RAD * 90.0f), MatrixTranslate(-16.167286f, -1.5f, 13.490955f)));
    trooper.node.update = TrooperNode_Update;
    trooper.move_sign = 1.0;
    return trooper;
}