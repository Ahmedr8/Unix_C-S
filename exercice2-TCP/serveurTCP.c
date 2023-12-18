#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <dirent.h>
#define _GNU_SOURCE
#define MAX_BUFFER_SIZE 1024
typedef struct userCredentials{
    char username[100];
    char password[100];
}UserCredentials;
const UserCredentials users[] = {
    {
        "ahmed","ahmed"
    },
    {
        "elyes","elyes"
    },
    {
        "topadmin","topadmin"
    },
    {
        "root","root"
    },
    {
        "user","user"
    }
};
void error(const char *msg) {
    perror(msg);
    exit(1);
}
int loginUser(char username[100],char password[100],int client_socket){
    int length = sizeof(users)/sizeof(users[0]);
    int loggedIn = 0;
   for(int i = 0 ; i<length;i++){
    if (strcmp(users[i].username,username) == 0 && strcmp(users[i].password,password) == 0){
       loggedIn = 1;
       write(client_socket,&loggedIn,sizeof(loggedIn));
       return loggedIn;
    }
   }
   write(client_socket,&loggedIn,sizeof(loggedIn));
   return loggedIn;
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
    struct dirent *file;
    dir = opendir(directory_path);
    char buffer[1024] = "";
    if (dir != NULL){
        while((file = readdir(dir)) != NULL){
            strcat(buffer,strcat(file->d_name,"\n"));

        }
    closedir(dir);

    }
    else
    {
        strcat(buffer,"Le chemin saisi est invalide !");
    }
    if (write(client_socket,buffer,sizeof(buffer)) < 0)
        error("Erreur lors de l'envoi de la liste des fichiers");
}

// Fonction pour envoyer le contenu d'un fichier au client
void sendFileContent(int client_socket, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (file == NULL)
        printf("Fichier inexistant!");
    else
    {
    char buffer[MAX_BUFFER_SIZE];
    int bytesRead;
    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (write(client_socket, buffer, bytesRead) < 0) {
            error("Erreur lors de l'envoi du contenu du fichier au client");
        }
    }
    fclose(file);
    }
}

// Fonction pour envoyer la durée écoulée depuis le début de la connexion
void sendElapsedTime(int client_socket, time_t start_time) {
    time_t current_time;
    time(&current_time);
    double elapsed_time = difftime(current_time, start_time);
    if (write(client_socket, &elapsed_time, sizeof(elapsed_time)) < 0) {
        error("Erreur lors de l'envoi de la durée écoulée au client");
    }
}
void handleClient(int client_socket){

        time_t start_time;
        time(&start_time);
        int choice;
        do{
            char firstChoice[60] = "1-Afficher date Complète";
            char secondChoice[60] = "2-Afficher Liste Fichier";
            char thirdChoice[60] = "3-Afficher Contenu Fichier";
            char fourthChoice[90] = "4-Afficher Durée Ecoulée depuis le début de la connexion";
            char fifthChoice[20] = "5-Déconnexion";
            for (int i = 0 ; i < 5 ; i ++ ){
                if (i==0)
                    write(client_socket,firstChoice,sizeof(firstChoice));
                else if(i==1)
                    write(client_socket,secondChoice,sizeof(secondChoice));
                else if (i==2)
                    write(client_socket,thirdChoice,sizeof(thirdChoice));
                else if (i==3)
                    write(client_socket,fourthChoice,sizeof(fourthChoice));
                else if (i==4)
                    write(client_socket,fifthChoice,sizeof(fifthChoice));
                char buffer[10];
                int bytesRead = read(client_socket, buffer, sizeof(buffer));
                if (bytesRead == 0) {
                    printf("La connexion a été fermée par le serveur.\n");
                } 
                else if (bytesRead <0){
                    error("Erreur lors de la lecture de la réponse du serveur");
                }
            }
            if (read(client_socket, &choice, sizeof(choice)) < 0) {
                error("Erreur lors de la réception du choix du client");
            }
            switch (choice) {
                case 1:
                    // Envoie de la date complète (jour / mois / année) et l'heure (heure: minutes: secondes) de l'horloge système du serveur
                    sendDateTime(client_socket);
                    break;
                case 2:
                    char path[256];
                    read(client_socket,path,sizeof(path));
                    // Envoie de la liste des fichiers d'un répertoire
                    sendFileList(client_socket,path); // Vous pouvez changer "." avec le chemin du répertoire souhaité
                    break;
                case 3:
                    char name[256];
                    read(client_socket,name,sizeof(name));
                    sendFileContent(client_socket,name);
                    break;
                case 4:
                    // Envoie de la durée déjà écoulée depuis le début de la connexion du client courant
                    sendElapsedTime(client_socket, start_time);
                    break;
                case 5:
                    close(client_socket);
                    break;
                default:
                    printf("Option invalide.\n");
            }
        }while(choice!=5);
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
        error("Erreur lors de la mise en écoute de la socket\n");
    }

    printf("Serveur en attente sur le port %d...\n", server_port);
    int n;
    char username[100];
    char password[100];
    while (1) {

        socklen_t client_addr_len = sizeof(client_addr);
        // Attente d'une connexion client
        int client_socket =  0;
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket < 0) {
            error("Erreur lors de l'acceptation de la connexion client\n");
        }
        //FORK
        n = fork();
        if ( n < 0)
            perror("Erreur lors de la création du fils\n");
        if (n == 0){
            int loggedIn = 0;
            while(loggedIn == 0)
        {           
            read(client_socket,username,sizeof(username));
            read(client_socket,password,sizeof(password));
            loggedIn = loginUser(username,password,client_socket);
        }
        printf("Nouvelle connexion acceptée.\n");
            handleClient(client_socket);
        }
        else
        {
            close(client_socket);
        }
    // Fermeture de la socket du serveur
}
    close(server_socket);
    return 0;
}
