from pyperclip import copy
import math
from json import load

map1 = ["wawwww",
        "wabcdw",
        "wefghw",
        "wwwwww",
        ]

maps = {}
with open("pokemon_map_compression/maps.json", mode="r") as fd:
    maps = load(fd)

def compress(map, name, indent=""):
    pre_comp_size = len(map) * len(map[0]) * 8

    map = ".".join(map)+","

    chars = "".join(set(map))
    char_size = (len(chars)+1).bit_length() # 32 - __builtin_clz(x);

    prev_char = map[0]
    map_separated = []
    part = ""
    for char in map:
        if len(part) >= 15:
            prev_char = char
            map_separated.append(part)
            part = char
        elif char == prev_char:
            part += char
        else:
            prev_char = char
            map_separated.append(part)
            part = char
    map_separated.append(part)

    for i in range(len(map_separated)):
        part = map_separated[i]
        if len(part)*char_size < 4 + (char_size*2):
            continue

        map_separated[i] = f"?{format(len(part), 'x')}{part[0]}"

    map = "".join(map_separated)

    if "?" in map:
        chars += "?"

    char_size = (len(chars)).bit_length()

    total = 1
    next_is_num = False
    for char in map:
        if not next_is_num:
            idx = chars.index(char)
            total = total << char_size
            total += idx
        else:
            total *= 16
            total += int(char, 16)
        next_is_num = char == "?"
    
    size = (total.bit_length()+6)//8
    total = total << ((size*8) - total.bit_length()+1)
    total -= 2**(total.bit_length()-1)
    output = total.to_bytes(size, "big")

    # print(map)
    # print(format(total, "b"))
    # print()

    string_output = f"{indent}byte {name}TileMap[{size}] = " + "{"
    for byte in output:
        string_output += hex(byte)
        string_output += ", "
    string_output = string_output[:-2] + "};"
    string_output += f'\n{indent}String {name}TileSet = "{chars}";'

    copy(string_output)
    print(string_output)
    return ((len(output) + len(chars)) * 8) / (pre_comp_size)

# print(compress(map1, "map1", "  "))
print(compress(maps["levelData5"], "map2", "  "))

# total = 0
# max_val = 0
# for name, map in maps.items():
#     corr = compress(map, name, "  ")
#     total += corr
#     if max_val < corr:
#         max_val = corr

# print(f"Max: {max_val}")
# print(f"Avg: {total/len(maps)}")