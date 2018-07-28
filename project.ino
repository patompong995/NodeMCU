#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>

#include <EspSoftwareSerial.h>

#include <math.h>



SoftwareSerial se_read(D5, D6); // write only

SoftwareSerial se_write(D0, D1); // read only

String const url = "http://ecourse.cpe.ku.ac.th/exceed/api/";



struct ProjectData {

  float TEMP_OVEN_1;
  float TEMP_OVEN_2;
  float TEMP_KITCHEN;
  float LPG;
  int32_t FAN_STATUS;
  int32_t OVEN_1;
  int32_t OVEN_2;
  int32_t WARNING;
  int32_t SWITCH_OVEN_1;
  int32_t SWITCH_OVEN_2;
} cur_project_data;



struct ServerData {
  //  float TEMP_OVEN_1;
  //  float TEMP_OVEN_2;
  //  float TEMP_KITCHEN;
  //  float LPG;
  //  int32_t FAN_STATUS;
  //  int32_t OVEN_1;
  //  int32_t OVEN_2;
  int32_t WARNING;
  int32_t SWITCH_OVEN_1;
  int32_t SWITCH_OVEN_2;
} server_data;



const char GET_SERVER_DATA = 1;

const char GET_SERVER_DATA_RESULT = 2;

const char UPDATE_PROJECT_DATA = 3;



// wifi configuration
const char SSID[] = "EXCEED_FRONT_LEFT_2.4GHz";
const char PASSWORD[] = "1234567890";



// for nodemcu communication

uint32_t last_sent_time = 0;

char expected_data_size = 0;

char cur_data_header = 0;

char buffer[256];

int8_t cur_buffer_length = -1;



void send_to_arduino(char code, void *data, char data_size) {

  char *b = (char*)data;

  int sent_size = 0;

  while (se_write.write(code) == 0) {

    delay(1);

  }

  while (sent_size < data_size) {

    sent_size += se_write.write(b, data_size);

    delay(1);

  }

}



