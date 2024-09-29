// dtls_server.c
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 4433

void handle_error(const char *file, int lineno, const char *msg) {
    fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

#define CHECK(x) if (!(x)) handle_error(__FILE__, __LINE__, #x)

int main() {
    SSL_CTX *ctx;
    SSL *ssl;
    int fd;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);

    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(DTLS_server_method());
    CHECK(ctx);

    CHECK(SSL_CTX_use_certificate_file(ctx, "server-cert.pem", SSL_FILETYPE_PEM));
    CHECK(SSL_CTX_use_PrivateKey_file(ctx, "server-key.pem", SSL_FILETYPE_PEM));

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(fd >= 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    CHECK(bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0);

    ssl = SSL_new(ctx);
    CHECK(ssl);
    BIO *bio = BIO_new_dgram(fd, BIO_NOCLOSE);
    CHECK(bio);
    SSL_set_bio(ssl, bio, bio);

    while (1) {
        printf("Waiting for connections...\n");
        CHECK(SSL_accept(ssl) == 1);
        printf("Client connected.\n");

        char buf[1024];
        int len = SSL_read(ssl, buf, sizeof(buf) - 1);
        CHECK(len >= 0);
        buf[len] = '\0';
        printf("Received: %s\n", buf);

        const char reply[] = "Hello, DTLS client!";
        CHECK(SSL_write(ssl, reply, sizeof(reply)) == sizeof(reply));

        SSL_shutdown(ssl);
    }

    close(fd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}
