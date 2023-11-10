#include <iostream>
#include <unistd.h>
#include <cstring>
#include <fstream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

using namespace std;

struct Contexto {
    std::string txtToSerarch;
    std::string tiempo;
    std::string ori;
    bool isFound;
    std::vector<std::pair<std::string, int>> resultados;
};

struct Mensaje {
    string origen;
    string destino;
    Contexto contexto;
};

string menu(){
    string mensaje="";
    cout<<"BUSCADOR BASADO EN INDICE INVERTIDO"<<endl;
    cout<<endl;
    cout<<"Escriba el texto a buscar: ";
    getline(cin,mensaje);
    cout << "MENSAJE INGRESADO: " << mensaje << endl << endl;
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

// Definir una función para serializar un Contexto a un json
void to_json(json& j, const Contexto& c) {
    j = json{{"txtToSerarch", c.txtToSerarch}, {"tiempo", c.tiempo}, {"ori", c.ori}, {"isFound", c.isFound}, {"resultados", c.resultados}};
}

// Definir una función para serializar un Mensaje a un json
void to_json(json& j, const Mensaje& m) {
    j = json{{"origen", m.origen}, {"destino", m.destino}, {"contexto", m.contexto}};
}

// Definir una función para deserializar un json a un Contexto
void from_json(const json& j, Contexto& c) {
    j.at("txtToSerarch").get_to(c.txtToSerarch);
    j.at("tiempo").get_to(c.tiempo);
    j.at("ori").get_to(c.ori);
    j.at("isFound").get_to(c.isFound);
    j.at("resultados").get_to(c.resultados);
}

// Definir una función para deserializar un json a un Mensaje
void from_json(const json& j, Mensaje& m) {
    j.at("origen").get_to(m.origen);
    j.at("destino").get_to(m.destino);
    j.at("contexto").get_to(m.contexto);
}

void imprimirMensaje(Mensaje mensaje){
    cout << "Respuesta: (tiempo = " << mensaje.contexto.tiempo << ", origen=" << mensaje.contexto.ori << ")\n";

    if(mensaje.contexto.isFound == true)
        for (int i = 0; i < mensaje.contexto.resultados.size(); i++) 
            std::cout << "\t" << i + 1 << ") texto: " << mensaje.contexto.resultados[i].first << ", puntaje: " << mensaje.contexto.resultados[i].second << std::endl;
    else
        cout << "No se encontraron coincidencias" << endl;
    
}

int main() {
    cargarVariablesEnv();
    string from = getenv("FROM");
    string to = getenv("TO");
    from.pop_back();
    to.pop_back();

    int sockfd_B;
    struct sockaddr_in server_B;

    if(from == to){
        cout << "FROM == TO" << endl;
        return 1;
    }

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
        Mensaje mensaje;
        mensaje.origen = from;
        mensaje.destino = to;
        mensaje.contexto.txtToSerarch = texto;

        // Convertir el objeto Mensaje a un json
        json j = mensaje;

        auto start = std::chrono::steady_clock::now();
        string mensaje2 = from +","+to + ","+ texto;

        send(sockfd_B, j.dump(4).c_str(), j.dump(4).length(), 0);


        char buffer[1024];  // Tamaño del búfer para el mensaje (ajústalo según tus necesidades)
        ssize_t bytesReceived = recv(sockfd_B, buffer, sizeof(buffer), 0);
        string receivedMessage(buffer, bytesReceived);
    
        auto end = std::chrono::steady_clock::now();
        
        auto diff = end - start;

        j = json::parse(receivedMessage);
        mensaje = j.get<Mensaje>();

        mensaje.contexto.tiempo =   to_string(::chrono::duration_cast<std::chrono::nanoseconds>(diff).count()) + " ns.";

        imprimirMensaje(mensaje);

        cout << "\nDesea salir (S/N): ";
        char respuesta;
        cin >> respuesta;
        cout << "RESPUESTA: " << respuesta << endl;
        if (respuesta == 'S' || respuesta == 's') {
            cout << "SALIENDO" << endl;
            break; // Salir del bucle
        }
        else if(respuesta == 'N' || respuesta == 'n'){
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cin.clear();
            limpiarPantalla();
        }
        else{
            cout << "Respuesta invalida." << endl;
            break;
        }
            

    }

    close(sockfd_B);
    
    return 0;
}



