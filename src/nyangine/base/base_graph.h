#pragma once

#include "nyangine/base/base.h"
#include "nyangine/base/base_arena.h"
#include "nyangine/base/base_array.h"
#include "nyangine/base/base_assert.h"
#include "nyangine/base/base_memory.h"
#include "nyangine/base/base_types.h"

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────
 * TYPES
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_derive_graph(node_type, edge_type)                                                                                                       \
  typedef struct {                                                                                                                                   \
    u64       from;                                                                                                                                  \
    u64       to;                                                                                                                                    \
    edge_type edge;                                                                                                                                  \
  } node_type##_##edge_type##Edge;                                                                                                                   \
  nya_derive_array(node_type##_##edge_type##Edge);                                                                                                   \
                                                                                                                                                     \
  typedef struct {                                                                                                                                   \
    u64                                node_id;                                                                                                      \
    node_type                          node;                                                                                                         \
    node_type##_##edge_type##EdgeArray edges;                                                                                                        \
  } node_type##GraphNode;                                                                                                                            \
  nya_derive_array(node_type##GraphNode);                                                                                                            \
                                                                                                                                                     \
  typedef struct {                                                                                                                                   \
    node_type##GraphNodeArray nodes;                                                                                                                 \
    NYA_Arena*                arena;                                                                                                                 \
  } node_type##Graph

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * CREATION MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_graph_create(arena_ptr, node_type, edge_type)                                                                                            \
  (node_type##Graph) {                                                                                                                               \
    .nodes = nya_array_create(arena_ptr, node_type##GraphNode), .arena = (arena_ptr),                                                                \
  }

#define nya_graph_destroy(graph_ptr)                                                                                                                 \
  ({                                                                                                                                                 \
    for (u64 _nya_i = 0; _nya_i < (graph_ptr)->nodes.length; ++_nya_i) { nya_array_destroy(&(graph_ptr)->nodes.items[_nya_i].edges); }               \
    nya_array_destroy(&(graph_ptr)->nodes);                                                                                                          \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * NODE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_graph_add_node(graph_ptr, node_data)                                                                                                     \
  ({                                                                                                                                                 \
    typeof((graph_ptr)->nodes.items[0]) _nya_new_node;                                                                                               \
    _nya_new_node.node    = (node_data);                                                                                                             \
    _nya_new_node.node_id = (graph_ptr)->nodes.length;                                                                                               \
    _nya_new_node.edges   = (typeof((graph_ptr)->nodes.items[0].edges)){                                                                             \
        .items    = nya_arena_alloc((graph_ptr)->arena, _NYA_ARRAY_DEFAULT_CAPACITY * sizeof(typeof((graph_ptr)->nodes.items[0].edges.items[0]))),   \
        .length   = 0,                                                                                                                               \
        .capacity = _NYA_ARRAY_DEFAULT_CAPACITY,                                                                                                     \
        .arena    = (graph_ptr)->arena                                                                                                               \
    };                                                                                                                                               \
    nya_array_add(&(graph_ptr)->nodes, _nya_new_node);                                                                                               \
    _nya_new_node.node_id;                                                                                                                           \
  })

#define nya_graph_get_node(graph_ptr, node_index)                                                                                                    \
  ({                                                                                                                                                 \
    nya_assert(                                                                                                                                      \
        (node_index) < (graph_ptr)->nodes.length,                                                                                                    \
        "Graph node index " FMTu64 " out of bounds (length: " FMTu64 ")",                                                                            \
        (node_index),                                                                                                                                \
        (graph_ptr)->nodes.length                                                                                                                    \
    );                                                                                                                                               \
    &(graph_ptr)->nodes.items[node_index];                                                                                                           \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * GRAPH EDGE MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_graph_add_edge(graph_ptr, from_index, to_index, edge_data)                                                                               \
  ({                                                                                                                                                 \
    nya_assert(                                                                                                                                      \
        (from_index) < (graph_ptr)->nodes.length,                                                                                                    \
        "Graph from_node index " FMTu64 " out of bounds (length: " FMTu64 ")",                                                                       \
        (from_index),                                                                                                                                \
        (graph_ptr)->nodes.length                                                                                                                    \
    );                                                                                                                                               \
    nya_assert(                                                                                                                                      \
        (to_index) < (graph_ptr)->nodes.length,                                                                                                      \
        "Graph to_node index " FMTu64 " out of bounds (length: " FMTu64 ")",                                                                         \
        (to_index),                                                                                                                                  \
        (graph_ptr)->nodes.length                                                                                                                    \
    );                                                                                                                                               \
    typeof((graph_ptr)->nodes.items[0].edges.items[0]) _nya_new_edge  = { .from = (from_index), .to = (to_index), .edge = (edge_data) };             \
    typeof((graph_ptr)->nodes.items[0])*               _nya_from_node = &(graph_ptr)->nodes.items[from_index];                                       \
    nya_array_add(&_nya_from_node->edges, _nya_new_edge);                                                                                            \
  })

#define nya_graph_add_undirected_edge(graph_ptr, from_index, to_index, edge_data)                                                                    \
  ({                                                                                                                                                 \
    nya_graph_add_edge((graph_ptr), (from_index), (to_index), (edge_data));                                                                          \
    nya_graph_add_edge((graph_ptr), (to_index), (from_index), (edge_data));                                                                          \
  })

#define nya_graph_has_edge(graph_ptr, from_index, to_index)                                                                                          \
  ({                                                                                                                                                 \
    if ((from_index) >= (graph_ptr)->nodes.length || (to_index) >= (graph_ptr)->nodes.length) { return false; }                                      \
    typeof((graph_ptr)->nodes.items[0])* _nya_from_node = &(graph_ptr)->nodes.items[from_index];                                                     \
    b8                                   _nya_found     = false;                                                                                     \
    nya_array_for (&_nya_from_node->edges, edge_index) {                                                                                             \
      if (_nya_from_node->edges.items[edge_index].to == (to_index)) {                                                                                \
        _nya_found = true;                                                                                                                           \
        break;                                                                                                                                       \
      }                                                                                                                                              \
    }                                                                                                                                                \
    _nya_found;                                                                                                                                      \
  })

#define nya_graph_edge_count(graph_ptr, node_index)                                                                                                  \
  ({                                                                                                                                                 \
    if ((node_index) >= (graph_ptr)->nodes.length) { return 0; }                                                                                     \
    (graph_ptr)->nodes.items[node_index].edges.length;                                                                                               \
  })

/*
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 * UTILITY MACROS
 * ─────────────────────────────────────────────────────────────────────────────────────────────────────────────
 */

#define nya_graph_node_count(graph_ptr) ((graph_ptr)->nodes.length)
#define nya_graph_is_empty(graph_ptr)   ((graph_ptr)->nodes.length == 0)

#define nya_graph_total_edges(graph_ptr)                                                                                                             \
  ({                                                                                                                                                 \
    u64 total = 0;                                                                                                                                   \
    for (u64 node_index = 0; node_index < (graph_ptr)->nodes.length; ++node_index) { total += (graph_ptr)->nodes.items[node_index].edges.length; }   \
    total;                                                                                                                                           \
  })

#define nya_graph_degree(graph_ptr, node_index) (((node_index) < (graph_ptr)->nodes.length) ? (graph_ptr)->nodes.items[(node_index)].edges.length : 0)

#define nya_graph_for_each_node(graph_ptr, node_var)                                                                                                 \
  for (typeof((graph_ptr)->nodes.items[0])* node_var = (graph_ptr)->nodes.items; node_var < (graph_ptr)->nodes.items + (graph_ptr)->nodes.length;    \
       ++node_var)

#define nya_graph_for_each_edge(graph_ptr, from_node_index, edge_var)                                                                                \
  if ((from_node_index) < (graph_ptr)->nodes.length)                                                                                                 \
    for (typeof((graph_ptr)->nodes.items[0].edges.items[0])* edge_var = (graph_ptr)->nodes.items[from_node_index].edges.items;                       \
         edge_var < (graph_ptr)->nodes.items[from_node_index].edges.items + (graph_ptr)->nodes.items[from_node_index].edges.length;                  \
         ++edge_var)

#define nya_graph_for_each_neighbor(graph_ptr, from_node_index, neighbor_var)                                                                        \
  if ((from_node_index) < (graph_ptr)->nodes.length)                                                                                                 \
    for (typeof((graph_ptr)->nodes.items[0].edges.items[0])* edge_var = (graph_ptr)->nodes.items[from_node_index].edges.items;                       \
         edge_var < (graph_ptr)->nodes.items[from_node_index].edges.items + (graph_ptr)->nodes.items[from_node_index].edges.length;                  \
         ++edge_var)                                                                                                                                 \
      for (typeof((graph_ptr)->nodes.items[0])* neighbor_var = &(graph_ptr)->nodes.items[edge_var->to]; neighbor_var != NULL; neighbor_var = NULL)

#define nya_graph_get_neighbors(graph_ptr, node_index, neighbors)                                                                                    \
  ({                                                                                                                                                 \
    if ((node_index) < (graph_ptr)->nodes.length) {                                                                                                  \
      (neighbors) = nya_arena_alloc(                                                                                                                 \
          (graph_ptr)->arena,                                                                                                                        \
          ((graph_ptr)->nodes.items[node_index].edges.length + 1) * sizeof(typeof((graph_ptr)->nodes.items[0].node))                                 \
      );                                                                                                                                             \
      u64 _nya_count = 0;                                                                                                                            \
      nya_array_for (&(graph_ptr)->nodes.items[node_index].edges, edge_index) {                                                                      \
        if ((graph_ptr)->nodes.items[node_index].edges.items[edge_index].to != (node_index)) {                                                       \
          (neighbors)[_nya_count++] = (graph_ptr)->nodes.items[(graph_ptr)->nodes.items[node_index].edges.items[edge_index].to].node;                \
        }                                                                                                                                            \
      }                                                                                                                                              \
      (neighbors)[_nya_count] = (typeof((graph_ptr)->nodes.items[0].node)){ 0 }; /* Terminate list */                                                \
    }                                                                                                                                                \
  })

#define nya_graph_are_connected(graph_ptr, from_index, to_index)                                                                                     \
  ({                                                                                                                                                 \
    if ((from_index) >= (graph_ptr)->nodes.length || (to_index) >= (graph_ptr)->nodes.length) { return false; }                                      \
    b8* _nya_visited = nya_arena_alloc((graph_ptr)->arena, (graph_ptr)->nodes.length * sizeof(b8));                                                  \
    for (u64 i = 0; i < (graph_ptr)->nodes.length; ++i) { _nya_visited[i] = false; }                                                                 \
    _nya_visited[(from_index)] = true;                                                                                                               \
                                                                                                                                                     \
    /* Simple BFS queue */                                                                                                                           \
    u64* _nya_queue      = nya_arena_alloc((graph_ptr)->arena, (graph_ptr)->nodes.length * sizeof(u64));                                             \
    u64  _nya_queue_head = 0, _nya_queue_tail = 1;                                                                                                   \
    _nya_queue[0] = (from_index);                                                                                                                    \
                                                                                                                                                     \
    b8 _nya_found = false;                                                                                                                           \
    while (_nya_queue_head < _nya_queue_tail && !_nya_found) {                                                                                       \
      u64 _nya_current = _nya_queue[_nya_queue_head++];                                                                                              \
      if (_nya_current == (to_index)) {                                                                                                              \
        _nya_found = true;                                                                                                                           \
        break;                                                                                                                                       \
      }                                                                                                                                              \
                                                                                                                                                     \
      nya_array_for (&(graph_ptr)->nodes.items[_nya_current].edges, edge_index) {                                                                    \
        u64 _nya_neighbor = (graph_ptr)->nodes.items[_nya_current].edges.items[edge_index].to;                                                       \
        if (!_nya_visited[_nya_neighbor]) {                                                                                                          \
          _nya_visited[_nya_neighbor]   = true;                                                                                                      \
          _nya_queue[_nya_queue_tail++] = _nya_neighbor;                                                                                             \
        }                                                                                                                                            \
      }                                                                                                                                              \
    }                                                                                                                                                \
    _nya_found;                                                                                                                                      \
  })

#define nya_graph_find_node(graph_ptr, predicate_expr)                                                                                               \
  ({                                                                                                                                                 \
    typeof((graph_ptr)->nodes.items[0])* _nya_found_node = 0;                                                                                        \
    nya_graph_for_each_node((graph_ptr), _nya_node) {                                                                                                \
      if (predicate_expr) {                                                                                                                          \
        _nya_found_node = _nya_node;                                                                                                                 \
        break;                                                                                                                                       \
      }                                                                                                                                              \
    }                                                                                                                                                \
    _nya_found_node;                                                                                                                                 \
  })

#define nya_graph_count_nodes(graph_ptr, predicate_expr)                                                                                             \
  ({                                                                                                                                                 \
    u64 _nya_count = 0;                                                                                                                              \
    nya_graph_for_each_node((graph_ptr), _nya_node) {                                                                                                \
      if (predicate_expr) { ++_nya_count; }                                                                                                          \
    }                                                                                                                                                \
    _nya_count;                                                                                                                                      \
  })

#define nya_graph_is_isolated(graph_ptr, node_index)                                                                                                 \
  ({                                                                                                                                                 \
    if ((node_index) >= (graph_ptr)->nodes.length) { return true; }                                                                                  \
    (graph_ptr)->nodes.items[node_index].edges.length == 0;                                                                                          \
  })

#define nya_graph_indegree(graph_ptr, node_index)                                                                                                    \
  ({                                                                                                                                                 \
    if ((node_index) >= (graph_ptr)->nodes.length) { return 0; }                                                                                     \
    u64 _nya_indegree = 0;                                                                                                                           \
    nya_graph_for_each_node((graph_ptr), _nya_node) {                                                                                                \
      nya_array_for (&_nya_node->edges, edge_index) {                                                                                                \
        if (_nya_node->edges.items[edge_index].to == (node_index)) { ++_nya_indegree; }                                                              \
      }                                                                                                                                              \
    }                                                                                                                                                \
    _nya_indegree;                                                                                                                                   \
  })

#define nya_graph_node_exists(graph_ptr, node_index) ((node_index) < (graph_ptr)->nodes.length)
