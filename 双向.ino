#include <ESP8266WiFi.h>
#include <espnow.h>

uint8_t esp_HAVE_address[] = {0xAC, 0x0B, 0xFB, 0xF7, 0x7E, 0xCA}; // esp01s的mac地址（有高温胶布）
uint8_t esp_NO_address[] = {0x50, 0x02, 0x91, 0xFD, 0x06, 0x87}; // esp01s的mac地址（没有高温胶布）

// 接收信息时的回调函数
uint8_t data_rc[50];
void OnDataRecv(uint8_t *mac, uint8_t *incomingData, uint8_t len) {
    memcpy(&data_rc, incomingData, sizeof(data_rc));
    for (int i = 0; i < len; i++) {
        Serial.write(data_rc[i]);
    }
}

void setup() {
    Serial.begin(115200); // 初始化串口，设置其波特率为115200
    WiFi.mode(WIFI_STA); // 设置WIFI模式为STA
    WiFi.disconnect(); // 断开WIFI连接
  
    // Init ESP-NOW
    if (esp_now_init() != 0) {
        return; // 初始化失败，退出
    }
  
    // 设置角色为 ESP_NOW_ROLE_COMBO
    esp_now_set_self_role(ESP_NOW_ROLE_COMBO);
  
    // 添加对端设备
    if (esp_now_add_peer(esp_HAVE_address, ESP_NOW_ROLE_COMBO, 1, NULL, 0) != 0) {
        return; // 添加对端失败，退出
    }
    if (esp_now_add_peer(esp_NO_address, ESP_NOW_ROLE_COMBO, 1, NULL, 0) != 0) {
        return; // 添加对端失败，退出
    }
  
    // 注册回调函数
    esp_now_register_recv_cb(OnDataRecv);
}

uint8_t data[100];
uint8_t data_now[100];
int i = 0;
int data_long = 0;

void loop() {
    if (Serial.available() > 0) {
        data[i] = Serial.read();
        if (data[0] != 0x81) {
            i = 0;
        } else {
            i++;
            if (i > 1 && data[i - 1] == 0x7E) {
                data_long = i - 2;
                
                for (int a = 1; a < data_long + 1; a++) {
                    data_now[a - 1] = data[a];
                    if (a == data_long) {
                        i = 0;
                    }
                }
                
                // 发送数据到 esp_HAVE_address 和 esp_NO_address
                esp_now_send(esp_HAVE_address, data_now, data_long);
                esp_now_send(esp_NO_address, data_now, data_long);
            }
        }
    }
}
