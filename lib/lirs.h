#ifndef __LIRS_H__
#define __LIRS_H__

#include <list>
#include <iostream>
#include <unordered_map>
#include <assert.h>
#include "lru.h"
#include <stdio.h>

#define NEED_PRUNING(n) ((n)->type != LIR)
struct lirs_node;
typedef std::list<lirs_node*>::iterator lirs_iterator;

enum lirs_type {
    LIR = 101,
    HIR,
    NHIR,
};

struct lirs_node {
    ll key;
    ll value;

    lirs_type type;
    lirs_iterator s;
    lirs_iterator q;

    lirs_node(ll _key, ll _value, lirs_iterator ends, lirs_iterator endq)
        : key(_key), value(_value), s(ends), q(endq), type(LIR) {}
    lirs_node(ll _key, ll _value, lirs_iterator ends, lirs_iterator endq, lirs_type _type)
        : key(_key), value(_value), s(ends), q(endq), type(_type) {}

    void Set(lirs_type _type) { type = _type;}
};

class LIRS {
public:
    LIRS(ll size)
        : cache_size_(size), used_size_(0), s_size_(0.99 * size), q_size_(0.1 * size) {
            assert(s_size_);
            assert(q_size_);
        }

    LIRS(ll size, ll s, ll q) 
        : cache_size_(size), used_size_(0), s_size_(s), q_size_(q) {}

    ~LIRS() {
        for (auto it = map_.begin(); it != map_.end(); ++it) {
            //std::cout << "key: " << it->second->key << std::endl;
            delete (it->second);
        }
    }

    void FreeOne() {
        assert(!q_.empty());

        auto pnode = q_.back();
        q_.pop_back();
        pnode->q = q_.end();

        if (IS_VALID(pnode->value)) {
            pnode->value = INVALID;
            -- used_size_;
        }

        if (pnode->s != s_.end()) {
            pnode->type = NHIR;
        } else {
            //std::cout << "Free Pnode" << std::endl;
            map_.erase(pnode->key);
            delete pnode;
        }
    }

    bool Add(ll key, ll value) {
        //Print();
        if (map_.find(key) != map_.end()) { // find it
            auto pnode = map_[key];
            if (!IS_VALID(pnode->value)) {
                ++ used_size_;
            }
            Get(key, value);

            return true;
        }

        if (used_size_ >= cache_size_ || q_.size() >= q_size_) { // 清理
            FreeOne();
        }

        // S is not FULL, so just input it as LIR
        lirs_node *p = new lirs_node(key, value, s_.end(), q_.end());
        assert(p);
        Push(p, true);
        ++ used_size_;

        // S is FULL, so just input it as HIR
        if (used_size_ > s_size_) {
        //if (s_.size() > s_size_) {
            p->type = HIR;
            Push(p, false);
        }

        return true;
    }

    ll Get(ll key, ll value = INVALID) {
        if (map_.find(key) == map_.end()) {
            return NONE;
        }
        
        auto p = map_[key];

        if (p->type == LIR) {
            assert(p->s != s_.end());
            MoveTop(p);
        } else if (p->type == HIR) {
            assert(p->q != q_.end());
            if (p->s != s_.end()) {
                p->type = LIR;

                MoveTop(p);
                Pop(p, false);
                Bottom();
            } else {
                Push(p, true);
                MoveTop(p, false);
            }
        } else {
            assert(p->type == NHIR);
            FreeOne();
            p->value = value;

            if (p->s != s_.end()) {
                p->type = LIR;
                MoveTop(p);
                Bottom();
            } else {
                assert(p->q == q_.end());
                p->type = HIR;
                Push(p, true);
                Push(p, false);
            }
        }

        Pruning();

        return p->value;
    }

    ll Peek(ll key) {
        ll value = NONE;
        if (map_.find(key) != map_.end()) {
            value = map_[key]->value;
        }

        sta_.Hit(value);
        return value;
    }

    void Pruning() {
        while (!s_.empty() && NEED_PRUNING(s_.back())) {
            s_.back()->s = s_.end();
            s_.pop_back();
        }
    }

    void Print(bool flag = false) {
        if (! flag) {
            for (auto pit = s_.begin(); pit != s_.end(); ++pit) {
                auto it = *pit;
                std::cout << "[" << it->key << ":" << it->value << " " << it->type << "]";
            }
            std::cout << std::endl;

            for (auto pit = q_.begin(); pit != q_.end(); ++pit) {
                auto it = *pit;
                std::cout << "[" << it->key << ":" << it->value << " " << it->type << "]";
            }
            std::cout << std::endl;
        }

        sta_.Print();
        std::cout << "{" << s_.size() << ":" << q_.size() << "}"
                  << "{" << used_size_ << ":" << s_size_ << ":" << q_size_  << "}" << std::endl;
    }
          
private:
    void Bottom() {
        auto bottom = s_.back();
        if (bottom->type == LIR) {
            bottom->type = HIR;
            if (bottom->q != q_.end()) {
                Pop(bottom, false);
            }
            Push(bottom, false);
        }
    }
    // true to S, false to Q
    void Push(lirs_node *p, bool toS) {
        if (toS) {
            s_.push_front(p);
            p->s = s_.begin();
        } else {
            q_.push_front(p);
            p->q = q_.begin();
        }

        if (map_.find(p->key) == map_.end()) {
            map_[p->key] = p;
        }
    }

    void Pop(lirs_node *p, bool fromS) {
        if (fromS) {
            assert(p->s != s_.end());
            s_.erase(p->s);
            p->s = s_.end();
        } else {
            assert(p->q != q_.end());
            q_.erase(p->q);
            p->q = q_.end();
        }
    }

    void MoveTop(lirs_node *p, bool toS = true) {
        Pop(p, toS);
        Push(p, toS);
    }

    // front -- top  back  -- bottom
    std::list<lirs_node*> s_, q_;
    std::map<ll, lirs_node*> map_;
    
    ll cache_size_, used_size_;
    ll s_size_;
    ll q_size_;

    statistic sta_;
};

#endif // __LIRS_H__
