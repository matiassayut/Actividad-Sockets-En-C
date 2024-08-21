#include <stdio.h>
#include <winsock2.h>
#include <string.h>
#include <stdbool.h>

#pragma comment(lib, "ws2_32.lib")

typedef struct Client {
    WSADATA WSAData;
    SOCKET server;
    SOCKADDR_IN addr;
    char buffer[1024];
    bool activo;
    int puerto;
} Client;

void Client_Init(Client* cliente);
void Client_Enviar(Client* cliente);
int Client_Recibir(Client* cliente);
void Client_CerrarSocket(Client* cliente);

void Client_Init(Client* cliente) {
    cliente->activo = true;

    printf("Ingrese el puerto al que quiere conectarse: ");
    scanf("%d", &(cliente->puerto));
    printf("Conectando al servidor...\n\n");

    WSAStartup(MAKEWORD(2, 0), &(cliente->WSAData));
    cliente->server = socket(AF_INET, SOCK_STREAM, 0);
    cliente->addr.sin_addr.s_addr = inet_addr("192.168.1.43");
    cliente->addr.sin_family = AF_INET;
    cliente->addr.sin_port = htons(cliente->puerto);

    while (connect(cliente->server, (SOCKADDR*)&(cliente->addr), sizeof(cliente->addr)) == -1) {
        printf("Ha ingresado mal el puerto\n");
        printf("Ingrese un puerto nuevamente: ");
        scanf("%d", &(cliente->puerto));
        cliente->addr.sin_port = htons(cliente->puerto);
    }

    printf("Conectado al Servidor!\n\n");
}

void Client_Enviar(Client* cliente) {
    char mensaje[1024] = { 0 };
    char opcion[2] = { 0 };
    bool salir = false;

    do {
        printf("Por favor, ingrese una de las siguientes opciones\n");
        printf("Opcion 1: Generar nombre de usuario\n");
        printf("Opcion 2: Generar contrasenia\n");
        printf("Opcion 3: Cerrar sesion\n");
        scanf("%s", opcion);

        if (strlen(opcion) > 1) {
            opcion[0] = '4';  // opción inválida
        }

        switch (opcion[0]) {
            case '1':
                printf("Ingrese la longitud deseada: (Entre 5 y 15)\n");
                getchar();  // limpiar el buffer del teclado
                while (fgets(mensaje, sizeof(mensaje), stdin)) {
                    if (strcmp(mensaje, "volver") == 0) {
                        break;
                    }
                    // Verificar si el número está en el rango deseado
                    if (atoi(mensaje) >= 5 && atoi(mensaje) <= 15) {
                        printf("Longitud aceptada:\n");
                        break;  // Salir del bucle si el número es válido
                    } else {
                        printf("Longitud invalida. Intente nuevamente.\n");
                    }
                }
                mensaje[strcspn(mensaje, "\n")] = 0;  // eliminar el salto de línea
                memmove(mensaje + 1, mensaje, strlen(mensaje) + 1); // insertar 'a' al principio
                mensaje[0] = 'a';  // insertar carácter de comando al inicio
                strcpy(cliente->buffer, mensaje);
                salir = true;
                break;

            case '2':
                printf("Ingrese la longitud deseada: (Entre 8 inclusive y 50)\n");
                getchar();  // limpiar el buffer del teclado
                while (fgets(mensaje, sizeof(mensaje), stdin)) {
                    if (strcmp(mensaje, "volver") == 0) {
                        break;
                    }
                    // Verificar si el número está en el rango deseado
                    if (atoi(mensaje) >= 8 && atoi(mensaje) < 50) {
                        printf("Longitud aceptada:\n");
                        break;  // Salir del bucle si el número es válido
                    } else {
                        printf("Longitud invalida. Intente nuevamente.\n");
                    }
                    
                }
                mensaje[strcspn(mensaje, "\n")] = 0;  // eliminar el salto de línea
                memmove(mensaje + 1, mensaje, strlen(mensaje) + 1); // insertar 'b' al principio
                mensaje[0] = 'b';  // insertar carácter de comando al inicio
                strcpy(cliente->buffer, mensaje);
                salir = true;
                break;

            case '3':
                strcpy(cliente->buffer, "c");
                send(cliente->server, cliente->buffer, sizeof(cliente->buffer), 0);
                memset(cliente->buffer, 0, sizeof(cliente->buffer));
                Client_CerrarSocket(cliente);
                cliente->activo = false;
                salir = true;
                break;

            default:
                printf("Error, ha ingresado un caracter incorrecto\n\n");
                break;
        }
    } while (!salir);

    send(cliente->server, cliente->buffer, sizeof(cliente->buffer), 0);
    memset(cliente->buffer, 0, sizeof(cliente->buffer));
    printf("El mensaje ha sido enviado.\n");
}

int Client_Recibir(Client* cliente) {
    int rec = recv(cliente->server, cliente->buffer, sizeof(cliente->buffer), 0);
    printf("\nEl servidor dice: \n%s\n\n", cliente->buffer);
    memset(cliente->buffer, 0, sizeof(cliente->buffer));
    return rec;
}

void Client_CerrarSocket(Client* cliente) {
    closesocket(cliente->server);
    WSACleanup();
}

int main() {
    Client cliente;
    Client_Init(&cliente);

    while (cliente.activo) {
        Client_Enviar(&cliente);
        if (cliente.activo) {
            if (Client_Recibir(&cliente) == -1) {
                Client_Init(&cliente);
            }
        }
    }

    printf("Se ha cerrado la conexion.\n");
    return 0;
}
