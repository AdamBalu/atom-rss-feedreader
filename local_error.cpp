#include "local_error.hpp"
#include <iostream>

using namespace std;

int handleError(int err) {
    if (err == ERR_WRONG_ARGS) {
        cerr << "Chyba: Byly zachyceny špatné programové argumenty" << endl;
    } else if (err == ERR_ESTABLISHING_CONNECTION) {
        cerr << "Chyba: Nepodařilo se zahájit spojení na zadanou adresu" << endl;
    } else if (err == ERR_CONNECTION) {
        cerr << "Chyba: Nepodařilo se napojit na zadanou adresu" << endl;
    } else if (err == ERR_READ) {
        cerr << "Chyba: Čtení zdroje zlyhalo" << endl;
    } else if (err == ERR_CERTIFICATES) {
        cerr << "Chyba: Nepodařilo se ověřit platnost certifikátu" << endl;
    } else if (err == ERR_NO_LINK) {
        cerr << "Chyba: Nebylo zadáno žádné URL/feedfile" << endl;
    } else if (err == ERR_INVALID_LINK) {
        cerr << "Chyba: Bylo zadáno chybné URL" << endl;
    } else if (err == ERR_OUT) {
        cerr << "Chyba: Vnitřní chyba programu - nepodařilo se vytvořit soubor na zápis" << endl;
    } else if (err == ERR_OPEN_FILE) {
        cerr << "Chyba: Nepodařilo se otevřít zadaný soubor (soubor neexistuje nebo je zadaná cesta špatná)" << endl;
    } else if (err == ERR_FILE_FORMAT) {
        cerr << "Chyba: Byl detekován špatný formát feedfile" << endl;
    } else if (err == ERR_XML_FORMAT) {
        cerr << "Chyba: RSS/Atom feed má chybné XML formátování" << endl;
    } else if (err == ERR_HTTP_RESPONSE) {
        cerr << "Chyba: Zadaná stránka poskytla nevhodnou odpověď (očekává se HTTP/1.0 nebo HTTP/1.1 200 OK)" << endl;
    } else if (err == ERR_XML_NOT_DETECTED) {
        cerr << "Chyba: Stránka neobsahuje korektní formát XML" << endl;
    }
    return 1;
}

void handleWarning(int warning) {
    if (warning == WARNING_MISSING_VALS) {
        cerr << "Varování: Jedna z voleb v argumentech vyžaduje hodnotu, která nebyla zadána, takže byla tato volba"
                " přeskočena" << endl;
    } else if (warning == WARNING_TOO_MANY_PARAMS) {
        cerr << "Varování: Bylo zadáno příliš mnoho URL nebo jiných parametrů, program se může chovat nesprávně. "
                "Prosím skontrolujte programové argumenty." << endl;
    }
}

void handleWarningOption(char option) {
    cerr << "Varování: neznámá volba: \"-" << option << "\", přeskakuje se..." << endl;
}
