#ifdef CODEBLOCK
#include "src/StdAfx.h"
#include <string>
#include "src/include/SeriesData.h"

using namespace std;
string runcat(char* infile, char* outfile, char* format);
int main(int argc, char *argv[])
{
    char* input = (char *)"test_data/dist_101208-1.txt";
    char* output = (char *)"test_data/test_cb1.out";
    char* format = (char *)"[*|*]";
    string ret;
    ret = runcat(input, output, format);
    return 0;
}
#endif
