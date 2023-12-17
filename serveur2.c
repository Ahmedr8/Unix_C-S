#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <dirent.h>

#define MAX_BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Fonction pour envoyer la date et l'heure au client
void sendDateTime(int client_socket) {
    time_t t;
    struct tm *tm_info;

    time(&t);
    tm_info = localtime(&t);

    char datetime[MAX_BUFFER_SIZE];
    strftime(datetime, sizeof(datetime), "%Y-%m-%d %H:%M:%S", tm_info);

    if (write(client_socket, datetime, sizeof(datetime)) < 0) {
        error("Erreur lors de l'envoi de la date et de l'heure au client");
    }
}

// Fonction pour envoyer la liste des fichiers d'un répertoire au client
void sendFileList(int client_socket, const char *directory_path) {
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(directory_path)) != NULL) {
        // Envoi du nombre de fichiers
        int file_count = 0;
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                file_count++;
            }
        }
        closedir(dir);

        if (write(client_socket, file_count, sizeof(file_count), 0) < 0) {
            error("Erreur lors de l'envoi du nombre de fichiers au client");
        }

        // Envoi des noms de fichiers
        dir = opendir(directory_path);
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_type == DT_REG) {
                if (write(client_socket, ent->d_name, sizeof(ent->d_name), 0) < 0) {
                    error("Erreur lors de l'envoi du nom de fichier au client");
                }
            }
        }
        closedir(dir);
    } else {
        error("Erreur lors de l'ouverture du répertoire");
    }
}

// Fonction pour envoyer le contenu d'un fichier au client
void sendFileContent(int client_socket, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        error("Erreur lors de l'ouverture du fichier");
    }

    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, bytesRead, 0) < 0) {
            error("Erreur lors de l'envoi du contenu du fichier au client");
        }
    }

    fclose(file);
}

// Fonction pour envoyer la durée écoulée depuis le début de la connexion
void sendElapsedTime(int client_socket, time_t start_time) {
    time_t current_time;
    time(&current_time);

    double elapsed_time = difftime(current_time, start_time);

    if (send(client_socket, &elapsed_time, sizeof(elapsed_time), 0) < 0) {
        error("Erreur lors de l'envoi de la durée écoulée au client");
    }
}

int main(int argc, char *argv[]) {

    const int server_port = atoi(argv[1]);

    // Création de la socket
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
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

    // Mise en écoute de la socket
    if (listen(server_socket, 5) < 0) {
        error("Erreur lors de la mise en écoute de la socket");
    }

    printf("Serveur en attente sur le port %d...\n", server_port);

    while (1) {
        socklen_t client_addr_len = sizeof(client_addr);

        // Attente d'une connexion client
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            error("Erreur lors de l'acceptation de la connexion client");
        }

        printf("Nouvelle connexion acceptée.\n");

        // Code pour l'authentification (nom d'utilisateur + mot de passe) auprès du serveur
        // (à implémenter)

        time_t start_time;
        time(&start_time);

        while (1) {
        //envoi menu
      	     
            // Réception du choix du client
            int choice;
            if (recv(client_socket, &choice, sizeof(choice), 0) < 0) {
                error("Erreur lors de la réception du choix du client");
            }

            switch (choice) {
                case 1:
                    // Envoie de la date complète (jour / mois / année) et l'heure (heure: minutes: secondes) de l'horloge système du serveur
                    sendDateTime(client_socket);
                    break;
                case 2:
                    // Envoie de la liste des fichiers d'un répertoire
                    sendFileList(client_socket, "."); // Vous pouvez changer "." avec le chemin du répertoire souhaité
                    break;
                case 3:
                    // Envoie du contenu d'un fichier
                    // (à implémenter)
                    break;
                case 4:
                    // Envoie de la durée déjà écoulée depuis le début de la connexion du client courant
                    sendElapsedTime(client_socket, start_time);
                    break;
                default:
                    printf("Option invalide.\n");
            }
        }

        // Fermeture de la connexion client
        close(client_socket);
    }

    // Fermeture de la socket du serveur
    close(server_socket);

    return 0;
}
