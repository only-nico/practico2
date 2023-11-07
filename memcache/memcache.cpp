#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#include <sstream> 
using namespace std;
void cargarVariablesEnv() {
    const char* envFile = "cache.env";
    ifstream file(envFile);
    if (!file) {
        cerr << "No se pudo abrir el archivo .env" << endl;
        exit(1);
    }
    string line;
    while (getline(file, line)) {
        size_t equalsPos = line.find('=');
        if (equalsPos != string::npos) {
            string key = line.substr(0, equalsPos);
            string value = line.substr(equalsPos + 1);
            setenv(key.c_str(), value.c_str(), 1);
        }
    }
    file.close();
}
int main() {
    cargarVariablesEnv();
    string host=getenv("HOST");
    host.pop_back();
    string front= getenv("FRONT");
    front.pop_back();
    string back= getenv("BACK");
    back.pop_back();
    string memorysize= getenv("MEMORYSIZE");
    
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
    string receivedMessage(buffer, bytesReceived);
    
    if (bytesReceived == -1) {
        perror("Error al recibir el mensaje");
        return 1;
    }

    // Convertir el mensaje en una cadena C++
    string mensajeRecibido=receivedMessage;
    string from, to, mensaje;
    istringstream ss(receivedMessage);

    getline(ss, from, ',');  // Lee el valor "/desde" y lo almacena en 'desde'
    getline(ss, to, ',');  // Lee el valor "/hacia" y lo almacena en 'hacia'
    getline(ss, mensaje);

    queue<pair<string,vector<string>>> cache;
    //lo siguiente es para tener algo en cache al principio
    pair<string,vector<string>> elemento1 = make_pair("hola", vector<string>{"info1", "info2", "info3"});
    cache.push(elemento1);

    pair<string,vector<string>> elemento2 = make_pair("chao", vector<string>{"info1", "info2", "info3"});
    cache.push(elemento2);
    vector<string> vec ;
    queue<pair<string,vector<string>>> copiacache= cache;
    string vectorRespuesta = "";
    bool condicion=true;//para verficar que el elemento se encontro, si no, lo buscara en el backend
    while(!copiacache.empty()||condicion) {
            pair<string,vector<string>> elemento= copiacache.front();
            if (elemento.first == mensaje) {
                cout << "Se encontró el elemento " << endl;
                vec = elemento.second;
                condicion=false;
                break; // Sal del bucle si se encuentra una coincidencia
            }
            copiacache.pop();
        }
    if(!condicion){
        // Iterar sobre los elementos del vector y agregarlos a la cadena
        for (const string& element : vec) {
            // Convertir cada elemento a una cadena y agregarlo a la cadena principal
            vectorRespuesta += element + " ";
        }
    }else{
        string peticion= host+","+back+","+mensaje;
        send(sockfd_C,peticion.c_str(),peticion.length(),0);
        ssize_t bytesPeticion = recv(sockfd_C, buffer, sizeof(buffer), 0);
        string respuestaPeticion(buffer, bytesReceived);
        string desde, hacia, mensaje;
        istringstream ss(respuestaPeticion);

        getline(ss, from, ',');  // Lee el valor "/desde" y lo almacena en 'desde'
        getline(ss, to, ',');  // Lee el valor "/hacia" y lo almacena en 'hacia'
        getline(ss, mensaje);
    }
    string respuesta= mensaje+","+vectorRespuesta;
    send(sockfd_A, respuesta.c_str(), respuesta.length(), 0);
    
    // Cerrar los sockets de A y C
    close(sockfd_A);
    close(sockfd_C);
    
    return 0;

}
