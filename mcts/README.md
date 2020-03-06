Monte Carlo Tree Search facilities include
1. A tree to record games
2. A rollout policy

There are multiple types of nodes
1. Normal nodes with children (select)
2. Leaf nodes never visited (simulate)
3. Leaf nodes visited once (expand)
4. Leaf nodes that end the game, visited (retrive previous result)

For normal nodes, child_num is positive, while for the rest 3 types, -child_num serves as an enumerator.
mcts_is_leaf() tests if is not first type, mcts_leaf_type() gives type of leaf.
