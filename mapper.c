// network_sim_mapper.c
#define _GNU_SOURCE
#include <arpa/inet.h>   // inet_pton, inet_ntop, in_addr_t, ntohl
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// NOTA: usa buffer estático (não é thread-safe). Bom pra demo/MVP.
static const char *todotted_u32(in_addr_t ip_netorder) {
    static char buf[INET_ADDRSTRLEN]; // 16 bytes
    // Converte pra host order antes de extrair bytes via shift/mask
    uint32_t h = ntohl((uint32_t)ip_netorder);

    uint8_t a = (uint8_t)((h >> 24) & 0xff);
    uint8_t b = (uint8_t)((h >> 16) & 0xff);
    uint8_t c = (uint8_t)((h >>  8) & 0xff);
    uint8_t d = (uint8_t)((h >>  0) & 0xff);

    // Monta "a.b.c.d"
    // (buf tem 16 bytes, isso cabe)
    snprintf(buf, sizeof(buf), "%u.%u.%u.%u", a, b, c, d);
    return buf;
}

// Recebe string, valida e normaliza.
// Retorna NULL se inválido.
static const char *todotted_str(const char *s) {
    static char buf[INET_ADDRSTRLEN];
    struct in_addr addr;

    // inet_pton retorna 1 se ok, 0 se inválido, -1 se erro
    int rc = inet_pton(AF_INET, s, &addr);
    if (rc != 1) return NULL;

    // inet_ntop escreve forma canônica em buf
    if (!inet_ntop(AF_INET, &addr, buf, sizeof(buf))) return NULL;

    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <ipv4>\n", argv[0]);
        fprintf(stderr, "example: %s 10.0.0.1\n", argv[0]);
        return 2;
    }

    const char *s = argv[1];

    // 1) caminho “string -> valida -> normaliza”
    const char *canon = todotted_str(s);
    if (!canon) {
        fprintf(stderr, "invalid ipv4: '%s'\n", s);
        return 2;
    }
    printf("[todotted_str]  %s\n", canon);

    // 2) caminho “string -> inet_addr -> u32(network order) -> desmonta bytes”
    // inet_addr é legado e tem ambiguidade com 255.255.255.255, mas ok pra demo.
    in_addr_t ip = inet_addr(s);

    printf("[inet_addr]     0x%08x (network order bits)\n", (unsigned)ip);
    printf("[todotted_u32]  %s\n", todotted_u32(ip));

    return 0;
}
