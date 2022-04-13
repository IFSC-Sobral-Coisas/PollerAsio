//
// Created by msobral on 13/04/2022.
//

#include "Poller.h"
#include <algorithm>

using std::find;

Poller::Poller() {
}

Poller::~Poller() {
}


void Poller::adiciona(Callback & cb) {
    cb.init(io);
    cbs.push_back(&cb);
}

void Poller::remove(Callback & cb) {
    cb.disable();
    cb.disable_timeout();
    auto it = find(cbs.begin(), cbs.end(), &cb);
    if (it != cbs.end()) cbs.erase(it);

}

void Poller::limpa() {
    for (auto & cb: cbs) {
        remove(*cb);
    }
}

void Poller::despache() {
    for (auto & cb: cbs) cb->run();
    io.run();
}

void Poller::despache_simples() {
    for (auto & cb: cbs) cb->run();
    io.run_one();
}
