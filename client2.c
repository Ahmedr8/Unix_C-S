#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {

    const char *server_address = argv[1];
    const int server_port = atoi(argv[2]);

    // Création de la socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        error("Erreur lors de la création de la socket");
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port);

    // Convertir l'adresse IP du serveur depuis le format ascii en format resaux
    if (inet_aton(server_address, &server_addr.sin_addr) == 0) {
        fprintf(stderr, "Adresse IP invalide : %s\n", server_address);
        exit(1);
    }

    // Connexion au serveur
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        error("Erreur lors de la connexion au serveur");
    }

    printf("Connecté au serveur %s:%d\n", server_address, server_port);

    // Code pour l'authentification (nom d'utilisateur + mot de passe) auprès du serveur
    // (à implémenter)

    char buffer[MAX_BUFFER_SIZE];
    ssize_t bytesRead;
    while (1) {
        // Affichage du menu
        printf("\nMenu:\n");
      for(int i=1;i<3;++){
    bytesRead = read(*, buffer, sizeof(buffer));
    if (bytesRead < 0) {
        error("Erreur lors de la lecture de la réponse du serveur");
    } else if (bytesRead == 0) {
        printf("La connexion a été fermée par le serveur.\n");
    } else {
        printf("%d : %s\n",i,buffer);
    }
    }

        // Demande à l'utilisateur de choisir une option
        printf("Choix : ");
        int choice;
        scanf("%d", &choice);

        // Envoi du choix au serveur
            if (write(client_socket, choice, sizeof(choice)) < 0) {
        error("Erreur lors de l'envoi du choi au serveur");
    }

        switch (choice) {
            case 1:
                // La visualisation de la liste des services offerts par le serveur
                // (à implémenter)
                break;
            case 2:
                // Choix du service et envoi de la demande du service souhaité
                // (à implémenter)
                break;
            case 3:
                // La visualisation des résultats renvoyés par le serveur
                // (à implémenter)
                break;
            case 4:
                // Quitter
                printf("Déconnexion.\n");
                close(client_socket);
                exit(0);
            default:
                printf("Option invalide.\n");
        }
    }

    return 0;
}
