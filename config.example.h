#ifndef CONFIG_H
#define CONFIG_H

// ================================================================================
// M5StickCPlus2 センサーモニター 設定ファイル例
// ================================================================================
// このファイルは、M5StickCPlus2 センサーモニターの基本的な設定を示しています。
// 実際のプロジェクトで使用する際は、このファイルを「config.h」としてコピーし、
// YOUR_XXX と記述されている部分をあなたの環境に合わせて書き換えてください。
// 「config.h」はGitで追跡されないように設定されているため、個人情報が公開される心配はありません。
// ================================================================================

// ========== ネットワーク設定 ==========
// 接続したいWi-FiネットワークのSSID（名前）とパスワードを設定します。
// 例: "MyHomeWiFi", "MyWifiPassword123"
const char* WIFI_NETWORK_NAME = "YOUR_WIFI_SSID";     // 接続するWiFiのSSID（ネットワーク名）
const char* WIFI_NETWORK_PASSWORD = "YOUR_WIFI_PASSWORD";  // WiFiのパスワード

// ========== MQTT設定 ==========
// センサーデータを受信するためのMQTTブローカー（サーバー）情報を設定します。
// 例: "192.168.1.100", "sensor_data", 1883
const char* MQTT_BROKER_ADDRESS = "YOUR_MQTT_BROKER_IP";      // MQTTブローカ（サーバ）のIPアドレス
const char* MQTT_TOPIC_NAME = "sensor_data";           // 購読するトピック名（データのカテゴリ）
const int MQTT_BROKER_PORT = 1883;                     // MQTTブローカのポート番号（標準は1883）
const char* MQTT_CLIENT_ID_PREFIX = "M5StickCPlus2-";  // MQTT接続時のクライアントID接頭辞。重複を避けるため、M5StickCPlus2のMACアドレスなどを利用するのも良いでしょう。

// ========== 時刻同期設定 ==========
// 正確な現在時刻を取得するためのNTP（Network Time Protocol）サーバー情報を設定します。
// 通常、"pool.ntp.org" で問題ありません。日本時間へのオフセットは秒単位で指定します。
const char* TIME_SERVER_ADDRESS = "pool.ntp.org";               // NTPサーバのアドレス
const long JAPAN_TIME_OFFSET_SECONDS = 32400;                   // 日本時間のオフセット（+9時間を秒換算: 9 * 60 * 60 = 32400）
const unsigned long TIME_UPDATE_INTERVAL_MILLISECONDS = 60000;  // 時刻を更新する間隔（ミリ秒）。例: 60000 = 1分

// ========== 表示更新設定 ==========
// マイコンのメインループの処理間隔や、画面表示の更新に関わる設定です。
const unsigned long MAIN_LOOP_DELAY_MILLISECONDS = 100;           // メインループの待機時間（ミリ秒）。これによりCPUの負荷を軽減します。

// ========== 画面表示位置の設定 ==========
// M5StickCPlus2のディスプレイ（240x135ピクセル）上での各要素の表示位置を調整します。
// UI/UXの改善により、数値が大きく表示されるように座標を調整しています。

// 画面全体の垂直方向の微調整。全体の表示を上下に動かしたい場合に調整してください。
const int VERTICAL_OFFSET = -5; 

// 画面上部のタイトル、現在時刻、接続ステータスの表示位置
const int TITLE_POSITION_X = 5;      
const int TITLE_POSITION_Y = 2 + VERTICAL_OFFSET;       
const int TIME_DISPLAY_X = 140;       
const int TIME_DISPLAY_Y = 2 + VERTICAL_OFFSET;         
const int CONNECTION_STATUS_X = 190;  
const int CONNECTION_STATUS_Y = 2 + VERTICAL_OFFSET;    

// CO2値とTHI値の大きな数値表示の共通位置
// 数値が非常に大きいため、X座標は画面中央に近くなるように調整されています。
const int LARGE_VALUE_X = 15;           // 大きな数値のX座標
const int LARGE_VALUE_Y = 40 + VERTICAL_OFFSET; // 大きな数値のY座標

// CO2/THIラベル（例: "CO2:", "THI:"）の表示位置。数値の少し上に配置されます。
const int LARGE_LABEL_X = 15;
const int LARGE_LABEL_Y = 20 + VERTICAL_OFFSET;

// データがない場合やエラーメッセージの表示位置
const int NO_DATA_MESSAGE_X = 40;     
const int NO_DATA_MESSAGE_Y = 55 + VERTICAL_OFFSET;     


// ========== 再試行・タイムアウト設定 ==========
// ネットワーク接続やデータ取得が失敗した際の再試行回数や待機時間を設定します。
const int MAXIMUM_NTP_RETRY_ATTEMPTS = 10;                        // NTP同期の最大試行回数
const unsigned long MQTT_RECONNECTION_DELAY_MILLISECONDS = 5000;  // MQTT再接続を試みるまでの待機時間（ミリ秒）
const unsigned long CONNECTION_SUCCESS_DISPLAY_TIME = 2000;       // 接続成功メッセージを画面に表示する時間（ミリ秒）

// ========== JSON解析設定 ==========
// 受信したセンサーデータをJSON形式で解析するために必要なメモリサイズを設定します。
// 受信するJSONデータの構造やサイズに合わせて調整してください。
const size_t JSON_PARSING_MEMORY_SIZE = 2048;  // JSON解析用メモリサイズ（バイト）

// ========== 交互表示のための設定 ==========
// CO2値とTHI値を交互に表示する際の間隔を設定します。
const unsigned long INTERACTIVE_DISPLAY_INTERVAL_MILLISECONDS = 3000; // 3秒ごとに交互表示（ミリ秒）

#endif // CONFIG_H