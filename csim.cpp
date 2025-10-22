#include <iostream>

int csim( int argc, char **argv ) {

  return 0;
}
typedef struct direct_map_element {
    int tag;
    bool valid;
    int index;
} direct_map_element;

// determine if value is in cache
bool memory_hit(direct_map_element *cache, direct_map_element e){
  int idx = e.index;
  direct_map_element cache_e = *(cache+ idx);
  if (cache_e.valid == true && cache_e.tag == e.tag) {
    return true;
  }
  return false;
}

// if value not in cache, go to memory to retrieve
void load_from_memory() {

}