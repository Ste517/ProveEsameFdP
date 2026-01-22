#ifndef COMPITO_H
#define COMPITO_H

#include <iostream>
using namespace std;

class ExchangeClient {
    const static int CLIENT_NUMBER = 5;
    const static int BUFFER_NUMBER = 5;
    const static int MESSAGE_SIZE = 19;

    struct Dato {
        char messaggio[MESSAGE_SIZE+1];
        int mittente;
        int destinatario;
        bool read_from[CLIENT_NUMBER];
    };

    static bool used_identifiers[CLIENT_NUMBER];
    static int get_first_available_id();
    static int last_written_buffer;
    static Dato* buffers[BUFFER_NUMBER];

    const int client_id;

    bool write_to_buffer(Dato* &buffer, int destinatario, const char* &messaggio);
    bool read_from_buffer(Dato *&buffer, int mittente, char *messaggio);

    static char caesar(char lower_bound, char upper_bound, char char_to_shift, int shift);
public:
    ExchangeClient();
    bool scrivi(const char *dato, int destinatario);
    friend ostream& operator<<(ostream& os, const ExchangeClient &exchange);
    bool leggi(char* dato, int mittente);
    operator int();
    ~ExchangeClient();
    static void cifra(char* dato, int shift_c, int shift_p);
};

#endif // COMPITO_H
