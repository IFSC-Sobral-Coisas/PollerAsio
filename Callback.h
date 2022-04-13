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

class Poller;

using std::unique_ptr;

// classe abstrata para os callbacks do poller
class Callback {
public:
    // fd: descritor de arquivo a ser monitorado. Se < 0, este callback é um timer
    // tout: timeout em milissegundos. Se < 0, este callback não tem timeout
    Callback(boost::asio::posix::stream_descriptor::native_handle_type fd, long tout);
    
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
    boost::asio::posix::stream_descriptor::native_handle_type filedesc() const;

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
    void disable();

    // ativa monitoramento do descritor
    void enable();

    // retorna true se monitoramento do descritor está ativado
    bool is_enabled() const { return enabled;}

    // retorna true se timeout está ativado
    bool timeout_enabled() const { return enabled_to;}
protected:
    unique_ptr<boost::asio::posix::stream_descriptor> descr;
    unique_ptr<boost::asio::steady_timer> timer;

    boost::asio::posix::stream_descriptor::native_handle_type fd; // se < 0, este callback se torna um simples timer
    long tout; 
    long base_tout;// milissegundos. Se <= 0, este callback não tem timeout
    bool enabled_to;
    bool enabled;

private:
    friend class Poller;

    void init(boost::asio::io_context & io);
    void run();
    void prep_handle(const boost::system::error_code& erro);
    void prep_handle_timeout(const boost::system::error_code& erro);
    // operator==: compara dois objetos callback
    // necessário para poder diferenciar callbacks ...
    virtual bool operator==(const Callback & o) const;
};
#endif /* CALLBACK_H */

