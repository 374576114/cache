#ifndef __LRU_H__
#define __LRU_H__

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include "assert.h"

typedef unsigned long long ll;

#define NONE ((ll)-11)
#define INVALID (NONE)
#define IS_VALID(value) ((value) != NONE && (value) != INVALID)

struct node {
    ll key;
    ll value;
    ll f;

    node(ll a, ll b) : key(a), value(b), f(1) {}
    node(ll a, ll b, ll _f) : key(a), value(b), f(_f) {}
    void setf(ll _f) { f = _f; }
};

struct statistic {
    ll all;
    ll hit;

    statistic() : all(0), hit(0) {}

    void Hit(ll value) {
        ++ all;
        if ((value != NONE) && (value != INVALID)) {
            ++ hit;
        } else {
        }
    }

    void Print() {
        assert(all);
        std::cout << "All Read: " << all << " Hit:" << hit << " " << 1.0 * hit / all << std::endl;
    }
};

class LRU {
public:
    LRU(ll size) : size_(size) {}
    ~LRU() {}

    bool Add(ll key, ll value) {
        node cache(key, value);

        if (map_.find(key) != map_.end()) {
            auto it = map_[key];
            lru_.erase(it);
        }

        lru_.push_front(cache);
        map_[key] = lru_.begin();

        //Print();
        if (lru_.size() > size_) {
            auto key = lru_.back();
            return Del(key.key);
        }

        return false;
    }

    ll Peek(ll key) {
        ll value = NONE;

        if (map_.find(key) != map_.end()) {
            value = map_[key]->value;
        }

        sta_.Hit(value);
        return value;
    }

    ll Get(ll key) {
        if (map_.find(key) != map_.end()) {
            auto it = map_[key];
            node cache(map_[key]->key, map_[key]->value);

            lru_.erase(it);
            lru_.push_front(cache);
            map_[key] = lru_.begin();
            return cache.value;
        }

        return NONE;
    }

    bool Del(ll key) {
        if (map_.find(key) != map_.end()) {
            auto it = map_[key];

            lru_.erase(it);
            map_.erase(key);
            return true;
        }

        return false;
    }

    ll Len() {
        return lru_.size();
    }

    ll Popback() {
        ll key = lru_.back().key;
        Del(key);

        return key;
    }

    void Print(bool flag = false) {
        if (! flag) {
            for (auto it = lru_.begin(); it != lru_.end(); ++it) {
                std::cout << "[" << it->key << ":" << it->value << "] ";
            }
            std::cout << std::endl;
        }

        std::cout << "cnt: " << lru_.size() << "size: " << size_ << std::endl;
        sta_.Print();
    }
private:
    std::list<node> lru_;
    std::map<ll, std::list<node>::iterator> map_;
    ll size_;
    statistic sta_;
};

class LFU {
public:
    LFU(ll size) : size_(size) {}
    ~LFU() {}

    bool Insert(node &cache) {
        if (lfu_.empty() || cache.f <= lfu_.back().f) { // tail
            lfu_.push_back(cache);
            auto it = --lfu_.end();
            map_[cache.key] = it;
            return true;
        }

        if (cache.f > lfu_.front().f) { // head
            lfu_.push_front(cache);
            map_[cache.key] = lfu_.begin();
            return true;
        }

        for (auto it = lfu_.begin(); it != lfu_.end(); ++it) { // middle
            if (cache.f > it->f) {
                auto newit = lfu_.insert(it, cache);
                map_[cache.key] = newit;
                return true;
            }
        }

        return false;
    }

    bool Add(ll key, ll value) {
        bool flag = false;
        node cache(key, value);

        if (map_.find(key) != map_.end()) {
            auto it = map_[key];
            cache.f = it->f + 1;
            lfu_.erase(it);
        }

        if (lfu_.size() >= size_) {
            auto key = lfu_.back();
            flag = Del(key.key);
        }

        Insert(cache);

        //Print();
        return flag;
    }

    ll Peek(ll key) {
        ll value = NONE;

        if (map_.find(key) != map_.end()) {
            value = map_[key]->value;
        }

        sta_.Hit(value);

        return value;
    }

    ll Get(ll key) {
        if (map_.find(key) != map_.end()) {
            auto it = map_[key];
            node cache(it->key, it->value, it->f + 1);

            lfu_.erase(it);
            Insert(cache);

            return cache.value;
        }

        return NONE;
    }

    bool Del(ll key) {
        if (map_.find(key) != map_.end()) {
            auto it = map_[key];

            lfu_.erase(it);
            map_.erase(key);
            return true;
        }

        return false;
    }

    ll Len() {
        return lfu_.size();
    }

    ll Popback() {
        ll key = lfu_.back().key;
        Del(key);

        return key;
    }

    void Print(bool flag = false) {
        if (! flag) {
            for (auto it = lfu_.begin(); it != lfu_.end(); ++it) {
                std::cout << "[" << it->key << ":" << it->value << " " << it->f << "] ";
            }
            std::cout << std::endl;
        }

        sta_.Print();
    }

private:
    std::list<node> lfu_;
    std::map<ll, std::list<node>::iterator> map_;
    ll size_;
    statistic sta_;
};

#endif //__LRU_H__
