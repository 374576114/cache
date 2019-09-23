#include "reader.h"

int main()
{
    char r;
    ll k, v;
    Reader reader("./msr", 4096);

    while (reader.Generator(r, k, v)) {
        record tmp(r, k, v);
        tmp.Print();
    }

    return 0;
}
