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

    if (!file) {
        cerr << "No se pudo abrir el archivo .idx" << endl;
        exit(1);
    }

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

void buscarPalabra(Mensaje& mensaje, const std::map<std::string, std::vector<std::pair<std::string, int>>>& invertedIndexMap, const std::string& texto, int topk) {
    // Tokenizar el texto en palabras
    std::istringstream palabrasStream(texto);
    std::vector<std::string> palabrasABuscar;
    std::string palabraBuscada;
    while (palabrasStream >> palabraBuscada) {
        palabrasABuscar.push_back(palabraBuscada);
    }

    // for (string palabra: palabrasABuscar){
    //     cout << palabra << endl;
    // }
    vector<std::string> palabrasABuscar2 = palabrasABuscar;
    // Verificar si todas las palabras están presentes
    bool todasLasPalabrasPresentes = true;

    // Crear un conjunto de archivos para la primera palabra como punto de referencia
    std::set<std::string> archivosEnReferencia;

    while (!palabrasABuscar.empty()) {
        const auto& primerPalabra = palabrasABuscar.front();

        cout << "buscando: " << primerPalabra << endl;
        auto it = invertedIndexMap.find(primerPalabra);

        if (it != invertedIndexMap.end()) {
            for (const auto& [archivo, _] : it->second) {
                cout << archivo << endl;
                archivosEnReferencia.insert(archivo);
            }
        } else {
            todasLasPalabrasPresentes = false;
            break;
        }
        palabrasABuscar.erase(palabrasABuscar.begin());
    }

    // Realiza la búsqueda y conteo de palabras solo si todas las palabras están presentes

    for(string palabra: palabrasABuscar2){
        cout << palabra << endl;
    }

    if (todasLasPalabrasPresentes) {
        std::map<std::string, int> apariciones;

        for (const auto& palabra : palabrasABuscar2) {
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
        int i = 1;
        for (const auto& archivo : archivosEnReferencia) {
            int totalApariciones = 0;

            for (const auto& palabra : palabrasABuscar2) {
                const auto& listaApariciones = invertedIndexMap.at(palabra);
                for (const auto& [arch, cantidad] : listaApariciones) {
                    if (arch == archivo) {
                        totalApariciones += cantidad;
                    }
                }
            }
            if(i <= topk){
                cout << "i: " << i << endl;
                aparicionesOrdenadas.push_back({archivo, totalApariciones});
                i++;
            }
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

void cargarVariablesEnv() {
    const char* envFile = "backend.env";
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
    string file = getenv("FILE");
    int topk = stoi(getenv("TOPK"));
    // cout << topk;

    if(from == to){
        cout << "FROM == TO" << endl;
        return 1;
    }

    map<string, vector<pair<string, int>>>invertedIndex = cargarIndex(file);

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
            char buffer[1024];
            ssize_t bytesPeticion = recv(connection_B, buffer, sizeof(buffer), 0);
            string respuestaPeticion(buffer, bytesPeticion);

            cout << "RECIBI DE B: " << respuestaPeticion << endl;

            json j = json::parse(respuestaPeticion);

            Mensaje mensaje = j.get<Mensaje>();

            string from, to, texto;

            from = mensaje.origen;
            to = mensaje.destino;
            texto = mensaje.contexto.txtToSerarch;
            mensaje.contexto.ori = "BACKEND";
            buscarPalabra(mensaje, invertedIndex, texto, topk);

            j = mensaje;

            cout << "DEBERIA ENVIAR A B: " << j.dump(4).c_str() << endl; 
            send(connection_B, j.dump(4).c_str(), j.dump(4).length(), 0);

        }

        // Cerrar el socket de B
        close(sockfd_B);
    }
    return 0;
}