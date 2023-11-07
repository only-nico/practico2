#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
int main() {
    int sockfd_B;
    struct sockaddr_in client_B;

    // Crear un socket para B
    sockfd_B = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_B == -1) {
        perror("Error al crear el socket para B");
        return 1;
    }

    // Configurar la dirección y el puerto del servidor C para B
    client_B.sin_family = AF_INET;
    client_B.sin_port = htons(3002); // El mismo puerto que B
    client_B.sin_addr.s_addr = INADDR_ANY;

    // Enlazar el socket de B al puerto
    bind(sockfd_B, (struct sockaddr *)&client_B, sizeof(client_B));

    // Escuchar conexiones entrantes desde B
    listen(sockfd_B, 5);

    // Aceptar la conexión de B
    sockfd_B = accept(sockfd_B, nullptr, nullptr);

    // Realizar la comunicación con B
    // Puedes usar send y recv para enviar y recibir datos
    char buffer[1024];
    ssize_t bytesPeticion = recv(sockfd_B, buffer, sizeof(buffer), 0);
    string respuestaPeticion(buffer, bytesPeticion);
    send(sockfd_B,respuestaPeticion.c_str(),respuestaPeticion.length(),0);
    // Cerrar el socket de B
    close(sockfd_B);

    return 0;
}
