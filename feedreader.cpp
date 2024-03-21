#include <unistd.h>
#include <iostream>
#include <regex>
#include <fstream>
#include "feedreader.hpp"
#include "connection.hpp"
#include "proc_response.hpp"
#include "local_error.hpp"

void checkOtherParamsProvided(int argc, char **argv, userData *userData) {
    for (; optind < argc; optind++) {
        if (userData->url.empty()) {
            userData->url = argv[optind];
        } else {
            handleWarning(WARNING_TOO_MANY_PARAMS);
        }
    }
}

void argParser(int argc, char **argv, userData *userData) {
    int opt;
    while ((opt = getopt(argc, argv, ":f:c:C:hauT")) != -1) {
        if (opt == 'a') {
            userData->aFlag = true;
        } else if (opt == 'u') {
            userData->uFlag = true;
        } else if (opt == 'T') {
            userData->TFlag = true;
        } else if (opt == 'h') {
            userData->helpFlag = true;
            return;
        } else if (opt == 'f') {
            userData->feedFilePath = optarg;
        } else if (opt == 'c') {
            userData->certFile = optarg;
        } else if (opt == 'C') {
            userData->certAddress = optarg;
        } else if (opt == ':') {
            handleWarning(WARNING_MISSING_VALS);
        } else if (opt == '?') {
            handleWarningOption((char) optopt);
        }
    }
    checkOtherParamsProvided(argc, argv, userData);
}

int processFeedFile(userData *userData) {
    ifstream file;
    file.open(userData->feedFilePath.c_str());
    if (!file.is_open())
        return handleError(ERR_OPEN_FILE);

    string line;
    while (getline(file, line)) {
        if (regex_match(line, regex("\\s*#.*"))) // ignoring comments
            continue;
        if (line.empty()) // ignoring empty lines
            continue;
        // trim of leading and trailing whitespace
        line = regex_replace(line, regex("^\\s+"), "");
        line = regex_replace(line, regex("\\s+$"), "");
        userData->url = line;
        if (establishConnectionAndGetData(userData))
            continue;
        if (processXmlAndPrint(userData))
            continue;
    }
    file.close();
    return 0;
}

void printHelp() {
    cout << "Čtečka novinek ve formátu Atom a RSS s podporou TLS - Návod k použití" << endl;
    cout << "Spuštění: " << endl;
    cout <<  "./feedreader feedreader [-h] <URL | -f <feedfile>> [-c <certfile>] [-C <certaddr>] [-T] [-a] [-u]" << endl;
    cout << "       -h                 zobrazí pomocný manuál a ukončí program" << endl;
    cout << "       -f <feedfile>      definuje cestu k souboru s odkazama na stránky s RSS/Atom feedem" << endl;
    cout << "       -c <certfile>      Volitelný parametr -c definuje soubor <certfile> s certifikáty, který se použije" << endl;
    cout << "                          pro ověření platnosti certifikátu SSL/TLS předloženého serverem." << endl;
    cout << "       -C <certaddr>      Volitelný parametr -C určuje adresář <certaddr>, ve kterém  se mají vyhledávat " << endl;
    cout << "                          certifikáty, které se použijí pro ověření platnosti certifikátu SSL/TLS předloženého serverem." << endl;
    cout << "       -T                 pro každý záznam zobrazí informace o čase změny záznamu, či vytvoření záznamu (pozn.)" << endl;
    cout << "       -a                 pro každý záznam zobrazí jméno autora, či jeho e-mailovou adresu (pozn.)" << endl;
    cout << "       -u                 pro každý záznam zobrazí asociované URL (pozn.)" << endl;
    cout << endl;
    cout << "       poznámka: je-li ve staženém souboru obsaženo" << endl;
}

int main(int argc, char **argv) {
    userData userData;
    argParser(argc, argv, &userData);
    if (userData.helpFlag) {
        printHelp();
        return 0;
    }
    if (!userData.feedFilePath.empty()) {
        if (processFeedFile(&userData))
            return 0;
    } else { // no feedfile
        if (establishConnectionAndGetData(&userData))
            return 0;
        processXmlAndPrint(&userData);
    }
    return 0;
}
