#include "../test_generator.h"

int main(int argc, char** argv) {
    if (argc < 2)
        return -1;
    Teakra::Test::GenerateTestCasesToFile(argv[1]);
}
