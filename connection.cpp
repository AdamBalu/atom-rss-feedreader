#include "connection.h"
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <regex>
#include <fstream>
#include <iostream>

// most of the code is a modification of https://wiki.openssl.org/index.php/SSL/TLS_Client
void initOpensslLib() {
    (void) SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
}

void getPort(Link *link, const string& hN, bool secureConn) {
    if (regex_match(hN, regex(R"([^\/]*:\d+(\/.*)?)"))) {
        string prefixRem = regex_replace(hN, regex(".*:"), "");
        string port;
        for (size_t i = 0; i < prefixRem.length() && isdigit(prefixRem[i]); i++)
            if (isdigit(prefixRem[i]))
                port += prefixRem[i];
        link->port = port;
    }
    link->port = secureConn ? "443" : "80";
}

int setHostnamePortResource(Link *link, const string& url, bool *isSecure) {
    string fullLink;
    string hostnameWithResource;
    string unsecuredConn = "http://";
    string secureConn = "https://";
    *isSecure = false;

    if (!url.empty())
        fullLink = url;
    else
        return handleError(ERR_NO_LINK);

    // removing http / https prefix
    if (strstr(fullLink.c_str(), unsecuredConn.c_str()) != nullptr) {
        hostnameWithResource = regex_replace(fullLink, regex(unsecuredConn), "");
        getPort(link,hostnameWithResource, *isSecure);
    } else if (strstr(fullLink.c_str(), secureConn.c_str()) != nullptr) {
        *isSecure = true;
        hostnameWithResource = regex_replace(fullLink, regex(secureConn), "");
        getPort(link, hostnameWithResource, *isSecure);
    } else {
        return handleError(ERR_INVALID_LINK);
    }

    link->hostname = regex_replace(hostnameWithResource, regex("\\/.*"), "");
    link->resource = regex_replace(hostnameWithResource, regex(link->hostname), "");
    return 0;
}

int establishSecuredConnectionAndGetData(const Link& link, userData *userData) {
    long res;
    SSL_CTX *ctx;
    BIO *web, *out;
    FILE *rawData = fopen(RAW_DATA_FILENAME, "w+");
    if (rawData == nullptr) {
        handleError(ERR_OPEN_FILE);
        return 1;
    }

    const SSL_METHOD* method = SSLv23_method();
    if (method == nullptr) return handleError(ERR_CONNECTION);

    ctx = SSL_CTX_new(method);
    if (ctx == nullptr) return handleError(ERR_CONNECTION);

    // certificates verification setup
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL);

    SSL_CTX_set_verify_depth(ctx, 4);

    // actual verification
    if (userData->certAddress.empty() && userData->certFile.empty()) {
        SSL_CTX_set_default_verify_paths(ctx);
    } else {
        // function only takes NULL or correct certificates, we need to convert empty string in userData certs to NULL
        const char *CF = (!userData->certFile.empty())    ? userData->certFile.c_str()    : NULL;
        const char *CA = (!userData->certAddress.empty()) ? userData->certAddress.c_str() : NULL;
        res = SSL_CTX_load_verify_locations(ctx, CF, CA);
        if (1 != res) return handleError(ERR_CERTIFICATES);
    }

    web = BIO_new_ssl_connect(ctx);
    if (web == nullptr) return handleError(ERR_CONNECTION);

    string hostAndPort = link.hostname + ":" + link.port;
    res = BIO_set_conn_hostname(web, hostAndPort.c_str());

    out = BIO_new_fp(rawData, BIO_NOCLOSE);
    if (out == nullptr) return handleError(ERR_OUT);

    // connect to the provided url
    res = BIO_do_connect(web);
    if (res != 1)
        return handleError(ERR_CONNECTION);

    res = BIO_do_handshake(web);
    if (res != 1) return handleError(ERR_CONNECTION);

    string getRequest =
            "GET " + link.resource + " HTTP/1.0\r\n" +
            "Host: " + link.hostname + "\r\n" +
            "Connection: close\r\n\r\n";

    BIO_puts(web, getRequest.c_str());
    BIO_puts(out, "\n");

    // receiving response data
    int len;
    do {
        char buff[BUF_SIZE] = {};
        len = BIO_read(web, buff, sizeof(buff));
        if (len > 0) {
            BIO_write(out, buff, len);
        }
    } while (len > 0 || BIO_should_retry(web));

    BIO_free(out);
    BIO_free_all(web);
    SSL_CTX_free(ctx);
    fclose(rawData);
    return 0;
}

int establishUnsecuredConnectionAndGetData(const Link& link) {
    long res;
    BIO *web, *out;
    string hostAndPort = link.hostname + ":" + link.port;
    FILE *rawData = fopen(RAW_DATA_FILENAME, "w+");
    if (rawData == nullptr) {
        handleError(ERR_OPEN_FILE);
        return 1;
    }

    web = BIO_new_connect(hostAndPort.c_str());
    if (web == nullptr) return handleError(ERR_CONNECTION);

    // connect to the url
    res = BIO_do_connect(web);
    if (res != 1) return handleError(ERR_CONNECTION);

    // received data will be written inside rawData file
    out = BIO_new_fp(rawData, BIO_NOCLOSE);
    if(nullptr == out) return handleError(ERR_OUT);

    string getRequest =
            "GET " + link.resource + " HTTP/1.0\r\n" +
            "Host: " + link.hostname + "\r\n" +
            "Connection: close\r\n\r\n";

    BIO_puts(web, getRequest.c_str());
    BIO_puts(out, "\n");

    // receive response data
    int len;
    do {
        char buff[BUF_SIZE] = {};
        len = BIO_read(web, buff, sizeof(buff));
        if (len > 0) {
            BIO_write(out, buff, len);
        }
    } while (len > 0 || BIO_should_retry(web));

    BIO_free(out);
    BIO_free_all(web);
    fclose(rawData);
    return 0;
}

int checkServerResponse() {
    const char *responseOK11 = "HTTP/1.1 200 OK\r";
    const char *responseOK10 = "HTTP/1.0 200 OK\r";
    ifstream infile(RAW_DATA_FILENAME);
    string line;
    while (getline(infile, line)) {
        if (line.empty())
            continue;
        if (line == responseOK11 || line == responseOK10)
            return 0;
        return handleError(ERR_HTTP_RESPONSE);
    }
    return 0;
}

int establishConnectionAndGetData(userData *userData) {
    Link link;
    bool isSecure;
    int errStatus;

    errStatus = setHostnamePortResource(&link, userData->url, &isSecure);
    if (errStatus == 1)
        return errStatus;

    initOpensslLib();

    if (isSecure) {
        errStatus = establishSecuredConnectionAndGetData(link, userData);
    } else {
        errStatus = establishUnsecuredConnectionAndGetData(link);
    }
    if (errStatus == 1)
        return errStatus;

    errStatus = checkServerResponse();
    return errStatus;
}
