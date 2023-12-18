#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#define NMAX 10  // Valeur maximale de n

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    const char *server_address = argv[1];
    const int server_port = atoi(argv[2]);

    // Création de la socket
    int client_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_socket < 0) {
        error("Erreur lors de la création de la socket");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    // Convertir l'adresse IP du serveur depuis le format texte en format reseux
    if (inet_aton(server_address, &server_addr.sin_addr) == 0) {
        fprintf(stderr, "Adresse IP invalide : %s\n", server_address);
        exit(1);
    }

    // Génération d'un nombre aléatoire entre 1 et NMAX
    int n = rand() % NMAX + 1;
    
     socklen_t server_addr_len = sizeof(server_addr);

    // Envoi du nombre au serveur
    if (sendto(client_socket, &n, sizeof(n), 0, (struct sockaddr*)&server_addr, server_addr_len) < 0) {
        error("Erreur lors de l'envoi du nombre au serveur");
    }

    printf("Nombre envoyé au serveur : %d\n", n);

    // Réception de la réponse du serveur
    int server_response;
    for(int i=1;i<=n;i++){
    	if (recvfrom(client_socket, &server_response, sizeof(server_response), 0, (struct 	sockaddr*)&server_addr, &server_addr_len) < 0) {
        error("Erreur lors de la réception de la réponse du serveur");
    }

    printf("Réponse du serveur : %d\n", server_response);
}
    // Fermeture de la socket
    close(client_socket);

    return 0;
}
