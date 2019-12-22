import json

exit_codes = ['q', 'Q']

def main():
    print_help()

    # Ask for filename
    filename = get_filename() 

    # Get comment
    comment = input("Comment: ")

    # Add edges
    print("Add edges!")
    edges = list()
    while True:
        edge = add_edge()
        if edge is None:
            break
        edges.append(edge)

    # Get nodes
    l_nodes = {e.id1 for e in edges}
    r_nodes = {e.id2 for e in edges}
    nodes = list(l_nodes | r_nodes)

    # Get bundles    
    print("Add bundles!")
    bundles = list()
    while True:
        bundle = add_bundle()
        if bundle is None:
            break
        bundles.append(bundle)

    graph = {
        'comment': comment,
        'graph': {
            'edges': [
                edge.dump() for edge in edges
            ],
            'nodes': [
                {
                    'id': node_id
                } for node_id in nodes
            ]
        },
        'bundles': [
            bundle.dump() for bundle in bundles
        ]
    }

    with open(filename, 'w') as out:
        json.dump(graph, out, indent = 4)

    print(f"Outputted to {filename}")

def print_help():
    instructions = """Graph Assistant Instructions
    1. Edge declarations phase
       - Prompts for the first and second nodes in the edge.
         Ex.
            Node 1: <Node1 str>
            Node 2: <Node2 str>
         Node string format: \d+[lr]?
         l and r denotes the left and right side of a node respectively.
         When done, use 'q' to exit the edge declaration phase.
    2. Bundle declaration phase
       - Prompts for a bundle.
         Ex.
            Finish bundles? <Finished str>
         Type 'qq' to indicate there are no more bundles and exit tool.
         Type <enter> to declare a new bundle.
       - Prompts for the left side of a bundle.
         Ex.
            Left side node: <Node1 str>
            > <Node2 str>
         Node string format: \d+r?
         r is used to indicate that the node will be reversed.
         Type 'q' when all nodes on the left side have been declared.
       - Prompts for the right side of a bundle.
         The steps are the same as the left side.
    3. Custom json format will be outputted to <difficulty><id>_<name>.json
    """
    print(instructions)

def _keep_prompting(prompt, condition):
    while True:
        input_str = input(prompt)
        if condition(input_str):
            return 

def get_filename():
    """Returns a formatted filename."""
    difficulty = input("Test graph difficulty: ")
    count = input("Test graph count: ")
    name = input("Test graph name: ")

    return f"{difficulty}{count}_{name.lower().replace(' ', '_')}.json" 
    
def add_edge():
    """Returns a parsed edge or None
    
    If the exit code is entered, returns None.

    Returns:
        An Edge object
    """

    edge = Edge()

    def parse_input(node_str, is_first = True):
        """Returns a tuple of (node_id, is_reverse)."""
        return int(node_str[:-1]), node_str[-1] == ('l' if is_first else 'r')
    
    def is_exit(input_str):
        return input_str in exit_codes

    def get_node(prompt, is_first = True):
        """Processes node or exits.

        Returns:
            A boolean indicating if an exit command was given.
        """

        while True:
            input_str = input(prompt)
            if is_exit(input_str):
                return True
            if input_str[-1] not in "lr":
                input_str += 'r' if is_first else 'l'
            try:
                if is_first:
                    edge.id1, edge.from_left = parse_input(input_str) 
                else:
                    edge.id2, edge.to_right = parse_input(input_str, False)
                return False
            except:
                continue

    if get_node("Node 1: ", is_first = True):
        return None
    if get_node("Node 2: ", is_first = False):
        return None
    
    return edge

def add_bundle():
    if input("Finish bundles? ") == 'qq':
        return None

    def parse_input(node_str):
        """Returns a tuple of (node_id, is_reverse)."""
        return int(node_str[:-1]), node_str[-1] == 'r'

    def is_exit(input_str):
        return input_str in exit_codes

    def get_node(prompt):
        while True:
            input_str = input(prompt)
            if is_exit(input_str):
                return None
            if input_str[-1] not in "fr":
                input_str += 'f'
            try:
                return parse_input(input_str)
            except:
                continue

    # Get left side
    bundle = Bundle()

    while True:
        l_node = get_node("Left side node: " if not len(bundle.left_nodes) else "> ")
        if l_node is None:
            break
        bundle.left_nodes.append(l_node)
    
    while True:
        r_node = get_node("Right side node: " if not len(bundle.right_nodes) else "> ")
        if r_node is None:
            break
        bundle.right_nodes.append(r_node)

    return bundle

class Edge(object):
    def __init__(self):
        self.id1 = -1;
        self.id2 = -1;
        self.from_left = False;
        self.to_right = False;
    
    def dump(self):
        """Returns edge as a compliant edge object."""
        return {
            'id1': self.id1,
            'id2': self.id2,
            'from_left': self.from_left,
            'to_right': self.to_right
        }

class Bundle(object):
    def __init__(self):
        self.left_nodes = list()
        self.right_nodes = list()
    
    def dump(self):
        """Returns bundle as a compliant bundle object."""
        return {
            'left': [
                {'id': node_id, 'is_reverse': is_reverse} for (node_id, is_reverse) in self.left_nodes
            ],
            'right': [
                {'id': node_id, 'is_reverse': is_reverse} for (node_id, is_reverse) in self.right_nodes
            ]
        }

if __name__ == '__main__':
    main()