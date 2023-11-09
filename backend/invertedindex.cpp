// #include <iostream>
// #include <unistd.h>
// #include <cstring>
// #include <sys/socket.h>
// #include <netinet/in.h>
// #include <sstream>
// #include <fstream>
// #include <vector>
// #include <map>

// using namespace std;

// map<string, vector<pair<string, int>>> cargarIndex(string filename) {
//     map<string, vector<pair<string, int>>> result;
//     ifstream file(filename);
//     string line;
//     while (getline(file, line)) {
//         string key = line.substr(0, line.find(":"));
//         string value = line.substr(line.find(":") + 1);
//         vector<pair<string, int>> files;
//         size_t start = 0;
//         size_t end = 0;
//         if (value.find(";") == string::npos) {
//             string file = value.substr(1, value.size() - 2);
//             size_t semicolon = file.find(";");
//             string filename = file.substr(0, semicolon);
//             int number = stoi(file.substr(semicolon + 1));
//             files.push_back(make_pair(filename, number));
//         } else {
//             while ((end = value.find(");", start)) != string::npos) {
//                 string file = value.substr(start + 1, end - start - 1);
//                 size_t semicolon = file.find(";");
//                 string filename = file.substr(0, semicolon);
//                 int number = stoi(file.substr(semicolon + 1));
//                 files.push_back(make_pair(filename, number));
//                 start = end + 2;
//             }
//         }
//         result[key] = files;
//     }

//     file.close();

//     // for (auto const& [key, val] : result) {
//     //     cout << key << ": ";
//     //     for (auto const& [filename, number] : val) {
//     //         cout << "(" << filename << ", " << number << ") ";
//     //     }
//     //     cout << endl;
//     // }
//     return result;
// }

// std::vector<std::string> buscarPalabra(string palabra, map<string, vector<pair<string, int>>>  invertedIndexMap)  {
//     // Imprime el contenido del mapa
//     vector<std::string> retornar;

//     for (auto const& [key, val] : invertedIndexMap) {
//         if(key == palabra){
//             string r;
//             for (auto const& [filename, number] : val) {
//                 r = "(" + filename + ", " + to_string(number) + ")";
//                 retornar.push_back(r);
//             }
//             return retornar;
//         }
//     }
//     return std::vector<std::string>();
// }


// int main() {

//     map<string, vector<pair<string, int>>>invertedIndex = cargarIndex("file.idx");

//     while (true) {
//         int sockfd_B;
//         struct sockaddr_in client_B;

//         // Crear un socket para B
//         sockfd_B = socket(AF_INET, SOCK_STREAM, 0);
//         if (sockfd_B == -1) {
//             perror("Error al crear el socket para B");
//             return 1;
//         }

//         // Configurar la dirección y el puerto del servidor C para B
//         client_B.sin_family = AF_INET;
//         client_B.sin_port = htons(3002); // El mismo puerto que B
//         client_B.sin_addr.s_addr = INADDR_ANY;

//         // Enlazar el socket de B al puerto
//         bind(sockfd_B, (struct sockaddr *)&client_B, sizeof(client_B));

//         // Escuchar conexiones entrantes desde B
//         listen(sockfd_B, 5);

//         // Aceptar la conexión de B
//         sockfd_B = accept(sockfd_B, nullptr, nullptr);

//         // Realizar la comunicación con B
//         // Puedes usar send y recv para enviar y recibir datos
//         char buffer[1024];
//         ssize_t bytesPeticion = recv(sockfd_B, buffer, sizeof(buffer), 0);
//         string respuestaPeticion(buffer, bytesPeticion);

//         cout << "RECIBI DE B: " << respuestaPeticion << endl;

//         // string mensajeRecibido = respuestaPeticion;
//         istringstream ss(respuestaPeticion);
//         string from, to, mensaje;
//         getline(ss, from, ',');  // Lee el valor "/desde" y lo almacena en 'from'
//         getline(ss, to, ',');  // Lee el valor "/hacia" y lo almacena en 'to'
//         getline(ss, mensaje);

//         cout << from << "->" << to << ":" << mensaje << endl;

//         std::vector<std::string> ocurrencias = buscarPalabra(mensaje, invertedIndex);

//         string mensajeEnviar = "";

