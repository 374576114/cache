#ifndef __BENCHMAR_H__
#define __BENCHMAR_H__

#include "lib/lru.h"
#include "lib/arc.h"
#include "lib/lirs.h"
#include "lib/mq.h"
#include "trace/reader.h"

template <typename T>
class benchmark {
public:
    benchmark(const char *path, ll size, ll factor = 1)
        : list_(size), reader_(path, factor), all_(0){}
    ~benchmark() {}

    void Test(const char *str = "TEST" ) {
        char rw;
        ll key, value;

        while (reader_.Generator(rw, key, value)) {
            ++ all_;
            if (rw == 'R') {
                auto v = list_.Peek(key);
                if (IS_VALID(v)) {
                    list_.Get(key);
                } else {
                    list_.Add(key, value);
                }
            } else if (rw == 'W') {
                list_.Add(key, value);
            } else {
                // can not be there
                assert(0);
            }
        }
        Print(str);
    }

    void Print(const char *path) {
        std::cout << "\tSummay: " << path << std::endl
            << "all access: " << all_ << std::endl;
        list_.Print(true);
    }

private:
    T list_;
    Reader reader_;
    ll all_;
};

#endif //__BENCHMAR_H__
