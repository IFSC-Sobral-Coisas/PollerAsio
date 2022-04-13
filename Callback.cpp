/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Callback.cpp
 * Author: msobral
 * 
 * Created on 20 de Setembro de 2018, 13:41
 */

#include "Callback.h"

Callback::Callback(int fd, long tout): fd(fd), base_tout(tout),tout(tout), enabled_to(true), enabled(fd >= 0) {
    if (tout < 0) throw -1;
}

Callback::Callback(long tout) : fd(-1), base_tout(tout),tout(tout), enabled_to(true), enabled(false) {
    if (tout < 0) throw -1;
}

Callback::~Callback() {
    disable();
    disable_timeout();
}

void Callback::init(boost::asio::io_context & io) {
    if (fd >= 0) {
        descr.reset(new boost::asio::posix::stream_descriptor(io, fd));
    }
    if (enabled_to) {
        timer.reset(new boost::asio::steady_timer(io, boost::asio::chrono::milliseconds(tout)));
    }
}

int Callback::filedesc() const { return fd;}

int Callback::timeout() const { return timer->expires_from_now().count();}

void Callback::reload_timeout() {
    tout = base_tout;
//    tout = base_tout;
}

void Callback::set_timeout(long timeout) {
    tout = timeout;
}

bool Callback::operator==(const Callback & o) const {
  return fd == o.fd;
}

void Callback::disable_timeout() {
    if (enabled_to) {
        enabled_to = false;
        if (timer.get()) {
            timer->cancel();
        }
    }
}

void Callback::enable_timeout() {
    enabled_to = true;
}

void Callback::disable() {
    if (enabled) {
        if (descr.get()) {
            descr->cancel();
        }
        enabled = false;
    }
}

void Callback::enable() {
    enabled = true;
}

void Callback::run() {
    if (enabled) {
        descr->cancel();
        descr->async_wait(boost::asio::posix::stream_descriptor::wait_read,
                          boost::bind(&Callback::prep_handle, this, boost::asio::placeholders::error));
    }
    if (enabled_to) {
        timer->expires_after(boost::asio::chrono::milliseconds(tout));
        timer->async_wait(boost::bind(&Callback::prep_handle_timeout, this, boost::asio::placeholders::error));
    }
}

void Callback::prep_handle(const boost::system::error_code& error) {
    if (! error) {
        this->handle();
        run();
    }
}

void Callback::prep_handle_timeout(const boost::system::error_code& error) {
    if (! error) {
        this->handle_timeout();
        reload_timeout();
        run();
    }
}