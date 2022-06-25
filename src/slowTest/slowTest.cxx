#include <cassert>

#include <slower.h>

int main(int argc, char* argv[]) {
  float slowVer = slowerVersion();
  SlowerConnection slower;

  int err = slowerSetup( slower, 2022 /* port */ );
  assert( err == 0 );
  
  return 0;
}
