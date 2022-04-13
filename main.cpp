#include <iostream>
#include <string>
#include "Poller.h"

using namespace std;

class CallbackStdin: public Callback {
public:
    CallbackStdin(long tout): Callback(0, tout) {}

    void handle() {
        string w;

        getline(cin, w);
        cout << "Lido: " << w << endl;
    }

    void handle_timeout() {
        cout << "Timeout !!!" << endl;
        disable();
        disable_timeout();
    }

};

class Timer: public Callback {
public:
    Timer(long tout): Callback(tout) {
        t0 = time(NULL);
    }

    void handle() {
    }

    void handle_timeout() {
        time_t t = time(NULL) - t0;
        cout << "TIMER: " << t << endl;
        if (t > 10) disable_timeout();
    }
private:
    time_t t0;
};

int main() {
    // cria o objeto CallbackStdin, com timeout de 5000ms
    CallbackStdin cb(5000);
    Timer t(3000);

    //cria um poller: ele contém um loop de eventos !
    Poller sched;

    // registra o callback no poller
    sched.adiciona(cb);
    sched.adiciona(t);

    // entrega o controle ao poller.
    // Ele irá esperar pelos eventos e executará
    // os respectivos callbacks
    sched.despache();

    cout << "Fui !" << endl;
}