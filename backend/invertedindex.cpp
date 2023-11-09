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

#include <set>
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

std::map<std::string, std::vector<std::pair<std::string, int>>> cargarIndex(const std::string& filename) {
    std::map<std::string, std::vector<std::pair<std::string, int>>> result;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        std::string key = line.substr(0, line.find(":"));
        std::string value = line.substr(line.find(":") + 1);

        std::vector<std::pair<std::string, int>> files;
        size_t start = 0;
        size_t end = 0;

        // Verifica si hay al menos un archivo en la línea
        if (value.find(";") != std::string::npos) {
            while ((end = value.find(");", start)) != std::string::npos) {
                std::string file = value.substr(start + 1, end - start);
                size_t semicolon = file.find(";");
                std::string filename = file.substr(0, semicolon);
                int number = std::stoi(file.substr(semicolon + 1));
                files.push_back(std::make_pair(filename, number));
                start = end + 2;
            }
        }

        // Procesa el último valor sin necesidad de ;)
        if (start < value.size()) {
            std::string file = value.substr(start + 1, value.size() - start - 2);
            size_t semicolon = file.find(";");
            std::string filename = file.substr(0, semicolon);
            int number = std::stoi(file.substr(semicolon + 1));
            files.push_back(std::make_pair(filename, number));
        }

        result[key] = files;
    }

    file.close();
/*
    for (const auto& [key, val] : result) {
        std::cout << key << ": ";
        for (const auto& [filename, number] : val) {
            std::cout << "(" << filename << ", " << number << ") ";
        }
        std::cout << std::endl;
    }*/

    return result;
}
    // Imprime el contenido del mapa

void buscarPalabra(Mensaje& mensaje, const std::map<std::string, std::vector<std::pair<std::string, int>>>& invertedIndexMap, const std::string& texto) {
    // Tokenizar el texto en palabras
    std::istringstream palabrasStream(texto);
    std::vector<std::string> palabrasABuscar;
    std::string palabraBuscada;
    while (palabrasStream >> palabraBuscada) {
        palabrasABuscar.push_back(palabraBuscada);
    }

    // Verificar si todas las palabras están presentes
    bool todasLasPalabrasPresentes = true;

    // Crear un conjunto de archivos para la primera palabra como punto de referencia
    std::set<std::string> archivosEnReferencia;

    if (!palabrasABuscar.empty()) {
        const auto& primerPalabra = palabrasABuscar.front();
        auto it = invertedIndexMap.find(primerPalabra);

        if (it != invertedIndexMap.end()) {
            for (const auto& [archivo, _] : it->second) {
                archivosEnReferencia.insert(archivo);
            }
        } else {
            todasLasPalabrasPresentes = false;
        }
    } else {
        todasLasPalabrasPresentes = false;
    }

    // Realiza la búsqueda y conteo de palabras solo si todas las palabras están presentes
    if (todasLasPalabrasPresentes) {
        std::map<std::string, int> apariciones;

        for (const auto& palabra : palabrasABuscar) {
            const auto& listaApariciones = invertedIndexMap.at(palabra);

            // Crear un conjunto de archivos para la palabra actual
            std::set<std::string> archivosParaPalabra;

            for (const auto& [archivo, _] : listaApariciones) {
                archivosParaPalabra.insert(archivo);
            }

            // Intersección con el conjunto de archivos de referencia
            std::set<std::string> interseccion;
            std::set_intersection(
                archivosEnReferencia.begin(), archivosEnReferencia.end(),
                archivosParaPalabra.begin(), archivosParaPalabra.end(),
                std::inserter(interseccion, interseccion.begin())
            );

            // Actualizar el conjunto de archivos de referencia con la intersección
            archivosEnReferencia = interseccion;
        }

        // Actualiza el contexto del mensaje
        mensaje.contexto.isFound = !archivosEnReferencia.empty();
        mensaje.contexto.resultados.clear();

        // Ordena las apariciones de mayor a menor antes de agregarlas al contexto
        std::vector<std::pair<std::string, int>> aparicionesOrdenadas;
        for (const auto& archivo : archivosEnReferencia) {
            int totalApariciones = 0;

            for (const auto& palabra : palabrasABuscar) {
                const auto& listaApariciones = invertedIndexMap.at(palabra);
                for (const auto& [arch, cantidad] : listaApariciones) {
                    if (arch == archivo) {
                        totalApariciones += cantidad;
                    }
                }
            }

            aparicionesOrdenadas.push_back({archivo, totalApariciones});
        }

        std::sort(aparicionesOrdenadas.begin(), aparicionesOrdenadas.end(),
                [](const auto& a, const auto& b) {
                    return a.second > b.second;
                });

        mensaje.contexto.resultados = aparicionesOrdenadas;
    } else {
        // Si alguna palabra no está en el índice, o no hay palabras, no hace nada
        mensaje.contexto.isFound = false;
        mensaje.contexto.resultados.clear();
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