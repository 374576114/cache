#include "lib/lru.h"
#include "lib/arc.h"
#include "lib/lirs.h"
#include "lib/mq.h"

#include "benchmark.h"

//const char* g_path = "trace/ngx";
const char* g_path = "trace/msr";
const ll    g_cache_size = 2 * 1024;
const ll    g_factor = 4096;

int main(int argc, char **argv)
{
    ll cache_size = g_cache_size;
    const char *path = g_path;

    if (argc > 2) {
        path = argv[1];
        cache_size = atoi(argv[2]);
    } else {
        std::cout << "[Usage: ./test log/path 1024\n\t[[Use default]]" << std::endl;
    }

    std::cout << "log: " << path << std::endl;
    std::cout << "cache_size: " << cache_size << std::endl;

    benchmark<LRU> lru(path, cache_size, g_factor);
    benchmark<LFU> lfu(path, cache_size, g_factor);
    benchmark<ARC> arc(path, cache_size, g_factor);
    benchmark<LIRS> lirs(path, cache_size, g_factor);
    benchmark<MQ>   mq(path, cache_size, g_factor);

    //lfu.Test("LFU");
    lru.Test("LRU");
    arc.Test("ARC");
    lirs.Test("LIRS");
    mq.Test("MQ");
}
