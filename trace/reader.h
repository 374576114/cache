#ifndef __READER_H__
#define __READER_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cctype>

#include "../lib/lru.h"

#define TMPKEY (12345)

struct record {
    char rw;
    ll key;
    ll len;

    record() : rw(0), key(0), len(0) {}
    record(ll r, ll k, ll _len) : rw(r), key(k), len(_len) {}

    bool Get(char &r, ll &k, ll &v) { // 获取一个
        if (len <= 0) {
            return false;
        }

        r = rw; k = key; v = TMPKEY;

        -- len; ++ key;

        return true;
    }

    void Print() { 
        std::cout << "[" << rw << " " << key << " " << len << "]" << std::endl;
    }
};

class Reader {
public:
    /* factor_ : The Block Cache Size */
    Reader(const char *file, ll factor = 1) : i_(0), factor_(factor) {
        file_.open(file);
        ReadTrace();
    }

    ~Reader() { file_.close(); }

    bool Generator(char &rw, ll &key, ll &value) {
        bool flag = false;

        while (i_ < v_.size()) {
            if (!v_[i_].Get(rw, key, value)) {
                ++ i_;
            } else {
                flag = true;
                break;
            }
        }

        return flag;
    }

private:
    void ReadTrace() {
        std::string str;
        record tmp;
        while (file_ >> str >> tmp.key >> tmp.len) {
            tmp.len = (tmp.len + factor_ - 1) / factor_;
            tmp.key = (tmp.key + factor_ - 1) / factor_;

            //tmp.Print();
            for (ll i = 0; i < str.size(); ++i) {
                if (std::toupper(str[i]) == 'R' || std::toupper(str[i]) == 'W') {
                    tmp.rw = std::toupper(str[i]);
                    break;
                }
            }

            v_.push_back(tmp);
        }
    }
    std::vector<record> v_;
    std::ifstream file_;
    ll i_;
    ll factor_; /* 1 Bytes 512 Bytes 4096 Bytes */
};

#endif // __READER_H__