//         cout << "OCURRENCIAS: ";
//         for (string ocurrencia : ocurrencias) {
//             cout << ocurrencia << " ";
//             mensajeEnviar += ocurrencia + ";"; // Agrega un ';' después de cada ocurrencia
//         }

//         cout << "DEBERIA ENVIAR A B: " << mensajeEnviar.c_str() << endl; 
//         send(sockfd_B, mensajeEnviar.c_str(), strlen(mensajeEnviar.c_str()), 0);


//         // Cerrar el socket de B
//         close(sockfd_B);
//     }
//     return 0;
// }


#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
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

map<string, vector<pair<string, int>>> cargarIndex(string filename) {
    map<string, vector<pair<string, int>>> result;
    ifstream file(filename);
    string line;
    while (getline(file, line)) {
        string key = line.substr(0, line.find(":"));
        string value = line.substr(line.find(":") + 1);
        vector<pair<string, int>> files;
        size_t start = 0;
        size_t end = 0;
        if (value.find(";") == string::npos) {
            string file = value.substr(1, value.size() - 2);
            size_t semicolon = file.find(";");
            string filename = file.substr(0, semicolon);
            int number = stoi(file.substr(semicolon + 1));
            files.push_back(make_pair(filename, number));
        } else {
            while ((end = value.find(");", start)) != string::npos) {
                string file = value.substr(start + 1, end - start - 1);
                size_t semicolon = file.find(";");
                string filename = file.substr(0, semicolon);
                int number = stoi(file.substr(semicolon + 1));
                files.push_back(make_pair(filename, number));
                start = end + 2;
            }
        }
        result[key] = files;
    }

    file.close();

    // for (auto const& [key, val] : result) {
    //     cout << key << ": ";
    //     for (auto const& [filename, number] : val) {
    //         cout << "(" << filename << ", " << number << ") ";
    //     }
    //     cout << endl;
    // }
    return result;
}

void buscarPalabra(Mensaje& mensaje, map<string, vector<pair<string, int>>>  invertedIndexMap, string texto)  {
    // Imprime el contenido del mapa
    vector<std::string> retornar;

    for (auto const& [key, val] : invertedIndexMap) {
        if(key == texto){
            mensaje.contexto.isFound = true;
            for (auto const& [filename, number] : val) {
                mensaje.contexto.resultados.push_back({filename, number});
            }
        }
    }
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

int main() {

    map<string, vector<pair<string, int>>>invertedIndex = cargarIndex("file.idx");

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

    while (true) {
        // Aceptar la conexión de B
        int connection_B = accept(sockfd_B, nullptr, nullptr);
        if (connection_B == -1) {
            perror("Error al aceptar la conexión de A");
            continue;
        }

        while(true){

            // Realizar la comunicación con B
            // Puedes usar send y recv para enviar y recibir datos
            char buffer[1024];
            ssize_t bytesPeticion = recv(connection_B, buffer, sizeof(buffer), 0);
            string respuestaPeticion(buffer, bytesPeticion);

            cout << "RECIBI DE B: " << respuestaPeticion << endl;

            json j = json::parse(respuestaPeticion);

            Mensaje mensaje = j.get<Mensaje>();

            // istringstream ss(respuestaPeticion);
            string from, to, texto;
            // getline(ss, from, ',');  // Lee el valor "/desde" y lo almacena en 'from'
            // getline(ss, to, ',');  // Lee el valor "/hacia" y lo almacena en 'to'
            // getline(ss, mensaje);

            from = mensaje.origen;
            to = mensaje.destino;
            texto = mensaje.contexto.txtToSerarch;
            mensaje.contexto.ori = "BACKEND";
            

            buscarPalabra(mensaje, invertedIndex, texto);
            // mensaje.origen = 

            j = mensaje;

            // string mensajeEnviar = "";

            // cout << "OCURRENCIAS: ";
            // for (string ocurrencia : ocurrencias) {
            //     cout << ocurrencia << " ";
            //     mensajeEnviar += ocurrencia + ";"; // Agrega un ';' después de cada ocurrencia
            // }

            cout << "DEBERIA ENVIAR A B: " << j.dump(4).c_str() << endl; 
            send(connection_B, j.dump(4).c_str(), j.dump(4).length(), 0);

        }

        // Cerrar el socket de B
        close(sockfd_B);
    }
    return 0;
}