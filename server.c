#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <time.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000

typedef struct Server {
    WSADATA WSAData;
    SOCKET server, client;
    SOCKADDR_IN serverAddr, clientAddr;
    char buffer[1024];
    char bufferString[];
} Server;

void Server_Init(Server* servidor);
int Server_Recibir(Server* servidor);
void Server_Enviar(Server* servidor);
void Server_CerrarSocket(Server* servidor);
void Server_EsperarCliente(Server* servidor);
void GenerarNombreDeUsuario(char *cadena, int longitud);
void Generarcontrasenia(char *cadena, int longitud);

void Server_Init(Server* servidor) {
    WSAStartup(MAKEWORD(2, 0), &(servidor->WSAData));
    servidor->server = socket(AF_INET, SOCK_STREAM, 0);
    servidor->serverAddr.sin_addr.s_addr = INADDR_ANY;
    servidor->serverAddr.sin_family = AF_INET;
    servidor->serverAddr.sin_port = htons(PORT);
    
    DWORD timeout = 120 * 1000;
    setsockopt(servidor->server, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);
    bind(servidor->server, (SOCKADDR*)&(servidor->serverAddr), sizeof(servidor->serverAddr));
    listen(servidor->server, 0);

    printf(" =============================\n");
    printf(" =======Inicia Servidor=======\n");
    printf(" =============================\n");
    printf(" Socket creado. Puerto de escucha: %d\n", PORT);

    int clientAddrSize = sizeof(servidor->clientAddr);
    if ((servidor->client = accept(servidor->server, (SOCKADDR*)&(servidor->clientAddr), &clientAddrSize)) != INVALID_SOCKET) {
        printf(" Conexion Aceptada.\n");
    }
}

int Server_Recibir(Server* servidor) {
    int mensaje = recv(servidor->client, servidor->buffer, sizeof(servidor->buffer), 0);
    strcpy(servidor->bufferString, servidor->buffer);
    memset(servidor->buffer, 0, sizeof(servidor->buffer));
    return mensaje;
}

void Server_Enviar(Server* servidor) {
    char opcion = servidor->bufferString[0];
    switch (opcion) {
        case 'a':
            printf("El cliente solicito Generar nombre de usuario.\n");
            memmove (servidor->bufferString, servidor->bufferString+1, strlen (servidor->bufferString+1) + 1); //Remover primer char correspondiente a la opcion
            GenerarNombreDeUsuario(servidor->buffer,atoi(servidor->bufferString));
            printf("El resultado enviado es: %s\n",servidor->buffer);
            send(servidor->client, servidor->buffer, sizeof(servidor->buffer), 0);
            memset(servidor->buffer, 0, sizeof(servidor->buffer));
            break;
        
        case 'b':
            printf("El cliente solicito Generar contrasenia.\n");
            memmove (servidor->bufferString, servidor->bufferString+1, strlen (servidor->bufferString+1) + 1); //Remover primer char correspondiente a la opcion
            Generarcontrasenia(servidor->buffer,atoi(servidor->bufferString));
            printf("El resultado enviado es: %s\n",servidor->buffer);
            send(servidor->client, servidor->buffer, sizeof(servidor->buffer), 0);
            memset(servidor->buffer, 0, sizeof(servidor->buffer));
            break;
        
        case 'c':
            printf("Conexion Cerrada por el cliente.\n");
            strcpy(servidor->buffer, "La conexion se ha cerrado.");
            send(servidor->client, servidor->buffer, sizeof(servidor->buffer), 0);
            memset(servidor->buffer, 0, sizeof(servidor->buffer));
            Server_CerrarSocket(servidor);
            Server_EsperarCliente(servidor);
            break;
        
        default:
            break;
    }
}

void Server_CerrarSocket(Server* servidor) {
    closesocket(servidor->client);
}

void Server_EsperarCliente(Server* servidor) {
    int clientAddrSize = sizeof(servidor->clientAddr);
    if ((servidor->client = accept(servidor->server, (SOCKADDR*)&(servidor->clientAddr), &clientAddrSize)) != INVALID_SOCKET) {
        printf(" Conexion Aceptada.\n");
    }
}

int main() {
    Server servidor;
    Server_Init(&servidor);

    while (1) {
        Server_Recibir(&servidor);
        Server_Enviar(&servidor);
    }

    return 0;
}

void GenerarNombreDeUsuario(char *cadena, int longitud) {
    const char consonantes[] = "bcdfghjklmnpqrstvwxyz";
    const char vocales[] = "aeiou";
    int num_consonantes = sizeof(consonantes) - 1;
    int num_vocales = sizeof(vocales) - 1;
    srand(time(NULL));
    int es_vocal = rand() % 2;
    for (int i = 0; i < longitud; i++) {
        if (es_vocal) {
            cadena[i] = vocales[rand() % num_vocales];
        } else {
            cadena[i] = consonantes[rand() % num_consonantes];
        }
        es_vocal = !es_vocal; //esto se realiza para que intercale entre vocales y consonantes
    }
}

void Generarcontrasenia(char *cadena, int longitud) {
    const char caracteres[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int num_caracteres = sizeof(caracteres) - 1;
    srand(time(NULL));
    for (int i = 0; i < longitud; i++) {
        cadena[i] = caracteres[rand() % num_caracteres];
    }
}