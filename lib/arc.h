#ifndef __ARC_H__
#define __ARC_H__

#include "lru.h"
#include <algorithm>

class ARC {
public:
    ARC(ll size) : t1(size), b1(size), t2(size), b2(size), size_(size), p_(0) {}
    ~ARC() {}


    bool Add(ll key, ll value) {
        //Print();
        if (t1.Peek(key) != NONE) {
            t1.Del(key);
            t2.Add(key, value);
            return true;
        }

        if (t2.Peek(key) != NONE) {
            t2.Add(key, value);
            return true;
        }

        ll delta = 1;
        ll b1len = b1.Len();
        ll b2len = b2.Len();

        if (b1.Peek(key) != NONE) {
            if (b2len > b1len) {
                delta = b2len / b1len;
            }

            p_ = std::min(size_, p_ + delta);

            if (t1.Len() + t2.Len() >= size_) {
                Replace(false);
            }

            b1.Del(key);
            t2.Add(key, value);

            return true;
        } else if (b2.Peek(key) != NONE) {
            if (b1len > b2len) {
                delta = b1len / b2len;
            }

            p_ = std::max((ll)0, p_ - delta);

            if (t1.Len() + t2.Len() >= size_) {
                Replace(true);
            }

            b2.Del(key);
            t2.Add(key, value);
            return true;
        }

        // add t1
        if (t1.Len() + t2.Len() >= size_) {
            Replace(false);
        }

        if (b1.Len() > size_ - p_) {
            b1.Popback();
        }

        if (b2.Len() > p_) {
            b2.Popback();
        }

        t1.Add(key, value);
        return true;
    }

    ll Get(ll key) {
        ll value = t1.Peek(key);
        if (value != NONE) {
            t1.Del(key);
            t2.Add(key, value);
            return value;
        }

        value = t2.Get(key);
        if (value != NONE) {
            return value;
        }

        return NONE;
    }

    void Replace(bool flag) {
        ll t1len = t1.Len();
        if (t1len > 0 && (t1len > p_ || (t1len == p_ && flag))) {
            ll key = t1.Popback();
            b1.Add(key, INVALID);
        } else {
            ll key = t2.Popback();
            b2.Add(key, INVALID);
        }
    }

    void Del(ll key) {
        t1.Del(key);
        b1.Del(key);
        t2.Del(key);
        b2.Del(key);
    }

    ll Len() {
        return t1.Len() + t2.Len();
    }

    ll Peek(ll key) {
        ll value = t1.Peek(key);
        if (value != NONE) {
            sta_.Hit(value);
            return value;
        }

        value = t2.Peek(key);
        sta_.Hit(value);
        return value;
    }

    void Print(bool flag = false) {
        if (! flag) {
            std::cout << "t1:" << std::endl;
            t1.Print();

            std::cout << "b1:" << std::endl;
            b1.Print();

            std::cout << "t2:" << std::endl;
            t2.Print();

            std::cout << "b2:" << std::endl;
            b2.Print();
        }

        sta_.Print();
    }

private:
    LRU t1, b1, t2, b2;
    ll size_;
    ll p_;

    statistic sta_;
};

#endif // __ARC_H__
