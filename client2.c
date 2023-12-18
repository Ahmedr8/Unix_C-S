#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <termios.h>
#define MAX_BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}
void enableEcho(){
    struct termios term;
    tcgetattr(STDIN_FILENO,&term);
    term.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO,TCSANOW,&term);
}
void disableEcho(){
    struct termios oldTerm,newTerm;
    tcgetattr(STDERR_FILENO,&oldTerm);
    newTerm = oldTerm;
    newTerm.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO,TCSANOW,&newTerm);
}
int main(int argc, char *argv[]) {
    int choice;
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
    char username[100];
    char password[100];
    int loggedIn = 0;
    while(!loggedIn)
    {   printf("username:");
        fflush(stdout);
        scanf("%s",username);
        printf("mot de passe pour %s:",username);
        fflush(stdout);
        disableEcho();
        scanf("%s",password);
        enableEcho();
        write(client_socket,username,sizeof(username));
        write(client_socket,password,sizeof(password));
        ssize_t bytesRead = read(client_socket,&loggedIn,sizeof(loggedIn));
        printf("\n");
        if (bytesRead > 0) {
          if(loggedIn == 0){
            printf("Nom d'utilisateur ou mot de passe incorrect !\n");
           }
        }
        else if (bytesRead == 0) {
            printf("La connexion a été fermée par le serveur.\n");
        }     
        else {
            error("Erreur lors de la lecture de la réponse du serveur");
        }
    }
    printf("\nConnecté au serveur %s:%d\n", server_address, server_port);

    // Code pour l'authentification (nom d'utilisateur + mot de passe) auprès du serveur
    // (à implémenter)
    char buffer[MAX_BUFFER_SIZE];
    do{
        // Affichage du menu
      for(int i=0;i<5;i++){
    ssize_t bytesRead = read(client_socket, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        printf("%s\n",buffer);
    } else if (bytesRead == 0) {
        printf("La connexion a été fermée par le serveur.\n");
    } else {
        error("Erreur lors de la lecture de la réponse du serveur");
    }
    write(client_socket,"Received",sizeof(char) * 9);
    }

        // Demande à l'utilisateur de choisir une option
        printf("Choix : ");
        scanf("%d", &choice);

        // Envoi du choix au serveur
            if (write(client_socket, &choice, sizeof(choice)) < 0) 
                error("Erreur lors de l'envoi du choi au serveur");
        switch (choice) {
            ssize_t bytesRead;
            case 1:
                bytesRead = read(client_socket, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    printf("%s\n",buffer);
                } else if (bytesRead == 0) {
                    printf("La connexion a été fermée par le serveur.\n");
                } else {
                    error("Erreur lors de la lecture de la réponse du serveur");
                }
                break;
            case 2:
                char path[256];
                printf("Saisir le chemin du répertoire\n");
                scanf("%s",path);
                write(client_socket,path,sizeof(path));
                
                bytesRead = read(client_socket, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    printf("%s\n",buffer);
                } else if (bytesRead == 0) {
                    printf("La connexion a été fermée par le serveur.\n");
                } else {
                    error("Erreur lors de la lecture de la réponse du serveur");
                }
                break;
            case 3:
                char name[256];
                printf("Saisir le nom du fichier\n");
                scanf("%s",name);
                write(client_socket,name,sizeof(name));
                
                bytesRead = read(client_socket, buffer, sizeof(buffer));
                if (bytesRead > 0) {
                    printf("%s\n",buffer);
                } else if (bytesRead == 0) {
                    printf("La connexion a été fermée par le serveur.\n");
                } else {
                    error("Erreur lors de la lecture de la réponse du serveur");
                }
                break;
            case 4:
                double timediff;
                bytesRead = read(client_socket, &timediff, sizeof(timediff));
                if (bytesRead > 0) {
                    printf("%f\n",timediff);
                } else if (bytesRead == 0) {
                    printf("La connexion a été fermée par le serveur.\n");
                } else {
                    error("Erreur lors de la lecture de la réponse du serveur");
                }
                break;
            case 5:
                printf("Déconnexion.\n");
                close(client_socket);
                exit(0);
            default:
                printf("Option invalide.\n");
        }
    }while(choice != 5);
    return 0;
}
