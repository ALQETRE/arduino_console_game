from pyperclip import copy
import math
from json import load

map1 = ["wwwwww",
        "w    w",
        "w    w",
        "wwwwww",
        ]

maps = {}
with open("pokemon_map_compression/maps.json", mode="r") as fd:
    maps = load(fd)

def compress(map, name, indent=""):
    map = ".".join(map)+","
    chars = "".join(set(map))
    char_size = len(chars).bit_length() # 32 - __builtin_clz(x);

    total = 0
    for char in map:
        idx = chars.index(char)
        total *= 2**char_size
        total += idx
    size = (total.bit_length()+7)//8
    output = total.to_bytes(size, "little")

    string_output = f"{indent}byte {name}TileMap[{size}] = " + "{"
    for byte in output:
        string_output += hex(byte)
        string_output += ", "
    string_output = string_output[:-2] + "};"
    string_output += f'\n{indent}string {name}TileSet = "{chars}";'

    copy(string_output)
    # print(string_output)
    return ((len(output) + len(chars)) * 8) / (len(map) * len(map[0]) * 8)

# print(compress(map1, "map1", "  "))

total = 0
max_val = 0
for name, map in maps.items():
    corr = compress(map, name, "  ")
    total += corr
    if max_val < corr:
        max_val = corr

print(f"Max: {max_val}")
print(f"Avg: {total/len(maps)}")