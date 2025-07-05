#ifndef CONFIG_H
#define CONFIG_H

// ================================================================================
// M5StickCPlus2 センサーモニター 設定ファイル例
// このファイルをコピーして config.h として保存し、YOUR_XXX 部分を書き換えてください。
// ================================================================================

// ========== ネットワーク設定 ==========
const char* WIFI_NETWORK_NAME = "YOUR_WIFI_SSID";     // 接続するWiFiのSSID（ネットワーク名）
const char* WIFI_NETWORK_PASSWORD = "YOUR_WIFI_PASSWORD";  // WiFiのパスワード

// ========== MQTT設定 ==========
const char* MQTT_BROKER_ADDRESS = "YOUR_MQTT_BROKER_IP";      // MQTTブローカ（サーバ）のIPアドレス
const char* MQTT_TOPIC_NAME = "sensor_data";           // 購読するトピック名（データのカテゴリ）
const int MQTT_BROKER_PORT = 1883;                     // MQTTブローカのポート番号（標準は1883）
const char* MQTT_CLIENT_ID_PREFIX = "M5StickCPlus2-";  // MQTT接続時のクライアントID接頭辞

// ========== 時刻同期設定 ==========
const char* TIME_SERVER_ADDRESS = "pool.ntp.org";               // NTPサーバのアドレス
const long JAPAN_TIME_OFFSET_SECONDS = 32400;                   // 日本時間のオフセット（+9時間を秒換算）
const unsigned long TIME_UPDATE_INTERVAL_MILLISECONDS = 60000;  // 時刻更新間隔（1分）

// ========== 表示更新設定 ==========
const unsigned long DISPLAY_UPDATE_INTERVAL_MILLISECONDS = 5000;  // 画面更新間隔（5秒）
const unsigned long MAIN_LOOP_DELAY_MILLISECONDS = 100;           // メインループの待機時間

// ========== 画面表示位置の設定 ==========
// 画面サイズ：240x135 ピクセル
const int VERTICAL_OFFSET = -5; 
const int TITLE_POSITION_X = 5;      
const int TITLE_POSITION_Y = 2 + VERTICAL_OFFSET;       
const int TIME_DISPLAY_X = 140;       
const int TIME_DISPLAY_Y = 2 + VERTICAL_OFFSET;         
const int CONNECTION_STATUS_X = 190;  
const int CONNECTION_STATUS_Y = 2 + VERTICAL_OFFSET;    
const int CO2_LABEL_X = 5;           
const int CO2_LABEL_Y = 25 + VERTICAL_OFFSET;           
const int CO2_VALUE_X = 5;           
const int CO2_VALUE_Y = 45 + VERTICAL_OFFSET;           
const int THI_LABEL_X = 5;           
const int THI_LABEL_Y = 85 + VERTICAL_OFFSET;           
const int THI_VALUE_X = 5;           
const int THI_VALUE_Y = 105 + VERTICAL_OFFSET;          
const int NO_DATA_MESSAGE_X = 40;     
const int NO_DATA_MESSAGE_Y = 55 + VERTICAL_OFFSET;     


// ========== 再試行・タイムアウト設定 ==========
const int MAXIMUM_NTP_RETRY_ATTEMPTS = 10;                        // NTP同期の最大試行回数
const unsigned long MQTT_RECONNECTION_DELAY_MILLISECONDS = 5000;  // MQTT再接続待機時間（5秒）
const unsigned long CONNECTION_SUCCESS_DISPLAY_TIME = 2000;       // 接続成功メッセージ表示時間（2秒）

// ========== JSON解析設定 ==========
const size_t JSON_PARSING_MEMORY_SIZE = 2048;  // JSON解析用メモリサイズ（バイト）

#endif // CONFIG_H
