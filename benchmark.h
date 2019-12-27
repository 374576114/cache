#ifndef __BENCHMAR_H__
#define __BENCHMAR_H__

#include "lib/lru.h"
#include "lib/arc.h"
#include "lib/lirs.h"
#include "lib/mq.h"
#include "trace/reader.h"
#include <chrono>

template <typename T>
class benchmark {
public:
    benchmark(const char *path, ll size, ll factor = 1)
        : list_(size), reader_(path, factor), all_(0), allr_(0), hitv_(0){
    }
    ~benchmark() {
        file_.close();
    }

    void Test(const char *str = "TEST", int interv = 1000) {
        char rw;
        ll key, value, allhit = 0, flag = 0;
        auto start = std::chrono::system_clock::now();
        file_.open(str);

        while (reader_.Generator(rw, key, value)) {
            ++ all_;
            if (rw == 'R') {
                ++ allr_;
                auto v = list_.Peek(key);
                if (IS_VALID(v)) {
                    ++ hitv_;
                    ++ allhit;
                    list_.Get(key);
                } else {
                    list_.Add(key, value);
                }

                if (allr_ % interv == 0) {
                    file_ << hitv_ << std::endl;
                    hitv_ = 0;
                }

            } else if (rw == 'W') {
                list_.Add(key, value);
            } else {
                // can not be there
                assert(0);
            }
        }
        std::cout << "interv: " << interv << " hit: " << hitv_ << " " << allhit << std::endl;

        auto end = std::chrono::system_clock::now();
        auto t = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        Print(str);
        std::cout << "Spend " << t.count()  << "ms" << std::endl;
    }

    void Print(const char *path) {
        std::cout << "\tSummay: " << path << std::endl
            << "all access: " << all_ << std::endl;
        list_.Print(true);
    }

private:
    T list_;
    Reader reader_;
    ll all_, allr_;
    ll hitv_;
    std::ofstream file_;
    std::ofstream log_;
};

#endif //__BENCHMAR_H__
