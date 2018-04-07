#include "SceneGraph.h"

#include <stdlib.h>
#include "raymath.h"

void DrawModelRaw(Model model, Matrix transform);

static void Node_DrawEx(GraphNode *node, Matrix parent_transform) {
    Matrix transform = MatrixMultiply(node->transform, parent_transform);

    if (node->model) {
        DrawModelRaw(*node->model, transform);
    }

    for (GraphNode *child = node->first_child; child; child = child->next_sibling) {
        Node_DrawEx(child, transform);
    }
}

void Node_Destroy(GraphNode *root_node) {
    for (GraphNode *child = root_node->first_child; child;) {
        GraphNode *next = child->next_sibling;
        Node_Destroy(child);
        child = next;
    }

    free(root_node);
}

void Node_Update(GraphNode *node, void *user) {
    node->update(node, user);

    for (GraphNode *child = node->first_child; child; child = child->next_sibling) {
        Node_Update(child, user);
    }
}

void Node_Draw(GraphNode *node) {
    Node_DrawEx(node, MatrixIdentity());
}

static void DrawModelRaw(Model model, Matrix transform) {
    void rlDrawMesh(Mesh mesh, Material material, Matrix transform);
    rlDrawMesh(model.mesh, model.material, MatrixMultiply(model.transform, transform));
}

void Node_Update_Stub(GraphNode *node, void *user) {}