#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

SoftwareSerial mySerial(10, 11);
DFRobotDFPlayerMini MP3;

const int Subir_Volumen = 6;
const int Bajar_Volumen = 5;
const int Siguiente = 4;
const int Regresar = 2;
const int Musica = 3;  

int volume = 30;  
int Primera_Musica = 1; 
bool Reproduccion = true;
bool lastReproduccion = true;  // Variable para detectar cambios en el estado de reproducción

const char song1[] PROGMEM = "1 Junto a Ti Maria";
const char song2[] PROGMEM = "2 La Nina de tus Ojos";
const char song3[] PROGMEM = "3 El Espiritu de Dios";
const char song4[] PROGMEM = "4 El Hombre sin Cristo";
const char song5[] PROGMEM = "5 Gracias Senor";
const char song6[] PROGMEM = "6 Joven";
const char song7[] PROGMEM = "7 Cumbia Virgen Maria";
const char song8[] PROGMEM = "8 Resucito";
const char song9[] PROGMEM = "9 Un Dia Mas";
const char song10[] PROGMEM = "10 Las Guerras";

const char* const nombresCanciones[] PROGMEM = { 
  song1, song2, song3, song4, song5, song6, song7, song8, song9, song10 
};

void setup() {
  pinMode(Subir_Volumen, INPUT_PULLUP);
  pinMode(Bajar_Volumen, INPUT_PULLUP);
  pinMode(Siguiente, INPUT_PULLUP);
  pinMode(Regresar, INPUT_PULLUP);
  pinMode(Musica, INPUT_PULLUP);

  mySerial.begin(9600);
  Serial.begin(9600);

  if (!MP3.begin(mySerial)) { 
    Serial.println(F("No se pudo conectar el DFPlayer Mini"));
    while(true);
  }

  MP3.volume(volume);
  MP3.play(Primera_Musica);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("No se pudo inicializar la pantalla OLED"));
    while(true);
  }

  display.display();
  delay(2000);
  display.clearDisplay();
  drawIcons(); // Dibuja los iconos en la pantalla
  displaySongName(); // Muestra el nombre de la canción
  displayVolume();   // Muestra el volumen sin parpadeo
}

void loop() {
  static int lastVolume = volume;

  // Controlar el volumen
  if (digitalRead(Subir_Volumen) == LOW) {
    if (volume < 30) {
      volume++;
      MP3.volume(volume);
      Serial.print("Volumen aumentado a: ");
      Serial.println(volume);
      delay(200);  
    }
  }

  if (digitalRead(Bajar_Volumen) == LOW) {
    if (volume > 0) {
      volume--;
      MP3.volume(volume);
      Serial.print("Volumen disminuido a: ");
      Serial.println(volume);
      delay(200);
    }
  }

  // Controlar la siguiente canción
  if (digitalRead(Siguiente) == LOW) {
    Primera_Musica++;
    if (Primera_Musica > 10) Primera_Musica = 1;  // Limitar a la cantidad de canciones
    MP3.play(Primera_Musica);
    delay(200);
    displaySongName(); // Actualiza el nombre de la canción
  }

  // Controlar la canción anterior
  if (digitalRead(Regresar) == LOW) {
    if (Primera_Musica > 1) {
      Primera_Musica--;
    } else {
      Primera_Musica = 10;  // Limitar a la cantidad de canciones
    }
    MP3.play(Primera_Musica);
    delay(200);
    displaySongName(); // Actualiza el nombre de la canción
  }

  // Controlar reproducción y pausa
  if (digitalRead(Musica) == LOW) {
    if (Reproduccion) {
      MP3.pause();  // Pausar la música
      Serial.println("Pausa");
    } else {
      MP3.start();  // Reanudar la música desde donde se pausó
      Serial.println("Reproducción");
    }
    Reproduccion = !Reproduccion;  // Cambiar el estado de la reproducción
    delay(200);  
  }

  // Actualiza solo el volumen si ha cambiado
  if (volume != lastVolume) {
    displayVolume(); // Solo actualizar la parte del volumen
    lastVolume = volume;
  }

  // Actualizar la pantalla si el estado de reproducción cambia
  if (Reproduccion != lastReproduccion) {
    lastReproduccion = Reproduccion;
    displaySongName(); // Actualiza el nombre de la canción
    drawIcons();       // Vuelve a dibujar los iconos para asegurar que no se borren
  }
}

void displaySongName() {
  // Limpiar solo la parte superior donde está el nombre de la canción
  display.fillRect(0, 0, SCREEN_WIDTH, 40, BLACK);
  display.setTextSize(1, 2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  int index = Primera_Musica - 1;
  if (index >= 0 && index < sizeof(nombresCanciones) / sizeof(nombresCanciones[0])) {
    char buffer[50];
    strcpy_P(buffer, (char*)pgm_read_word(&(nombresCanciones[index]))); // Leer desde PROGMEM
    display.print("----- Pista ____   ");
    display.println(buffer);
  } else {
    display.print("Pista: ");
    display.println("Desconocida");
  }

  display.display();
  drawIcons();  // Asegurarse de que los iconos se dibujen después de mostrar la canción
}

void displayVolume() {
  // Borra solo la parte del volumen
  display.fillRect(0, 50, 128, 14, BLACK);
  display.setTextSize(1, 2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 50);  // Posición del texto para el volumen
  display.print("Volumen: ");
  display.print(volume);
  display.display();
  drawIcons();  // Asegurarse de que los iconos se dibujen después de actualizar el volumen
}

void drawIcons() {
  // Tamaño de los iconos
  int iconSize = 10;
  int barWidth = 3;      // Ancho de las barras del icono de pausa
  int barSpacing = 1;    // Espacio entre las barras del icono de pausa
  int iconSpacing = 5;   // Espacio entre los iconos
  int additionalSpacing = 21;  // Espacio adicional para mover la flecha derecha

  // Coordenadas para los iconos en la esquina inferior derecha
  int xPrev = SCREEN_WIDTH - iconSize * 3 - iconSpacing * 2 - iconSpacing / 2;
  int yPrev = SCREEN_HEIGHT - iconSize - iconSpacing;
  int xPause = SCREEN_WIDTH - iconSize * 2 - iconSpacing / 2;
  int yPause = SCREEN_HEIGHT - iconSize - iconSpacing;
  int xNext = SCREEN_WIDTH - iconSize - iconSpacing * 2 - iconSpacing / 3 + additionalSpacing;
  int yNext = SCREEN_HEIGHT - iconSize - iconSpacing;

  // Icono de retroceso (flecha hacia la izquierda)
  display.fillTriangle(xPrev, yPrev + iconSize / 2, xPrev + iconSize, yPrev, xPrev + iconSize, yPrev + iconSize, WHITE);

  // Icono de pausa (dos barras verticales)
  display.fillRect(xPause - barWidth - barSpacing / 2, yPause, barWidth, iconSize, WHITE);  // Barra izquierda
  display.fillRect(xPause + barWidth + barSpacing / 2, yPause, barWidth, iconSize, WHITE);  // Barra derecha

  // Icono de siguiente (flecha hacia la derecha)
  display.fillTriangle(xNext - iconSize, yNext, xNext, yNext + iconSize / 2, xNext - iconSize, yNext + iconSize, WHITE);

  display.display();
}
