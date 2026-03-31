from pyperclip import copy

map1 = ["wwwwww",
        "w    w",
        "w    w",
        "wwwwww",
        ]

def compress(map, name, indent=""):
    chars = ""
    for line in map:
        chars = "".join(set(chars+line))
    string_output = f"{indent}uint64_t {name}TileMap[{len(map)}] = " + "{"
    for line in map:
        output = 0
        i = 0
        for char in line:
            output += chars.index(char) * (16 ** i)
            i+=1

        string_output += hex(output)
        string_output += ", "

    string_output = string_output[:-2] + "};"
    string_output += f'\n{indent}string {name}TileSet = "{chars}";'
    string_output += f'\n{indent}int {name}XSize = {len(map[0])};'
    string_output += f'\n{indent}int {name}YSize = {len(map)};'
    string_output += f'\n{indent}String {name}[{len(map)}];'
    string_output += f'\n{indent}decompress({name}TileMap, {name}TileSet, {name}XSize, {name}YSize, {name});'

    copy(string_output)
    return string_output

print(compress(map1, "map1", "  "))