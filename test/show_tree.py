from checker_engine import *
eng = ckr_eng()
def load_tree():
    """Load the tree into json"""
    with open("tree.txt", "r") as f:
        import orjson as js
        record = js.loads(f.read())
        print(record['pos'], record['ply'])
        return ckr_pos_from_str(record['pos'], record['ply']), record['root']

from anytree import Node, RenderTree
def form_tree(tree, root=None):
    """Load the data into a tree"""
    node = Node(tree['move'], root)
    node.tree = tree
    if (tree['type'] in ['BLACK WIN', 'WHITE WIN', 'DRAW']):
        node.result = tree['type'] + '/%d' % tree['rollout_num']
    elif tree['rollout_num'] != 0:
        node.result = "%.2f%%/%d" % \
        (50*tree['win_num']/tree['rollout_num'], tree['rollout_num'])
    else:
        node.result = tree['type']
    for i in tree['children']:
        form_tree(i, node)
    return node

def repl(tree, pos):
    """A Read-eval-print loop"""
    # Record positions to retreat
    pos_list = [pos]
    while True:
        dir = input("Enter a move: ")
        if dir == '':
            print("Navigate to largest move ")
            max = -1
            for i in tree.children:
                if i.tree['rollout_num'] > max:
                    max = i.tree['rollout_num']
                    tree = i
            if max == -1:
                print("No child nodes!")
                continue
            pos = eng.make_move(pos, tree.name)
            pos_list.append(pos)
        elif dir == '..':
            print("Navigate to last move")
            tree = tree.parent
            del pos_list[len(pos_list) - 1]
            pos = pos_list[len(pos_list) - 1]
        else:
            print("Navigate to " + dir)
            found = False
            for i in tree.children:
                if i.name == dir:
                    found = True
                    tree = i
                    pos = eng.make_move(pos, tree.name)
                    pos_list.append(pos)
                    break
            if found == False:
                print(dir + " not found!")
                continue
        for pre, _, node in RenderTree(tree, maxlevel=3):
            print("%s%s (%s)" % (pre, node.name, node.result))
        print_ckr_pos(pos)


pos, root = load_tree()
print(type(pos))
print_ckr_pos(pos)
tree = form_tree(root)
for pre, _, node in RenderTree(tree, maxlevel=4):
    print("%s[%s] (%s)" % (pre, node.name, node.result))
repl(tree, pos)
