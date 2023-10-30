#include <winsock2.h>
#include <windows.h>  // Otros encabezados de Windows si son necesarios
#include <stdio.h>

int createGame();
int joinGameFivePlayers();
const char* createGameFivePlayers();
char* findJsonValue(const char* jsonString, const char* key);
void fiftyGames();
void fivePlayers();
#pragma comment (lib, "Ws2_32.lib")

int main(void) {
    fivePlayers();
    //fiftyGames();
    return 0;
}

char* findJsonValue(const char* jsonString, const char* key) {
    printf("hola\n");
    const char* start = strstr(jsonString, key);
    if (!start) {
        return NULL; // La clave no se encontró en la cadena JSON
    }
    start = strstr(start, ":");
    if (!start) {
        return NULL; // Error en el formato del JSON
    }

    start = strchr(start, '"');
    if (!start) {
        return NULL; // Error en el formato del JSON
    }

    start++; // Avanzamos para omitir el primer '"' de la cadena de valor

    const char* end = strchr(start, '"');
    if (!end) {
        return NULL; // Error en el formato del JSON
    }

    // Calculamos la longitud del valor
    size_t length = end - start;

    char* value = (char*)malloc(length + 1); // +1 para el carácter nulo al final
    if (!value) {
        return NULL; // Error al asignar memoria
    }

    // Copiamos el valor a la cadena resultante
    strncpy(value, start, length);
    value[length] = '\0'; // Agregamos el carácter nulo al final

    return value;
}


int joinGameFivePlayers(char* player, char* id) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Error al inicializar WinSock\n");
        return 1;
    }

    // Crear un socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "Error al crear el socket\n");
        WSACleanup();
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80); // Puerto de destino
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del servidor (localhost)

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "Error al conectar al servidor\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Construir el cuerpo JSON de la solicitud con el ID y el nombre del jugador
    char requestBody[256];
    snprintf(requestBody, sizeof(requestBody), "{\"playerName\":\"%s\"}", player);

    // Construir la solicitud PUT
    const char* request = "PUT /api/games/%s HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n"
        "gameId: %s\r\n"
        "password: Shazam!\r\n"
        "player: %s\r\n\r\n"
        "%s";

    int requestBodyLength = strlen(requestBody);
    char requestBuffer[2048]; // Aumenta el tamaño si es necesario

    snprintf(requestBuffer, sizeof(requestBuffer), request, id, requestBodyLength, id, player, requestBody);

    // Enviar la solicitud PUT al servidor
    send(sock, requestBuffer, strlen(requestBuffer), 0);

    // Recibir y mostrar la respuesta del servidor
    char responseBuffer[1024];
    int bytesRead = recv(sock, responseBuffer, sizeof(responseBuffer), 0);
    if (bytesRead > 0) {
        responseBuffer[bytesRead] = '\0';
        printf("Respuesta del servidor:\n%s\n", responseBuffer);
    }
    else {
        fprintf(stderr, "No se recibió respuesta del servidor\n");
    }

    // Cerrar el socket y liberar recursos
    closesocket(sock);
    WSACleanup();

    return 0;
}

int joinPlayer(char* id) {
    char* arr[4] = { "aaa", "bbb", "ccc", "ddd" };
    for (int i = 0; i < 4; i++) {
        joinGameFivePlayers(arr[i], id);
    }
}

