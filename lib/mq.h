#ifndef __MQ_H__
#define __MQ_H__

#include "lru.h"

struct mq_node {
    ll key;
    ll value; 
    ll f;
    ll which; /* 0 - MQ_NUM-1 */
    std::list<mq_node*>::iterator it;
    long long exptime;

    mq_node(ll k, ll v, ll e):
        key(k), value(v), f(0), exptime(e) {}

    void Set(long long e) {exptime = e;}
};

typedef std::list<mq_node*> mq_list;

#define MQ_NUM ((ll)4)
#define MQ_LIFETIME ((ll)4)

class MQ {
public:
    MQ(ll size)
        : size_(size), qsize_(2 * size), used_(0), cur_(0) {}

    MQ(ll size, ll qsize)
        : size_(size), qsize_(qsize), used_(0), cur_(0) {}
    ~MQ() {
        for (auto it = map_.begin(); it != map_.end(); ++it) {
            delete it->second;
        }
    }

    void ToFifo(mq_node *p) {
        p->value = INVALID;
        p->which = MQ_NUM;

        if (qout_.size() >= qsize_) {
            auto pp = qout_.back();
            qout_.pop_back();
            map_.erase(pp->key);
            delete pp;
        }

        qout_.push_front(p);
        p->it = qout_.begin();
    }

    void Evict() {
        for (ll i = 0; i < MQ_NUM; ++i) {
            if (q_[i].empty()) {
                continue;
            } else {
                auto p = q_[i].back();
                q_[i].pop_back();
                -- used_;
                ToFifo(p);
                return;
            }
        }
    }

    void Adjust() {
        cur_ ++;
        for (ll i = 1; i < MQ_NUM; ++i) {
            if (q_[i].empty()) {
                continue;
            }

            auto p = q_[i].back();
            if (p->exptime <= cur_) {
                q_[i].pop_back();
                q_[i - 1].push_front(p);
                p->it = q_[i - 1].begin();
                p->exptime = cur_ + MQ_LIFETIME;
            }
        }
    }

    bool Add(ll key, ll value) {
        //Print();
        if (map_.find(key) != map_.end()) {
            Get(key, value);
            return true;
        }

        Adjust();

        long long exptime = cur_ + MQ_LIFETIME;
        auto p = new mq_node(key, value, exptime);
        assert(p);
        map_[key] = p;

        if (used_ >= size_) {
            Evict();
        }

        ll idx = QueueNum(p->f);
        q_[idx].push_front(p);
        p->which = idx;
        p->it = q_[idx].begin();

        ++ used_;
        return true;
    }

    ll Get(ll key, ll value = INVALID) {
        if (map_.find(key) == map_.end()) {
            return NONE;
        }

        Adjust();
        
        auto p = map_[key];
        ++ p->f;
        p->exptime = cur_ + MQ_LIFETIME;
        ll idx = QueueNum(p->f);

        if (p->which >= MQ_NUM) { // Qout
            if (value == INVALID) {
                return NONE;
            } else {
                p->value = value;
                qout_.erase(p->it);
                q_[idx].push_front(p);

                p->it = q_[idx].begin();
                p->which = idx;

                return value;
            }
        }

        q_[p->which].erase(p->it);
        q_[idx].push_front(p);

        p->it = q_[idx].begin();
        p->which = idx;

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

    void Print(bool flag = false) {
        if (! flag) {
            for (ll i = 0; i < MQ_NUM; ++i) {
                std::cout << i << " ";
                for (auto it = q_[i].begin(); it != q_[i].end(); ++it) {
                    auto p = *it;
                    std::cout << "[" << p->key << ":" << p->value << " " << p->f << "]";
                }
                std::cout << std::endl;
            }

            std::cout << "out ";
            for (auto it = qout_.begin(); it != qout_.end(); ++it) {
                auto p = *it;
                std::cout << "[" << p->key << ":" << p->value << " " << p->f << "]";
            }
            std::cout << std::endl;
        }

        sta_.Print();
    }

private:
    ll QueueNum(ll f) {
        ll sum = 1;
        for (ll i = 0; i < MQ_NUM; ++i) {
            if (sum >= f) {
                return i;
            } else {
                sum *= 2;
            }
        }

        return MQ_NUM - 1;
    }

    mq_list q_[MQ_NUM];
    mq_list qout_;
    std::unordered_map<int, mq_node*> map_;

    ll size_, used_, qsize_;
    ll cur_;

    statistic sta_;
};
#endif
