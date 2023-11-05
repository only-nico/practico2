#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;

int main() {
    int sockfd_A, sockfd_C;
    struct sockaddr_in client_A, server_C;

    // Crear un socket para A
    sockfd_A = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_A == -1) {
        perror("Error al crear el socket para A");
        return 1;
    }

    // Configurar la dirección y el puerto del servidor B para A
    client_A.sin_family = AF_INET;
    client_A.sin_port = htons(3001); // El mismo puerto que A
    client_A.sin_addr.s_addr = INADDR_ANY;

    // Enlazar el socket de A al puerto
    bind(sockfd_A, (struct sockaddr *)&client_A, sizeof(client_A));

    // Escuchar conexiones entrantes desde A
    listen(sockfd_A, 5);

    // Aceptar la conexión de A
    sockfd_A = accept(sockfd_A, nullptr, nullptr);

    // Crear un socket para C
    sockfd_C = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_C == -1) {
        perror("Error al crear el socket para C");
        return 1;
    }

    // Configurar la dirección y el puerto del servidor C
    server_C.sin_family = AF_INET;
    server_C.sin_port = htons(3002); // Elige un puerto apropiado para C
    server_C.sin_addr.s_addr = INADDR_ANY;

    // Conectar al servidor C
    if (connect(sockfd_C, (struct sockaddr *)&server_C, sizeof(server_C)) == -1) {
        perror("Error al conectar con C");
        return 1;
    }
    // Recibir el mensaje del socket
    char buffer[1024];  // Tamaño del búfer para el mensaje (ajústalo según tus necesidades)
    ssize_t bytesReceived = recv(sockfd_A, buffer, sizeof(buffer), 0);
    if (bytesReceived == -1) {
        perror("Error al recibir el mensaje");
        return 1;
    }

    // Convertir el mensaje en una cadena C++
    std::string receivedMessage(buffer, bytesReceived);
    string mensaje=receivedMessage;
    cout<<receivedMessage<<endl;
    send(sockfd_A, mensaje.c_str(), mensaje.length(), 0);
    // Realizar la comunicación con A y C
    // Puedes usar send y recv para enviar y recibir datos

    // Cerrar los sockets de A y C
    close(sockfd_A);
    close(sockfd_C);
    
    return 0;

}
