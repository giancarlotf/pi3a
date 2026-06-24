#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "PI3A";
const char *password = "OIZEASGTB";

ESP8266WebServer server(80); // Define o servidor na porta padrão

const char HTML_PAGINA[] PROGMEM = R"***(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Robô Agrícola</title>
</head>
<body>
    <form action="/enviar" method="GET">
        Fileiras [qtd]:<br>
        <input type="number" name="i" min="1" required oninput="document.getElementById('x_x').required = (this.value > 1);"><br>
        
        Comprimento das fileiras [m]:<br>
        <input type="number" name="y" min="0.1" step="0.01" required><br>
        
        Espaçamento entre fileiras [cm]:<br>
        <input type="number" name="x_x" id="x_x" min="1"><br>
        
        Largura máxima do plantio [m]:<br>
        <input type="number" name="x" min="0" step="0.01"><br>
        
        Espaçamento entre sementes [cm]:<br>
        <input type="number" name="n" min="1" required><br>
        
        Direção:<br>
        <select name="direcao">
            <option value="dir">Direita</option>
            <option value="esq">Esquerda</option>
        </select>
        <br><br>
        <button type="submit">Iniciar</button>
    </form>
</body>
</html>
)***";

void handleRoot()
{
  server.send(200, "text/html", HTML_PAGINA);
}

void handleFormulario()
{
  // Captura os dados do formulário HTTP
  int fileiras = server.arg("i").toInt();
  float comprimento = server.arg("y").toFloat();
  int esp_fileiras = server.arg("x_x").toInt();
  float largura_max = server.arg("x").toFloat();
  int esp_sementes = server.arg("n").toInt();
  String direcao = server.arg("direcao"); // "dir" ou "esq"

  // Monta uma string formatada para a Serial
  Serial.print(fileiras);
  Serial.print(",");
  Serial.print(comprimento, 2);
  Serial.print(",");
  Serial.print(esp_fileiras);
  Serial.print(",");
  Serial.print(largura_max, 2);
  Serial.print(",");
  Serial.print(esp_sementes);
  Serial.print(",");
  Serial.println(direcao); 

  // Responde no Display
  Serial.println("MSG:Iniciando...");

  // Responde no navegador
  String htmlResposta = "<h1>Dados enviados!</h1>";
  htmlResposta += "<p>Configuração carregada com sucesso.</p>";
  htmlResposta += "<a href='/'>Voltar</a>";
  server.send(200, "text/html", htmlResposta);
}

void setup()
{
  Serial.begin(9600);

  Serial.println("MSG:Configurando AP");
  WiFi.softAP(ssid, password);

  IPAddress esp_ip = WiFi.softAPIP();
  Serial.print("IP:");
  Serial.println(esp_ip);

  server.on("/", handleRoot);
  server.on("/enviar", handleFormulario);
  server.begin();

  Serial.println("MSG:HTTP Pronto!");
}

void loop()
{
  server.handleClient();
}