int createGame(char* cadena) {
    // Inicializar WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Error al inicializar WinSock\n");
        return 1;
    }

    // Crear un socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "Error al crear el socket\n");
        WSACleanup();
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80); // Puerto de destino
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del servidor (localhost)

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "Error al conectar al servidor\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Crear una cadena JSON utilizando los valores generados
    int jsonBufferSize = snprintf(NULL, 0, "{\"name\":\"%s\",\"owner\":\"%s\",\"password\":\"Shazam!\"}", cadena, cadena) + 1;
    char* jsonCadena = (char*)malloc(jsonBufferSize);

    if (jsonCadena == NULL) {
        fprintf(stderr, "Error al asignar memoria para la cadena JSON en la iteración.\n");
        free(cadena); // Liberar la memoria anteriormente asignada
        return 1;
    }

    snprintf(jsonCadena, jsonBufferSize, "{\"name\":\"%s\",\"owner\":\"%s\",\"password\":\"Shazam!\"}", cadena, cadena);

    // Construir la solicitud POST
    const char* request = "POST /api/games HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n\r\n"
        "%s";
    int postDataLength = strlen(jsonCadena);
    char requestBuffer[1024];
    snprintf(requestBuffer, sizeof(requestBuffer), request, postDataLength, jsonCadena);

    // Enviar la solicitud POST al servidor
    send(sock, requestBuffer, strlen(requestBuffer), 0);

    // Recibir y mostrar la respuesta del servidor
    char responseBuffer[1024];
    int bytesRead = recv(sock, responseBuffer, sizeof(responseBuffer), 0);
    if (bytesRead > 0) {
        responseBuffer[bytesRead] = '\0';
        printf("Respuesta del servidor:\n%s\n", responseBuffer);
    }
    else {
        fprintf(stderr, "No se recibió respuesta del servidor\n");
    }

    // Cerrar el socket y liberar recursos
    closesocket(sock);
    WSACleanup();

    return 0;
}

const char* createGameFivePlayers() {
    // Inicializar WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "Error al inicializar WinSock\n");
        return 1;
    }

    // Crear un socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        fprintf(stderr, "Error al crear el socket\n");
        WSACleanup();
        return 1;
    }

    // Configurar la dirección del servidor
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(80); // Puerto de destino
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP del servidor (localhost)

    // Conectar al servidor
    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        fprintf(stderr, "Error al conectar al servidor\n");
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Datos a enviar en la solicitud POST
    const char* postData = "{\"name\":\"Epsilon Centauri\",\"owner\":\"Thanos\",\"password\":\"Shazam!\"}";

    // Construir la solicitud POST
    const char* request = "POST /api/games HTTP/1.1\r\n"
        "Host: localhost\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: %d\r\n\r\n"
        "%s";
    int postDataLength = strlen(postData);
    char requestBuffer[1024];
    snprintf(requestBuffer, sizeof(requestBuffer), request, postDataLength, postData);

    // Enviar la solicitud POST al servidor
    send(sock, requestBuffer, strlen(requestBuffer), 0);

    // Recibir y mostrar la respuesta del servidor
    char responseBuffer[1024];
    int bytesRead = recv(sock, responseBuffer, sizeof(responseBuffer), 0);
    if (bytesRead > 0) {
        responseBuffer[bytesRead] = '\0';

        //printf("Respuesta del servidor:\n%s\n", responseBuffer);
    }
    else {
        fprintf(stderr, "No se recibió respuesta del servidor\n");
    }
    char* responseBuffer2 = (char*)malloc(sizeof(char) * (bytesRead + 1));
    if (responseBuffer2 == NULL) {
        fprintf(stderr, "Error al asignar memoria para la respuesta\n");
        return NULL;
    }
    strcpy(responseBuffer2, responseBuffer);
    // Cerrar el socket y liberar recursos
    closesocket(sock);
    WSACleanup();

    return responseBuffer2;
}

void fiftyGames() {
    for (int i = 1; i <= 50; i++) {
        // Calcular el tamaño de la cadena resultante
        int bufferSize = snprintf(NULL, 0, "Ejemplo%d", i) + 1;
        char* cadena = (char*)malloc(bufferSize);

        if (cadena == NULL) {
            fprintf(stderr, "Error al asignar memoria para la cadena en la iteración %d.\n", i);
            return;
        }

        // Generar la cadena
        snprintf(cadena, bufferSize, "Ejemplo%d", i);

        // Crear los juegos
        createGame(cadena);

        // Liberar la memoria de la cadena
        free(cadena);
    }
}

void fivePlayers() {
    const char* response = createGameFivePlayers();
    char* gameId = findJsonValue(response, "\"id\"");
    joinPlayer(gameId);
}