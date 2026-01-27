/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

// Simple test for graph
typedef struct {
  s32 value;
} City;

typedef struct {
  f32 weight;
} Road;

// Derive graph types only
nya_derive_graph(City, Road);

s32 main(void) {
  NYA_Arena arena = nya_arena_create(.name = "test_graph");
  CityGraph graph = nya_graph_create(&arena, City, Road);

  // Test basic functionality
  City node1    = { .value = 42 };
  u64  node1_id = nya_graph_add_node(&graph, node1);

  City node2    = { .value = 84 };
  u64  node2_id = nya_graph_add_node(&graph, node2);

  Road edge = { .weight = 1.5F };
  nya_graph_add_edge(&graph, node1_id, node2_id, edge);

  // Test results
  CityGraphNode* retrieved = nya_graph_get_node(&graph, node1_id);
  nya_assert(retrieved->node.value == 42);
  nya_assert(retrieved->node_id == node1_id);
  nya_assert(retrieved->edges.length == 1);

  // Test edge existence
  nya_assert(nya_graph_has_edge(&graph, node1_id, node2_id));
  nya_assert(!nya_graph_has_edge(&graph, node2_id, node1_id));

  // Test stats
  nya_assert(nya_graph_node_count(&graph) == 2);
  nya_assert(nya_graph_total_edges(&graph) == 1);

  // Test neighbor iteration
  u64 neighbor_count = 0;
  nya_graph_for_each_neighbor(&graph, node1_id, neighbor) {
    neighbor_count++;
    (void)neighbor; // Suppress unused variable warning
  }
  nya_assert(neighbor_count == 1);

  // Test neighbor retrieval
  City* neighbors = NULL;
  nya_graph_get_neighbors(&graph, node1_id, neighbors);
  nya_assert(neighbors && neighbors[0].value == 84);

  // Test connectivity
  nya_assert(nya_graph_are_connected(&graph, node1_id, node2_id));
  nya_assert(!nya_graph_are_connected(&graph, node2_id, node1_id));

  // Test find functionality
  CityGraphNode* found_node = nya_graph_find_node(&graph, _nya_node->node.value == 84);
  nya_assert(found_node && found_node->node.value == 84);

  // Test count by criteria
  u64 high_value_count = nya_graph_count_nodes(&graph, found_node->node.value > 40);
  nya_assert(high_value_count == 2);

  // Test isolation and indegree
  nya_assert(!nya_graph_is_isolated(&graph, node1_id));
  nya_assert(nya_graph_is_isolated(&graph, node2_id));
  nya_assert(nya_graph_indegree(&graph, node2_id) == 1);

  printf("Simple graph test passed!\n");

  nya_graph_destroy(&graph);
  nya_arena_destroy(&arena);

  return 0;
}
