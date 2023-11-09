// #include <iostream>
// #include <unistd.h>
// #include <fstream>
// #include <cstring>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <queue>
// #include <sstream> 
// using namespace std;
// void cargarVariablesEnv() {
//     const char* envFile = "cache.env";
//     ifstream file(envFile);
//     if (!file) {
//         cerr << "No se pudo abrir el archivo .env" << endl;
//         exit(1);
//     }
//     string line;
//     while (getline(file, line)) {
//         size_t equalsPos = line.find('=');
//         if (equalsPos != string::npos) {
//             string key = line.substr(0, equalsPos);
//             string value = line.substr(equalsPos + 1);
//             setenv(key.c_str(), value.c_str(), 1);
//         }
//     }
//     file.close();
// }
// int main() {
//     while (true){
            
//         cargarVariablesEnv();
//         string host=getenv("HOST");
//         host.pop_back();
//         string front= getenv("FRONT");
//         front.pop_back();
//         string back= getenv("BACK");
//         back.pop_back();
//         string memorysize= getenv("MEMORYSIZE");
        
//         int sockfd_A, sockfd_C;
//         struct sockaddr_in client_A, server_C;

//         // Crear un socket para A
//         sockfd_A = socket(AF_INET, SOCK_STREAM, 0);
//         if (sockfd_A == -1) {
//             perror("Error al crear el socket para A");
//             return 1;
//         }

//         // Configurar la dirección y el puerto del servidor B para A
//         client_A.sin_family = AF_INET;
//         client_A.sin_port = htons(3001); // El mismo puerto que A
//         client_A.sin_addr.s_addr = INADDR_ANY;

//         // Enlazar el socket de A al puerto
//         bind(sockfd_A, (struct sockaddr *)&client_A, sizeof(client_A));

//         // Escuchar conexiones entrantes desde A
//         listen(sockfd_A, 5);

//         // Aceptar la conexión de A
//         sockfd_A = accept(sockfd_A, nullptr, nullptr);

//         // Crear un socket para C
//         sockfd_C = socket(AF_INET, SOCK_STREAM, 0);
//         if (sockfd_C == -1) {
//             perror("Error al crear el socket para C");
//             return 1;
//         }

//         // Configurar la dirección y el puerto del servidor C
//         server_C.sin_family = AF_INET;
//         server_C.sin_port = htons(3002); // Elige un puerto apropiado para C
//         server_C.sin_addr.s_addr = INADDR_ANY;

//         // Conectar al servidor C
//         if (connect(sockfd_C, (struct sockaddr *)&server_C, sizeof(server_C)) == -1) {
//             perror("Error al conectar con C");
//             return 1;
//         }
        
//         // Recibir el mensaje del socket
//         char buffer[1024];  // Tamaño del búfer para el mensaje (ajústalo según tus necesidades)
//         ssize_t bytesReceived = recv(sockfd_A, buffer, sizeof(buffer), 0);
//         string receivedMessage(buffer, bytesReceived);
        
//         if (bytesReceived == -1) {
//             perror("Error al recibir el mensaje");
//             return 1;
//         }

//         // Convertir el mensaje en una cadena C++
//         string mensajeRecibido=receivedMessage;

//         cout << "RECICBI: " << mensajeRecibido << endl;

//         string from, to, mensaje;
//         istringstream ss(receivedMessage);

//         getline(ss, from, ',');  // Lee el valor "/desde" y lo almacena en 'from'
//         getline(ss, to, ',');  // Lee el valor "/hacia" y lo almacena en 'to'
//         getline(ss, mensaje);

//         cout << from << "->" << to << ":" << mensaje << endl;

//         queue<pair<string,vector<string>>> cache;

//         //lo siguiente es para tener algo en cache al principio
//         pair<string,vector<string>> elemento1 = make_pair("hola", vector<string>{"info1", "info2", "info3"});
//         cache.push(elemento1);

//         pair<string,vector<string>> elemento2 = make_pair("chao", vector<string>{"info1", "info2", "info3"});
//         cache.push(elemento2);

//         vector<string> vec ;
//         queue<pair<string,vector<string>>> copiacache= cache;
//         string vectorRespuesta = "";
//         bool condicion=true;//para verficar que el elemento se encontro, si no, lo buscara en el backend
//         while(!copiacache.empty()) {
//             pair<string,vector<string>> elemento= copiacache.front();
//             cout << elemento.first << endl;
//             if (elemento.first == mensaje) {
//                 cout << "Se encontró el elemento " << endl;
//                 vec = elemento.second;
//                 condicion=false;
//                 break; // Sal del bucle si se encuentra una coincidencia
//             }
//             cout << "COPIA CACHE FIRST? ";
//             cout << copiacache.front().first << endl;
//             copiacache.pop();
//         }
//         // cout << "sali" << endl;
//         if(!condicion){
//             // Iterar sobre los elementos del vector y agregarlos a la cadena
//             for (const string& element : vec) {
//                 // Convertir cada elemento a una cadena y agregarlo a la cadena principal
//                 vectorRespuesta += element + " ";
//             }
//             // vectorRespuesta += "" QUIERO INDICAR CACHE
//         }else{
//             cout << "else" << endl;
//             string peticion= host+","+back+","+mensaje;

//             cout << "ENVIANDO A C: " << peticion << endl;

//             send(sockfd_C,peticion.c_str(),peticion.length(),0);

