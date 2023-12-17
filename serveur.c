#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {


    const int server_port = atoi(argv[1]);

    // Création de la socket
    int server_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_socket < 0) {
        error("Erreur lors de la création de la socket");
    }

    struct sockaddr_in server_addr, client_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(server_port);

    // Liaison de la socket à l'adresse et au port spécifiés
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error("Erreur lors de la liaison de la socket");
    }

    printf("Serveur en attente sur le port %d...\n", server_port);

    int n;
    socklen_t client_addr_len = sizeof(client_addr);

    // Réception du nombre n du client
    if (recvfrom(server_socket, &n, sizeof(n), 0, (struct sockaddr*)&client_addr, &client_addr_len) < 0) {
        error("Erreur lors de la réception du nombre du client");
    }

    printf("Nombre reçu du client : %d\n", n);

    // Envoi de n nombres tirés au sort au client
    for (int i = 0; i < n; ++i) {
        int random_number = rand();
        if (sendto(server_socket, &random_number, sizeof(random_number), 0, (struct sockaddr*)&client_addr, client_addr_len) < 0) {
            error("Erreur lors de l'envoi du nombre tiré au sort au client");
        }
    }

    printf("%d nombres tirés au sort ont été envoyés au client.\n", n);

    // Fermeture de la socket
    close(server_socket);

    return 0;
}
