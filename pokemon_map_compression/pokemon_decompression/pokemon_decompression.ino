void setup() {
  Serial.begin(9600);

// From here
  uint64_t map1TileMap[4] = {0x0, 0x11110, 0x11110, 0x0};
  string map1TileSet = "w ";
  int map1XSize = 6;
  int map1YSize = 4;
  String map1[4];
  decompress(map1TileMap, map1TileSet, map1XSize, map1YSize, map1);
// To here is the code vs code gives you

  for (int i = 0; i < map1YSize; i++;){
    Serial.println(map1[i])
  }
}

void decompress(uint64_t* tileMap, String tileSet, int sizeX, int sizeY, String* out) {
  for (int y = 0; y < sizeY; y++) {
    String line = "";
    uint64_t lineByte = tileMap[y];
    for (int x = 0; x < sizeX; x++) {
      int blockId = (lineByte >> (x*4)) & 0b1111;
      char block = tileSet[blockId];
      line += block;
    }
    out[y] = line;
  }
}

void loop() {
  // put your main code here, to run repeatedly:

}
