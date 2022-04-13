//
// Created by msobral on 13/04/2022.
//

#ifndef POLLERASIO_POLLER_H
#define POLLERASIO_POLLER_H

#include <list>
#include <map>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include "Callback.h"

using std::list;
using std::map;

#define MAX_FDS 10

// Poller: um despachador de eventos
// Um objeto poller é capaz de monitorar um conjunto de descritores de arquivos
// e executar um callback para cada desccritor pronto para acesso
// Cada descritor pode especificar um timeout próprio
class Poller {
public:
    Poller();
    ~Poller();

    // adiciona um evento a ser vigiado, representado por um Callback
    void adiciona(Callback & cb);

    // remove callback associado ao descritor de arquivo fd
    void remove(Callback & cb);

    // remove todos callbacks
    void limpa();

    // vigia os descritores cadastrados e despacha os eventos (chama os callbacks)
    // para ser lido, ou até que expire o timeout (em milissegundos)
    void despache_simples();
    void despache();

protected:
    list<Callback*> cbs;
    boost::asio::io_context io;
};

#endif //POLLERASIO_POLLER_H
