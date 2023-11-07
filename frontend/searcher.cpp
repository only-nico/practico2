#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
using namespace std;
string menu(){
    string mensaje="";
    cout<<"BUSCADOR BASADO EN INDICE INVERTIDO"<<endl;
    cout<<endl;
    cout<<"Escriba el texto a buscar: ";
    getline(cin,mensaje);
    return mensaje;
}
void limpiarPantalla(){
    #ifdef _WIN32
            system("cls"); 
            #else
            system("clear"); 
    #endif
}
void cargarVariablesEnv() {
    const char* envFile = "front.env";
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
    string from = getenv("FROM");
    string to = getenv("TO");
    from.pop_back();
    int sockfd_B;
    struct sockaddr_in server_B;

    // Crear un socket para B
    sockfd_B = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd_B == -1) {
        perror("Error al crear el socket para B");
        return 1;
    }

    // Configurar la dirección y el puerto del servidor B
    server_B.sin_family = AF_INET;
    server_B.sin_port = htons(3001); // Elige un puerto apropiado
    server_B.sin_addr.s_addr = INADDR_ANY;

    // Conectar al servidor B
    if (connect(sockfd_B, (struct sockaddr *)&server_B, sizeof(server_B)) == -1) {
        perror("Error al conectar con B");
        return 1;
    }
    bool salir= true;
    while(salir){
        string texto = menu();
        string mensaje = from +","+to + ","+ texto;
        send(sockfd_B, mensaje.c_str(), mensaje.length(), 0);
        char buffer[1024];  // Tamaño del búfer para el mensaje (ajústalo según tus necesidades)
        ssize_t bytesReceived = recv(sockfd_B, buffer, sizeof(buffer), 0);
        std::string receivedMessage(buffer, bytesReceived);
        cout<<receivedMessage<<endl;
        cout << "Desea salir (S/N): ";
            char respuesta;
            cin >> respuesta;
            if (respuesta == 'S' || respuesta == 's') {
                break; // Salir del bucle
            }
            limpiarPantalla();
    }

    close(sockfd_B);
    
    return 0;
}



