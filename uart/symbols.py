import symtable
import ast

def get_map(source, tree):
    sym = symtable.symtable(source, "", "exec")

    ft = {}

    for func in tree.body:
        return_types = []
        for statement in func.body:
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
            ft[func.name] = 0
        else:
            ft[func.name] = return_types[0]
            if return_types[0] > 1:
                print("Returning multiple values not supported")
                exit(-8)

    functions = {}



    for child in sym.get_children():
        if child.get_type() != "function":
            print("Top level code must be function definiditon, found " + child.get_type() + " instead.")
            exit(-1)

        params = set(child.get_parameters())
        locals = set(child.get_locals()) ^ params


        functions[child.get_name()] = {
            "params": {x: i for i, x in enumerate(params)},
            "locals": {x: i for i, x in enumerate(locals)},
            "return": ft[child.get_name()],
        }

    return functions
