// dtls_client.c
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 4433
#define SERVER_IP "127.0.0.1"

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

    OpenSSL_add_ssl_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(DTLS_client_method());
    CHECK(ctx);

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    CHECK(fd >= 0);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr.sin_port = htons(PORT);

    ssl = SSL_new(ctx);
    CHECK(ssl);
    BIO *bio = BIO_new_dgram(fd, BIO_NOCLOSE);
    CHECK(bio);
    SSL_set_bio(ssl, bio, bio);

    CHECK(connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == 0);

    CHECK(SSL_connect(ssl) == 1);
    printf("Connected to server.\n");

    const char msg[] = "Hello, DTLS server!";
    CHECK(SSL_write(ssl, msg, sizeof(msg)) == sizeof(msg));

    char buf[1024];
    int len = SSL_read(ssl, buf, sizeof(buf) - 1);
    CHECK(len >= 0);
    buf[len] = '\0';
    printf("Received: %s\n", buf);

    SSL_shutdown(ssl);
    close(fd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}
