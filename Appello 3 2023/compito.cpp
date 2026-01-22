#include "compito.h"

#include <cstring>
#include <iostream>

using namespace std;

bool ExchangeClient::used_identifiers[ExchangeClient::CLIENT_NUMBER] = {false};
int ExchangeClient::last_written_buffer = 0;
ExchangeClient::Dato* ExchangeClient::buffers[ExchangeClient::BUFFER_NUMBER] = {nullptr};

int ExchangeClient::get_first_available_id() {
    for (int i = 0; i < CLIENT_NUMBER; i++) {
        if (used_identifiers[i] == false) return i+1;
    }
    exit(1);
}

ExchangeClient::ExchangeClient() : client_id(get_first_available_id()) {
    used_identifiers[client_id-1] = true;
};

bool ExchangeClient::write_to_buffer(Dato *&buffer, int destinatario, const char* &messaggio) {
    if (buffer == nullptr) {
        buffer = new Dato;
        for (int i = 0; i < (get_first_available_id()-1); i++) buffer->read_from[i]=false;
        buffer->read_from[client_id-1] = true;
        for (int i = get_first_available_id()-1; i < CLIENT_NUMBER; i++) buffer->read_from[i]=true;
        buffer->mittente = client_id;
        buffer->destinatario = destinatario;
        strncpy(buffer->messaggio, messaggio, MESSAGE_SIZE);
        buffer->messaggio[MESSAGE_SIZE] = '\0';
        return true;
    }
    return false;
}

bool ExchangeClient::scrivi(const char* dato, int destinatario) {
    if (destinatario > CLIENT_NUMBER || destinatario < 0) return false;
    if (destinatario == client_id) return false;
    if (destinatario == 0) {
        return write_to_buffer(buffers[0], destinatario, dato);
    }
    int i = (last_written_buffer % (BUFFER_NUMBER - 1)) + 1;
    for (int count = 0; count < (BUFFER_NUMBER - 1); count++) {
        if (write_to_buffer(buffers[i], destinatario, dato)) {
            last_written_buffer = i;
            return true;
        }
        i = (i % (BUFFER_NUMBER - 1)) + 1;
    }
    return false;
}

ostream &operator<<(ostream &os, const ExchangeClient &exchange) {
    if (ExchangeClient::BUFFER_NUMBER > 0) {
        os << "B,";
        if (ExchangeClient::buffers[0]) {
            os << "mitt:" << ExchangeClient::buffers[0]->mittente << ",dato:" << "\"" << ExchangeClient::buffers[0]->messaggio << "\"\n";
        } else {
            os << "vuoto\n";
        }
    }
    for (int i = 1; i < ExchangeClient::BUFFER_NUMBER; i++) {
        os << i << ",";
        if (ExchangeClient::buffers[i]) {
            os << "mitt:" << ExchangeClient::buffers[i]->mittente << ",dest:" << ExchangeClient::buffers[i]->destinatario << ",dato:" << "\"" << ExchangeClient::buffers[i]->messaggio << "\"\n";
        } else {
            os << "vuoto\n";
        }
    }
    return os;
}

bool ExchangeClient::read_from_buffer(Dato *&buffer, int mittente, char *messaggio) {
    if (buffer != nullptr) {
        if (buffer->read_from[client_id-1]) return false;
        if (buffer->mittente != mittente) return false;
        if (buffer->destinatario == 0) {
            buffer->read_from[client_id-1] = true;
            strcpy(messaggio,buffer->messaggio);
            for (int i = 0; i < CLIENT_NUMBER; i++) {
                if (buffer->read_from[i] == false) return true;
            }
            delete buffer;
            buffer = nullptr;
            return true;
        }
        if (buffer->destinatario == client_id) {
            strcpy(messaggio,buffer->messaggio);
            delete buffer;
            buffer = nullptr;
            return true;
        }
        return false;
    }
    return false;
}

bool ExchangeClient::leggi(char *dato, int mittente) {
    for (int i = 0; i < BUFFER_NUMBER; i++) {
        if (read_from_buffer(buffers[i],mittente,dato)) return true;
    }
    return false;
}

ExchangeClient::operator int() {
    int return_value = 0;
    if (buffers[0] && !buffers[0]->read_from[client_id-1] && buffers[0]->mittente != client_id) return_value++;
    for (int i = 1; i < BUFFER_NUMBER; i++) {
        if (buffers[i] != nullptr && buffers[i]->destinatario == client_id) return_value++;
    }
    return return_value;
}

ExchangeClient::~ExchangeClient() {
    if (buffers[0]) buffers[0]->read_from[client_id-1] = true;
    for (int i = 0; i < BUFFER_NUMBER; i++) {
        if (buffers[i] && (buffers[i]->destinatario == client_id)) {
            delete buffers[i];
        }
    }
}

char ExchangeClient::caesar(char lower_bound, char upper_bound, char char_to_shift, int shift) {
    int delta = int(upper_bound) - int(lower_bound) + 1;
    int shift_delta = (int(char_to_shift) - int(lower_bound) + shift) % delta;
    char shifted_char;
    if (shift_delta < 0) {
        shifted_char = char(shift_delta+1+upper_bound);
    } else {
        shifted_char = char(shift_delta+lower_bound);
    }
    return shifted_char;
}

void ExchangeClient::cifra(char *dato, int shift_c, int shift_p) {
    int string_length = strlen(dato);
    if (string_length == 0) return;

    for (int i = 0; i < string_length; i++) {
        if (dato[i] <= 'z' && dato[i] >= 'a') {
            dato[i] = caesar('a', 'z', dato[i], shift_c);
        } else if (dato[i] <= 'Z' && dato[i] >= 'A') {
            dato[i] = caesar('A', 'Z', dato[i], shift_c);
        } else if (dato[i] <= '9' && dato[i] >= '0') {
            dato[i] = caesar('0', '9', dato[i], shift_c);
        }
    }

    char temp[MESSAGE_SIZE + 1];
    for (int i = 0; i < string_length; i++) {
        int nuova_pos = (i - (shift_p % string_length) + string_length) % string_length;
        temp[nuova_pos] = dato[i];
    }
    temp[string_length] = '\0';
    strcpy(dato, temp);
}
