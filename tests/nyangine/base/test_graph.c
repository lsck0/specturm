/**
 * THIS FILE WAS CLANKER WANKED !!!
 **/

#include "nyangine/nyangine.c"
#include "nyangine/nyangine.h"

// Simple test for graph
typedef struct {
  s32 value;
} SimpleNode;

typedef struct {
  f32 weight;
} SimpleEdge;

// Single macro for all graph types and functions
nya_derive_graph(SimpleNode, SimpleEdge);

s32 main(void) {
  NYA_Arena       arena = nya_arena_create(.name = "test_graph");
  SimpleNodeGraph graph = nya_SimpleNodeGraph_create(&arena);

  // Test basic functionality
  SimpleNode node1    = { .value = 42 };
  u64        node1_id = nya_SimpleNodeGraph_add_node(&graph, node1);

  SimpleNode node2    = { .value = 84 };
  u64        node2_id = nya_SimpleNodeGraph_add_node(&graph, node2);

  SimpleEdge edge = { .weight = 1.5F };
  nya_SimpleNodeGraph_add_edge(&graph, node1_id, node2_id, edge);

  // Test results
  SimpleNodeGraphNode* retrieved = nya_SimpleNodeGraph_get_node(&graph, node1_id);
  nya_assert(retrieved->node.value == 42);
  nya_assert(retrieved->node_id == node1_id);
  nya_assert(retrieved->edges.length == 1);

  // Test edge existence
  nya_assert(nya_SimpleNodeGraph_has_edge(&graph, node1_id, node2_id));
  nya_assert(!nya_SimpleNodeGraph_has_edge(&graph, node2_id, node1_id));

  // Test stats
  nya_assert(nya_graph_node_count(&graph) == 2);
  nya_assert(nya_graph_total_edges(&graph) == 1);

  printf("Simple graph test passed!\n");

  nya_SimpleNodeGraph_destroy(&graph);
  nya_arena_destroy(&arena);

  return 0;
}