//             ssize_t bytesPeticion = recv(sockfd_C, buffer, sizeof(buffer), 0);
//             string respuestaPeticion(buffer, bytesPeticion);

//             cout << "RECIBI DE C: " << respuestaPeticion << endl;

//             vectorRespuesta += respuestaPeticion + "BACKEND";
//         }
//         string respuesta = vectorRespuesta;
//         cout << "ENVIANDO A A: " << respuesta;


//         send(sockfd_A, respuesta.c_str(), respuesta.length(), 0);

//         cout << "ENVIADO" << endl;
        
//         // Cerrar los sockets de A y C
//         close(sockfd_A);
//         close(sockfd_C);
    
//     }
//     return 0;

// }


#include <iostream>
#include <unistd.h>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <queue>
#include <sstream> 
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
    std::string origen;
    std::string destino;
    Contexto contexto;
};

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

// Definir una función para serializar un Contexto a un json
void to_json(json& j, const Contexto& c) {
    j = json{{"txtToSerarch", c.txtToSerarch}, {"tiempo", c.tiempo}, {"ori", c.ori}, {"isFound", c.isFound}, {"resultados", c.resultados}};
}

// Definir una función para serializar un Mensaje a un json
void to_json(json& j, const Mensaje& m) {
    j = json{{"origen", m.origen}, {"destino", m.destino}, {"contexto", m.contexto}};
}

int main() {
    Mensaje mensaje;
    cargarVariablesEnv();
    string host = getenv("HOST");
    host.pop_back();
    string front = getenv("FRONT");
    front.pop_back();
    string back = getenv("BACK");
    back.pop_back();
    string memorysize = getenv("MEMORYSIZE");

    int sockfd_A, sockfd_C;
    struct sockaddr_in client_A, server_C;

    // Crear un socket para A fuera del bucle while
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

    while (true) {
        // Aceptar la conexión de A
        int connection_A = accept(sockfd_A, nullptr, nullptr);

        // Verificar si la conexión fue aceptada correctamente
        if (connection_A == -1) {
            perror("Error al aceptar la conexión de A");
            continue;
        }
        vector<Contexto> cache;
        while (true) {
            
            char buffer[1024]; // Tamaño del búfer para el mensaje (ajústalo según tus necesidades)
            ssize_t bytesReceived = recv(connection_A, buffer, sizeof(buffer), 0);
            string receivedMessage(buffer, bytesReceived);

            if (bytesReceived == -1) {
                perror("Error al recibir el mensaje");
                break;
            }

            json j = json::parse(receivedMessage);

            // Convertir el mensaje en una cadena C++
            // string mensajeRecibido = receivedMessage;

            Mensaje mensaje = j.get<Mensaje>();


            cout << "RECIBI: " << mensaje.contexto.txtToSerarch << endl;

            string from2, to2, texto;

            from2 = mensaje.origen;
            to2 = mensaje.destino;
            texto = mensaje.contexto.txtToSerarch;

            /*istringstream ss(receivedMessage);

            getline(ss, from, ',');  // Lee el valor "/desde" y lo almacena en 'from'
            getline(ss, to, ',');  // Lee el valor "/hacia" y lo almacena en 'to'
            getline(ss, mensaje);
            */
            bool encontradoEnCache = false;
            for (const auto& contexto : cache) {
                cout<<contexto.txtToSerarch<<endl;
                if (contexto.txtToSerarch == texto) {
                    // Encontrado en la caché
                    encontradoEnCache = true;
                    Mensaje mensaje;
                    mensaje.origen = host;
                    mensaje.destino = front;
                    mensaje.contexto = contexto;
                    json j = mensaje;

                    cout << "ENVIANDO A A: " << j.dump(4) << endl;
                    send(connection_A, j.dump(4).c_str(), j.dump(4).length(), 0);
                    cout << "ENVIADO!!!" << endl;
                }
            }

            // Si no se encontró en la caché, buscar en el backend
            if (!encontradoEnCache) {
                Mensaje mensaje;
                mensaje.origen = host;
                mensaje.destino = back;
                mensaje.contexto.isFound = false;
                mensaje.contexto.txtToSerarch=texto;
                json j = mensaje;
                cout << "ENVIANDO A C: " << j.dump(4) << endl;
                send(sockfd_C, j.dump(4).c_str(), j.dump(4).length(), 0);
                cout << "ENVIADO!!!" << endl;

                ssize_t bytesPeticion = recv(sockfd_C, buffer, sizeof(buffer), 0);
                string respuestaPeticion(buffer, bytesPeticion);

                cout << "RECIBI DE C: " << respuestaPeticion << endl;

                j = json::parse(respuestaPeticion);
                mensaje = j.get<Mensaje>();

                // Actualizar la caché
                if (cache.size() >= 4) {
                    cache.pop_back();
                    mensaje.contexto.ori="cache";
                    cache.push_back(mensaje.contexto);
                } else {
                    mensaje.contexto.ori="cache";
                    cache.push_back(mensaje.contexto);
                }

                mensaje.origen = host;
                mensaje.destino = front;

                j = mensaje;
                cout << "ENVIANDO A A: " << j.dump(4).c_str() << endl;

                send(connection_A, j.dump(4).c_str(), j.dump(4).length(), 0);

                cout << "ENVIADO" << endl;
            }
        }
        // Cerrar el socket de A
        close(connection_A);
    }
    
    // Cerrar el socket principal de A al final del programa
    close(sockfd_C);
    return 0;
}
