#pragma once

#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_assert.h"
#include "nyangine/base/base_memory.h"
#include "nyangine/base/base_types.h"
#include "nyangine/math/math.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GRAPH DATA STRUCTURE
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 *
 * USAGE:
 *   // 1. Define your custom node and edge types
 *   typedef struct { char name[32]; } MyNode;
 *   typedef struct { f32 weight; } MyEdge;
 *
 *   // 2. Derive graph types and functions (call ONCE)
 *   nya_derive_graph(MyNode, MyEdge);
 *
 *   // 3. Use graph!
 *   MyNodeGraph graph = nya_MyNodeGraph_create(arena);
 *   u64 node_id = nya_MyNodeGraph_add_node(&graph, node_data);
 *   nya_MyNodeGraph_add_edge(&graph, from_id, to_id, edge_data);
 */

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GRAPH TYPE AND FUNCTION DEFINITION (SINGLE MACRO)
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */
#define nya_derive_graph(node_type, edge_type)                                                                                                       \
  /* Edge type definition */                                                                                                                         \
  typedef struct {                                                                                                                                   \
    u64       from;                                                                                                                                  \
    u64       to;                                                                                                                                    \
    edge_type edge;                                                                                                                                  \
  } node_type##_##edge_type##Edge;                                                                                                                   \
                                                                                                                                                     \
  /* Node type definition */                                                                                                                         \
  nya_derive_array(node_type##_##edge_type##Edge);                                                                                                   \
  typedef struct {                                                                                                                                   \
    u64                                node_id;                                                                                                      \
    node_type                          node;                                                                                                         \
    node_type##_##edge_type##EdgeArray edges;                                                                                                        \
  } node_type##GraphNode;                                                                                                                            \
                                                                                                                                                     \
  /* Graph type definition */                                                                                                                        \
  nya_derive_array(node_type##GraphNode);                                                                                                            \
  typedef struct {                                                                                                                                   \
    node_type##GraphNodeArray nodes;                                                                                                                 \
    NYA_Arena*                arena;                                                                                                                 \
  } node_type##Graph;                                                                                                                                \
                                                                                                                                                     \
  /* Graph functions */                                                                                                                              \
                                                                                                                                                     \
  static inline node_type##Graph nya_##node_type##Graph_create(NYA_Arena* arena) {                                                                   \
    return (node_type##Graph){ .nodes = nya_array_create(arena, node_type##GraphNode), .arena = arena };                                             \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline u64 nya_##node_type##Graph_add_node(node_type##Graph* graph, node_type node_data) {                                                  \
    u64                  node_index = graph->nodes.length;                                                                                           \
    node_type##GraphNode new_node   = { .node_id = node_index,                                                                                       \
                                        .node    = node_data,                                                                                        \
                                        .edges   = nya_array_create(graph->arena, node_type##_##edge_type##Edge) };                                    \
    nya_array_add(&graph->nodes, new_node);                                                                                                          \
    return node_index;                                                                                                                               \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline void nya_##node_type##Graph_add_edge(node_type##Graph* graph, u64 from_index, u64 to_index, edge_type edge_data) {                   \
    node_type##_##edge_type##Edge new_edge  = { .from = from_index, .to = to_index, .edge = edge_data };                                             \
    node_type##GraphNode*         from_node = &graph->nodes.items[from_index];                                                                       \
    nya_array_add(&from_node->edges, new_edge);                                                                                                      \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline void nya_##node_type##Graph_add_undirected_edge(node_type##Graph* graph, u64 from_index, u64 to_index, edge_type edge_data) {        \
    nya_##node_type##Graph_add_edge(graph, from_index, to_index, edge_data);                                                                         \
    nya_##node_type##Graph_add_edge(graph, to_index, from_index, edge_data);                                                                         \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline node_type##GraphNode* nya_##node_type##Graph_get_node(node_type##Graph* graph, u64 node_index) {                                     \
    return &graph->nodes.items[node_index];                                                                                                          \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline b8 nya_##node_type##Graph_has_edge(node_type##Graph* graph, u64 from_index, u64 to_index) {                                          \
    if (from_index >= graph->nodes.length || to_index >= graph->nodes.length) { return false; }                                                      \
    node_type##GraphNode* from_node = &graph->nodes.items[from_index];                                                                               \
    for (u64 edge_index = 0; edge_index < from_node->edges.length; ++edge_index) {                                                                   \
      if (from_node->edges.items[edge_index].to == to_index) { return true; }                                                                        \
    }                                                                                                                                                \
    return false;                                                                                                                                    \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline u64 nya_##node_type##Graph_edge_count(node_type##Graph* graph, u64 node_index) {                                                     \
    if (node_index >= graph->nodes.length) { return 0; }                                                                                             \
    return graph->nodes.items[node_index].edges.length;                                                                                              \
  }                                                                                                                                                  \
                                                                                                                                                     \
  static inline void nya_##node_type##Graph_destroy(node_type##Graph* graph) {                                                                       \
    for (u64 node_index = 0; node_index < graph->nodes.length; ++node_index) { nya_array_destroy(&graph->nodes.items[node_index].edges); }           \
    nya_array_destroy(&graph->nodes);                                                                                                                \
  }

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GRAPH UTILITY MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

// Graph-wide utilities
#define nya_graph_node_count(graph_ptr) ((graph_ptr)->nodes.length)
#define nya_graph_is_empty(graph_ptr)   ((graph_ptr)->nodes.length == 0)

#define nya_graph_total_edges(graph_ptr)                                                                                                             \
  ({                                                                                                                                                 \
    u64 total = 0;                                                                                                                                   \
    for (u64 node_index = 0; node_index < (graph_ptr)->nodes.length; ++node_index) { total += (graph_ptr)->nodes.items[node_index].edges.length; }   \
    total;                                                                                                                                           \
  })

#define nya_graph_degree(graph_ptr, node_index) (((node_index) < (graph_ptr)->nodes.length) ? (graph_ptr)->nodes.items[(node_index)].edges.length : 0)

// Iteration helpers
#define nya_graph_for_each_node(graph_ptr, node_var)                                                                                                 \
  for (u64 node_index = 0; node_index < (graph_ptr)->nodes.length && ((node_var) = &(graph_ptr)->nodes.items[node_index]); ++node_index)

#define nya_graph_for_each_edge(graph_ptr, from_node_index, edge_var)                                                                                \
  if ((from_node_index) < (graph_ptr)->nodes.length)                                                                                                 \
    for (u64 edge_index = 0; edge_index < (graph_ptr)->nodes.items[(from_node_index)].edges.length &&                                                \
                             ((edge_var) = &(graph_ptr)->nodes.items[(from_node_index)].edges.items[edge_index]);                                    \
         ++edge_index)

// Bounds checking helpers
#define nya_graph_node_exists(graph_ptr, node_index) ((node_index) < (graph_ptr)->nodes.length)