void wifi_initialization() {

  Serial.println("WIFI INITIALIZING.");



  WiFi.begin(SSID, PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {

    yield();

    delay(10);

  }



  Serial.println("WIFI INITIALIZED.");

}



void serial_initialization() {

  Serial.begin(115200);

  se_read.begin(38400);

  se_write.begin(38400);



  while (!se_read.isListening()) {

    se_read.listen();

  }



  Serial.println();

  Serial.println("SERIAL INITIALIZED.");

}



String set_builder(const char *key, int32_t value) {

  String str = url;

  str = str + key;

  str = str + "/set?value=";

  str = str + value;

  Serial.println(str);

  return str;

}



String get_builder(const char *key) {

  String str = url;

  str = str + key;

  str = str + "/view/";

  return str;

}



void update_data_to_server_callback(String const &str) {

  Serial.println("update_data_to_server_callback FINISHED!");

}



bool GET(const char *url, void (*callback)(String const &str, int32_t &value), int32_t &value) {

  HTTPClient main_client;

  main_client.begin(url);

  if (main_client.GET() == HTTP_CODE_OK) {

    Serial.println("GET REQUEST RESPONSE BEGIN");


    if (callback != 0) {

      callback(main_client.getString(), value);

    }
    delay(200);
    return true;

  }

  Serial.println("GET REQUEST RESPONSE BEGIN");
  delay(200);
  return false;

}

bool GET(const char *url, void (*callback)(String const &str, float &value), float &value) {

  HTTPClient main_client;

  main_client.begin(url);

  if (main_client.GET() == HTTP_CODE_OK) {

    Serial.println("GET REQUEST RESPONSE BEGIN");

    if (callback != 0) {

      callback(main_client.getString(), value);

    }
    delay(200);
    return true;

  }

  Serial.println("GET REQUEST RESPONSE BEGIN");
  delay(200);
  return false;

}



bool POST(const char *url, void (*callback)(String const &str)) {

  HTTPClient main_client;

  main_client.begin(url);

  if (main_client.GET() == HTTP_CODE_OK) {

    Serial.println("POST REQUEST RESPONSE BEGIN");

    if (callback != 0) {

      callback(main_client.getString());

    }
    delay(500);
    return true;

  }

  Serial.println("POST REQUEST RESPONSE BEGIN");
  delay(500);
  return false;

}



int get_request_int(String const &str) {

  int32_t tmp = str.toInt();

  return tmp;

}



float get_request_float(String const &str) {

  float tmp = str.toFloat();

  return tmp;

}

void get_request(String const &str, int32_t &value) {

  value = get_request_int(str);

}

void get_request(String const &str, float &value) {

  value = get_request_float(str);

}



void get_request_raw_callback(String const &str) {

  Serial.println("GET REQUEST RESPONSE BEGIN");

  Serial.println("========================");

  Serial.println(str.c_str());

  Serial.println("========================");

  Serial.println("GET REQUEST RESPONSE END");

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void setup() {

  serial_initialization();

  wifi_initialization();



  Serial.print("sizeof(ServerData): ");

  Serial.println((int)sizeof(ServerData));

  Serial.print("ESP READY!");

}
float TEMP_OVEN_1 = 0;
float TEMP_OVEN_2 = 0;
float TEMP_KITCHEN = 0;
float LPG = 0;
int32_t FAN_STATUS = 0;
int32_t OVEN_1 = 1;
int32_t OVEN_2 = 1;
int32_t SWITCH_OVEN_1 = 1;
int32_t SWITCH_OVEN_2 = 1;
void loop() {
  delay(1000);
  /*
  uint32_t cur_time = millis();
  if (cur_time - last_sent_time > 1000) {
    GET(get_builder("terngpalm-WARNING").c_str(), get_request, server_data.WARNING);
    GET(get_builder("terngpalm-SWITCH_OVEN_1").c_str(), get_request, server_data.SWITCH_OVEN_1);
    GET(get_builder("terngpalm-SWITCH_OVEN_2").c_str(), get_request, server_data.SWITCH_OVEN_2);
    Serial.print("WARNNING : ");
    Serial.println(server_data.WARNING);
    Serial.print("SWITCH_OVEN_1: ");
    Serial.println(server_data.SWITCH_OVEN_1);
    Serial.print("SWITCH_OVEN_2: ");
    Serial.println(server_data.SWITCH_OVEN_2);
    last_sent_time = cur_time;
  }
  */
  while (se_read.available()) {
    char ch = se_read.read();
    if (cur_buffer_length == -1) {
      cur_data_header = ch;
      switch (cur_data_header) {
        case UPDATE_PROJECT_DATA:
          expected_data_size = sizeof(ProjectData);
          cur_buffer_length = 0;
          break;
        case GET_SERVER_DATA:
          expected_data_size = sizeof(ServerData);
          cur_buffer_length = 0;
          break;
      }
    } else if (cur_buffer_length < expected_data_size) {
      buffer[cur_buffer_length++] = ch;
      if (cur_buffer_length == expected_data_size) {
        switch (cur_data_header) {
          case UPDATE_PROJECT_DATA: {
              ProjectData *project_data = (ProjectData*)buffer;
              TEMP_OVEN_1 = project_data->TEMP_OVEN_1;
              TEMP_OVEN_2 = project_data->TEMP_OVEN_2;
              TEMP_KITCHEN = project_data->TEMP_KITCHEN;
              LPG = project_data->LPG;
              FAN_STATUS = project_data->FAN_STATUS;
              OVEN_1 = project_data->OVEN_1;
              OVEN_2 = project_data->OVEN_2;
              SWITCH_OVEN_1 = project_data->SWITCH_OVEN_1;
              SWITCH_OVEN_2 = project_data->SWITCH_OVEN_2;
              POST(set_builder("terngpalm-TEMP_OVEN_1", TEMP_OVEN_1).c_str(), update_data_to_server_callback);delay(2);
              POST(set_builder("terngpalm-TEMP_OVEN_2", TEMP_OVEN_2).c_str(), update_data_to_server_callback);delay(2);
              POST(set_builder("terngpalm-TEMP_KITCHEN", TEMP_KITCHEN).c_str(), update_data_to_server_callback);delay(2);
              POST(set_builder("terngpalm-LPG", LPG).c_str(), update_data_to_server_callback);delay(2);
              POST(set_builder("terngpalm-FAN_STATUS", FAN_STATUS).c_str(), update_data_to_server_callback);delay(2);
              POST(set_builder("terngpalm-OVEN_1", OVEN_1).c_str(), update_data_to_server_callback);delay(2);
              POST(set_builder("terngpalm-OVEN_2", OVEN_2).c_str(), update_data_to_server_callback);delay(2);
              break;
            }
            break;
          case GET_SERVER_DATA:
            GET(get_builder("terngpalm-WARNING").c_str(), get_request, server_data.WARNING);
            GET(get_builder("terngpalm-SWITCH_OVEN_1").c_str(), get_request, server_data.SWITCH_OVEN_1);
            GET(get_builder("terngpalm-SWITCH_OVEN_2").c_str(), get_request, server_data.SWITCH_OVEN_2);
            delay(200);
            send_to_arduino(GET_SERVER_DATA_RESULT, &server_data, sizeof(ServerData));
            Serial.print("WARNNING : ");
            Serial.println(server_data.WARNING);
            Serial.print("SWITCH_OVEN_1: ");
            Serial.println(server_data.SWITCH_OVEN_1);
            Serial.print("SWITCH_OVEN_2: ");
            Serial.println(server_data.SWITCH_OVEN_2);
            Serial.println("Send data to arduino");
            break;
        }
        cur_buffer_length = -1;
      }
    }
  }
}
