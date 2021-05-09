#pragma once

#include <boost/preprocessor.hpp>


// Defines a sequence of tuples, each containing the arguments for one invocation of NodeFn.
// Note: all nodes are defined before they are used as a connection.
#define FP2_GRAPH_DATA                                                                          \
((0, N1, 0, 1))((1, N4, 1, 1, N1))((2, N3, 1, 0, N4))((3, N0, 0, 0, N3))                        \
((4, N5, 1, 2, N4))((5, N8, 2, 1, N4))((6, N7, 2, 0, N3, N8))((7, N9, 2, 2, N5))                \
((8, N11, 3, 1, N8))((9, N12, 3, 2, N9, N11))((10, N10, 2, 3, N9))((11, N13, 3, 3, N10))        \
((12, N6, 1, 3, N10))((13, N2, 0, 2, N6))((14, N14, 4, 0, N7))((15, N15, 4, 1, N12, N14))       \
((16, N16, 4, 3, N15))

#define FP2_NUM_NODES BOOST_PP_SEQ_SIZE(FP2_GRAPH_DATA)
#define FP2_MAYBE_COMMA(Index) BOOST_PP_COMMA_IF(BOOST_PP_LESS(BOOST_PP_ADD(Index, 1), FP2_NUM_NODES))

// When the graph is in its acyclic directional form, this is the node that has no parent.
#define FP2_ROOT_NODE N16


// Invokes NodeFn with NodeData
#define FP2_GRAPH_PRED(Rep, NodeFn, NodeData) NodeFn NodeData


// Defines the same graph structure according to some provided generator function.
// This allows different implementations to easily generate the same graph by only defining how to construct a node.
// NodeFn should be a macro of the form Fn(Name, X, Y, Connections...)
//
// Note: if there exists an edge between node A and B, either the invocation of NodeFn with A will have B in its
// connections, or the invocation of NodeFn with B will have A in its connections, but not both.
// If a node is mentioned in a connections list, NodeFn is guaranteed to have already been invoked for that node.
#define FP2_GRAPH_DEF(NodeFn)   \
BOOST_PP_SEQ_FOR_EACH(          \
    FP2_GRAPH_PRED,             \
    NodeFn,                     \
    FP2_GRAPH_DATA              \
)