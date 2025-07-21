import symtable
import ast

def get_map(source, tree):
    sym = symtable.symtable(source, "", "exec")

    globalz = []

    ft = {}

    vpo = set()

    vpi = set()

    for node in tree.body:
        if isinstance(node, ast.FunctionDef):
            return_types = []
            for statement in node.body:
                if type(statement) is ast.Return:
                    if type(statement.value) is ast.Tuple:
                        return_types.append(len(statement.value.elts))
                    elif statement.value is None:
                        return_types.append(0)
                    else:
                        return_types.append(1)

            if not all(x == return_types[0] for x in return_types):
                print("REturn statements must in function must agree")
                exit(-7)

            if len(return_types) == 0:
                ft[node.name] = 0
            else:
                ft[node.name] = return_types[0]
                if return_types[0] > 1:
                    print("Returning multiple values not supported")
                    exit(-8)
        elif isinstance(node, ast.Assign):
            if len(node.targets) != 1:
                print("DOes not support globals unpacking")
                exit(-30)

            if isinstance(node, ast.Name):
                print("Global assignment LHS must be a name")
                exit(-31)

            if isinstance(node.value, ast.Call):
                if isinstance(node.value.func, ast.Name):
                    print(node.value.func.id)
                    if node.value.func.id == "VPO":
                        vpo.add(node.targets[0].id)
                    elif node.value.func.id == "VPI":
                        vpi.add(node.targets[0].id)

            globalz.append(node.targets[0].id)

    functions = {}



    for child in sym.get_children():
        if child.get_type() != "function":
            print("Top level code must be function definiditon, found " + child.get_type() + " instead.")
            exit(-1)

        params = set(child.get_parameters())
        locals = set(child.get_locals()) ^ params


        functions[child.get_name()] = {
            "params": {x: len(child.get_parameters()) - i - 1 for i, x in enumerate(child.get_parameters())},
            "locals": {x: i for i, x in enumerate(locals)},
            "return": ft[child.get_name()],
        }

    return functions, {x: i for i, x in enumerate(globalz)}, vpo, vpi
