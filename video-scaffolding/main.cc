#include "video-generator.h"

int main() {
  VideoGeneratorInterface::MakeInstance()->GenerateAndPrint();
  return 0;
}
