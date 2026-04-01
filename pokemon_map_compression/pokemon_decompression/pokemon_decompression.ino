
void setup() {
  Serial.begin(115200);

// From here
  byte map1TileMap[12] = {0xa6, 0xb4, 0xa4, 0xa6, 0x78, 0x3a, 0x4a, 0x10, 0x59, 0xa4, 0xb6, 0xa2};
  String map1TileSet = "fe,d.gabchw?";

  byte map2TileMap[36] = {0x7a, 0x1, 0x73, 0x7, 0x54, 0x0, 0x10, 0x78, 0x40, 0x10, 0x44, 0x60, 0x5, 0x44, 0x1, 0x4, 0x45, 0x0, 0x54, 0x40, 0x10, 0x78, 0x40, 0x10, 0x7, 0x64, 0x0, 0x10, 0x7, 0x64, 0x0, 0x17, 0x30, 0x22, 0x75, 0x3};
  String map2TileSet = "w.0, cn?";
// To here is the code vs code gives you

  String map[69];

  decompress(map2TileMap, map2TileSet, map);

  for (int i = 0; i < 40; i++) {
    Serial.println(map[i]);
  }
}

void decompress(uint8_t* tileMap, String tileSet, String* out) {
  int baseCharSize = 16 - __builtin_clz(tileSet.length());
  int bitI = 0;
  bool lastRLE = false;
  bool lastRLECount = false;
  int rleCount = 0;
  String line = "";
  int lineIdx = 0;
  while (true) {
    int charSize = baseCharSize;
    if (lastRLE) {
      charSize = 4;
    }
    int byteI = bitI / 8;
    int bitOff = bitI % 8;

    uint16_t window = (tileMap[byteI] << 8) | (tileMap[byteI + 1]);
    uint8_t id = window >> (16 - charSize - bitOff) & ((1 << charSize) - 1);
    char tile = tileSet[id];
    bitI += charSize;

    // Serial.println(id);


    if (lastRLECount) {
      for (int i = 0; i < rleCount; i++){
        line += tile;
      }

      lastRLECount = false;
      continue;
    }
    if (lastRLE) {
      rleCount = id;

      lastRLE = false;
      lastRLECount = true;
      continue;
    }
    lastRLE = tile == '?';

    if (tile == '?') { continue; }
    if (tile == ',') { out[lineIdx] = line; break; }
    if (tile == '.') { out[lineIdx] = line; lineIdx++; line = ""; continue; }
    line += tile;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
