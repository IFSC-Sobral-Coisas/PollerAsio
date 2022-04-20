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

Callback::Callback(long tout) : base_tout(tout),tout(tout), enabled_to(true), enabled(false),
                                timeout_handler(this)
{
    if (tout < 0) throw -1;
}

Callback::~Callback() {
    disable();
    disable_timeout();
}

void Callback::init(boost::asio::io_context & io) {
    if (enabled_to) {
        timer.reset(new boost::asio::steady_timer(io, boost::asio::chrono::milliseconds(tout)));
    }
}

int Callback::timeout() const { return timer->expires_from_now().count();}

void Callback::reload_timeout() {
    tout = base_tout;
//    tout = base_tout;
}

void Callback::set_timeout(long timeout) {
    tout = timeout;
}

bool Callback::operator==(const Callback * o) const {
  return false;
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
    enabled = false;
}

void Callback::enable() {
    enabled = true;
}

void Callback::run() {
    if (enabled_to) {
        timer->expires_after(boost::asio::chrono::milliseconds(tout));
        timer->async_wait(timeout_handler);
    }
}

// adaptador para callback de um descritor genérico POSIX
CallbackStream::CallbackStream(boost::asio::posix::stream_descriptor::native_handle_type fd, long tout): Callback(tout), handler(this), fd(fd) {
    enabled = true;
}

CallbackStream::CallbackStream(long tout): Callback(tout),handler(this),fd(-1) {
}

void CallbackStream::init(boost::asio::io_context & io) {
    Callback::init(io);
    if (enabled) {
        descr.reset(new boost::asio::posix::stream_descriptor(io, fd));
    }
}

boost::asio::posix::stream_descriptor::native_handle_type CallbackStream::filedesc() const {
    if (descr.get()) {
        return descr->native_handle();
    } else return -1;
}

bool CallbackStream::operator==(const CallbackStream * o) const {
    return this == o;
}

void CallbackStream::run() {
    Callback::run();
    if (enabled) {
        if (descr.get()) {
            descr->cancel();
            descr->async_wait(boost::asio::posix::stream_descriptor::wait_read,
                              handler);
        }
    }
}

void CallbackStream::disable() {
    if (enabled) {
        if (descr.get()) {
            descr->cancel();
        }
        Callback::disable();
    }
}

CallbackStream::~CallbackStream() {
}

// adaptador para callback da porta serial
CallbackSerial::CallbackSerial(const string & path, long tout): Callback(tout),handler(this),dev_name(path) {
    enabled = true;
}

CallbackSerial::CallbackSerial(long tout): Callback(tout),handler(this) {
}

void CallbackSerial::disable() {
    if (enabled) {
        if (descr.get()) {
            descr->cancel();
        }
        Callback::disable();
    }
}

void CallbackSerial::init(boost::asio::io_context & io) {
    Callback::init(io);
    if (enabled) {
        descr.reset(new boost::asio::serial_port(io, dev_name));
    }
}

boost::asio::posix::stream_descriptor::native_handle_type CallbackSerial::filedesc() const {
    if (descr.get()) {
        return descr->native_handle();
    } else return -1;
}

void CallbackSerial::run() {
    Callback::run();
    if (enabled) {
        if (descr.get()) {
            descr->cancel();
            descr->async_read_some(boost::asio::buffer(data, BufSize),
                                   handler);
        }
    }
}

CallbackSerial::~CallbackSerial() {
}

bool CallbackSerial::operator==(const CallbackSerial * o) const {
    return this == o;
}
