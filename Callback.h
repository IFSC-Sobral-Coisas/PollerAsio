/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Callback.h
 * Author: msobral
 *
 * Created on 20 de Setembro de 2018, 13:41
 */

#ifndef CALLBACK_H
#define CALLBACK_H

#include <memory>
#include <string>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

class Poller;

using std::unique_ptr;
using std::string;

// classe abstrata para os callbacks do poller
class Callback {
public:
    // cria um callback para um timer (equivalente ao construtor anterior com fd=-1)
    // out: timeout
    Callback(long tout);

    ~Callback();

    // ao especializar esta classe, devem-se implementar estes dois métodos !
    // handle: trata o evento representado neste callback
    // handle_timeout: trata o timeout associado a este callback
    virtual void handle() = 0;
    virtual void handle_timeout() = 0;

    // obtém o descritor monitorado
    virtual boost::asio::posix::stream_descriptor::native_handle_type filedesc() const {return -1;};

    // obtém o valor atual de timeout ( quantos ms faltam para o timer disparar)
    int timeout() const;

    // define o valor do próximo timeout ... irá valer somente uma vez !
    void set_timeout(long timeout);

    // redefine o timeout com o valor default (aquele informado no construtor)
    void reload_timeout();

    // desativa timeout
    void disable_timeout();

    // ativa timeout
    void enable_timeout();

    // desativa monitoramento do descritor
    virtual void disable();

    // ativa monitoramento do descritor
    void enable();

    // retorna true se monitoramento do descritor está ativado
    bool is_enabled() const { return enabled;}

    // retorna true se timeout está ativado
    bool timeout_enabled() const { return enabled_to;}

protected:
    class TimeoutHandler{
    public:
        TimeoutHandler(Callback * ptr):cb(ptr) {}
        void operator()(boost::system::error_code err) {
            if (!err) {
                cb->handle_timeout();
                cb->reload_timeout();
                cb->run();
            }
        }
    private:
        Callback * cb;
    };

    unique_ptr<boost::asio::steady_timer> timer;
    TimeoutHandler timeout_handler;

    long tout;
    long base_tout;// milissegundos. Se <= 0, este callback não tem timeout
    bool enabled_to;
    bool enabled;

    friend class Poller;

    virtual void init(boost::asio::io_context & io);
    virtual void run();
    // operator==: compara dois objetos callback
    // necessário para poder diferenciar callbacks ...
    virtual bool operator==(const Callback * o) const;
};

const int BufSize = 256;

class CallbackSerial: public Callback {
public:
    // cria um callback para um timer (equivalente ao construtor anterior com fd=-1)
    // out: timeout
    CallbackSerial(const string & path, long tout);
    CallbackSerial(long tout);

    ~CallbackSerial();

    // desativa monitoramento do descritor
    virtual void disable();
    virtual boost::asio::posix::stream_descriptor::native_handle_type filedesc() const;

protected:
    class Complete{
    public:
        Complete(CallbackSerial * ptr):cb(ptr) {}
        void operator()(boost::system::error_code err, std::size_t n) {
            if (!err) {
                cb->buf_len = n;
                cb->handle();
                cb->run();
            }
        }
    private:
        CallbackSerial * cb;
    };

    unique_ptr<boost::asio::serial_port> descr;
    char data[BufSize];
    int buf_len;
    string dev_name;
    Complete handler;

    virtual void init(boost::asio::io_context & io);
    virtual void run();

    // operator==: compara dois objetos callback
    // necessário para poder diferenciar callbacks ...
    virtual bool operator==(const CallbackSerial * o) const;
};

class CallbackStream: public Callback {
public:
    // fd: descritor de arquivo a ser monitorado. Se < 0, este callback é um timer
    // tout: timeout em milissegundos. Se < 0, este callback não tem timeout
    CallbackStream(boost::asio::posix::stream_descriptor::native_handle_type fd, long tout);

    // cria um callback para um timer (equivalente ao construtor anterior com fd=-1)
    // out: timeout
    CallbackStream(long tout);

    ~CallbackStream();

    virtual void disable();
    virtual boost::asio::posix::stream_descriptor::native_handle_type filedesc() const;

protected:
    class Handler{
    public:
        Handler(CallbackStream * ptr):cb(ptr) {}
        void operator()(boost::system::error_code err) {
            if (!err) {
                cb->handle();
                cb->run();
            }
        }
    private:
        CallbackStream * cb;
    };
    unique_ptr<boost::asio::posix::stream_descriptor> descr;
    Handler handler;
    boost::asio::posix::stream_descriptor::native_handle_type fd;

    virtual void init(boost::asio::io_context & io);
    virtual void run();
    // operator==: compara dois objetos callback
    // necessário para poder diferenciar callbacks ...
    virtual bool operator==(const CallbackStream * o) const;
};
#endif /* CALLBACK_H */

