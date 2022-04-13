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
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

using std::unique_ptr;

// classe abstrata para os callbacks do poller
class Callback {
public:
    // fd: descritor de arquivo a ser monitorado. Se < 0, este callback é um timer
    // tout: timeout em milissegundos. Se < 0, este callback não tem timeout
    Callback(int fd, long tout);
    
    // cria um callback para um timer (equivalente ao construtor anterior com fd=-1)
    // out: timeout
    Callback(long tout);

    ~Callback();

    void init(boost::asio::io_context & io);

    // ao especializar esta classe, devem-se implementar estes dois métodos !
    // handle: trata o evento representado neste callback
    // handle_timeout: trata o timeout associado a este callback
    virtual void handle() = 0;
    virtual void handle_timeout() = 0;

    // operator==: compara dois objetos callback
    // necessário para poder diferenciar callbacks ...
    virtual bool operator==(const Callback & o) const;
    
    int filedesc() const;
    int timeout() const;
    void set_timeout(long timeout);
    void reload_timeout();
    void disable_timeout();
    void enable_timeout();
    void disable();
    void enable();
    bool is_enabled() const { return enabled;}
    bool timeout_enabled() const { return enabled_to;}
    void run();
    void prep_handle(const boost::system::error_code& erro);
    void prep_handle_timeout(const boost::system::error_code& erro);
protected:
    unique_ptr<boost::asio::posix::stream_descriptor> descr;
    unique_ptr<boost::asio::steady_timer> timer;

    int fd; // se < 0, este callback se torna um simples timer
    long tout; 
    long base_tout;// milissegundos. Se <= 0, este callback não tem timeout
    bool enabled_to;
    bool enabled;
};
#endif /* CALLBACK_H */

