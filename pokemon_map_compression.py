from pyperclip import copy

map1 = ["wwwww",
        "w   w",
        "w   w",
        "wwwww",
        ]

def compress(map, name):
    map1 = ".".join(map)
    chars = "".join(set(map1))
    ids = "0123456789abcdefghijklmnopqrstuv"

    output = ""
    for char in map1:
        output += ids[chars.index(char)]

    output = output[::-1]
    output = int(output, len(chars))
    output = output.to_bytes((output.bit_length() + 7) // 8, "big")

    string_output = f"byte {name}TileMap[] = " + "{"
    for byte in output:
        string_output += hex(byte)
        string_output += ", "
    string_output = string_output[:-2] + "}"

    string_output += f'\nstring {name}TileSet = "{chars}"'

    copy(string_output)
    return string_output

print(compress(map1, "map1"))