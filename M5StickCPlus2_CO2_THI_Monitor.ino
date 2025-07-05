/*
 * ================================================================================
 * M5StickCPlus2 MQTT センサーモニター（初学者向け詳細解説版）
 * ================================================================================
 * * このプログラムの目的：
 * - WiFiネットワークに接続する
 * - NTPサーバから正確な時刻を取得する
 * - MQTTブローカからセンサーデータ（CO2とTHI）を受信する
 * - 受信したデータをM5StickCPlus2の画面に表示する
 * - 5秒毎に表示を更新する
 * * 必要なライブラリ：
 * - M5StickCPlus2：M5Stack製品の制御
 * - WiFi：WiFi接続機能
 * - PubSubClient：MQTT通信機能
 * - ArduinoJson：JSON形式データの解析
 * - NTPClient：時刻同期機能
 * * 作成日: 2025年5月29日
 * 最終更新日: 2025年7月5日 (Apple社チーフデザイナーによるUI/UX改善版)
 * ================================================================================
 */

// ========== 必要なライブラリを読み込み ==========
#include <M5StickCPlus2.h>  // M5StickCPlus2の制御ライブラリ
#include <WiFi.h>           // WiFi接続ライブラリ
#include <PubSubClient.h>   // MQTT通信ライブラリ
#include <ArduinoJson.h>    // JSON解析ライブラリ
#include <NTPClient.h>      // 時刻同期ライブラリ
#include <WiFiUdp.h>        // NTP通信に必要
#include "config.h"         // 新しく作成した設定ファイルを読み込み！

// ========== データ構造体の定義 ==========
/*
 * センサーから受信するデータをまとめて管理するための構造体
 * 構造体：関連するデータを一つのまとまりとして扱うC++の機能
 */
struct SensorDataPacket {
  int carbonDioxideLevel;          // CO2濃度（整数値、ppm単位）
  float thermalComfortIndex;       // THI（温熱指標、小数点第1位まで）
  float ambientTemperature;        // 環境温度（摂氏）
  float relativeHumidity;          // 相対湿度（%）
  String comfortLevelDescription;  // 快適度レベル（日本語文字列）
  unsigned long dataTimestamp;     // データのタイムスタンプ（UNIX時間）
  bool hasValidData;               // データが有効かどうかのフラグ
};

// ========== グローバル変数（プログラム全体で使用する変数） ==========
/*
 * グローバル変数：プログラムのどこからでもアクセスできる変数
 * 通常は最小限に抑えるが、組み込みシステムでは必要に応じて使用
 */

// ネットワーク通信用のクライアントオブジェクト
WiFiUDP networkUdpClient;               // UDP通信クライアント（NTP用）
NTPClient timeClient(networkUdpClient,  // 時刻同期クライアント
                     TIME_SERVER_ADDRESS,
                     JAPAN_TIME_OFFSET_SECONDS,
                     TIME_UPDATE_INTERVAL_MILLISECONDS);
WiFiClient networkWifiClient;                             // WiFi通信クライアント（MQTT用）
PubSubClient mqttCommunicationClient(networkWifiClient);  // MQTT通信クライアント

// センサーデータ保存用変数
SensorDataPacket currentSensorReading = {
  // 現在のセンサーデータ
  0,     // CO2初期値
  0.0,   // THI初期値
  0.0,   // 温度初期値
  0.0,   // 湿度初期値
  "",    // 快適度初期値
  0,     // タイムスタンプ初期値
  false  // データ有効フラグ初期値
};

// タイミング制御用変数
unsigned long lastDisplayUpdateTime = 0;  // 最後に画面を更新した時刻

// 新規追加：交互表示の状態とタイミング制御
unsigned long lastInteractiveDisplayTime = 0;  // 最後に交互表示を更新した時刻
bool displayCO2 = true;                        // 現在CO2を表示中かTHIを表示中か

// ========== 関数の前方宣言 ==========
/*
 * 前方宣言：関数を使用する前に、その存在をコンパイラに知らせる
 * C++では関数を呼び出す前に定義されている必要があるため
 */
void initializeDisplaySystem();
void showSystemStartupMessage();
void establishWiFiConnection();
bool checkWiFiConnectionStatus();
void displayWiFiConnectionSuccess();
void synchronizeSystemTimeWithNTP();
bool attemptNTPTimeSynchronization();
void displayNTPSynchronizationResult(bool wasSuccessful);
void configureMQTTConnection();
void establishMQTTBrokerConnection();
String generateUniqueMQTTClientId();
bool attemptMQTTBrokerConnection(const String& clientIdentifier);
void subscribeToMQTTDataTopic();
void displayMQTTConnectionSuccess();
void displayMQTTConnectionFailure();
void handleIncomingMQTTMessage(char* topicName, byte* messagePayload, unsigned int messageLength);
bool validateJSONDataIntegrity(const String& jsonData);
String convertRawPayloadToString(byte* rawPayload, unsigned int payloadLength);
SensorDataPacket parseJSONSensorData(const String& jsonString);
void updateCurrentSensorData(const SensorDataPacket& newSensorData);
void maintainMQTTBrokerConnection();
void processIncomingMQTTMessages();
void updateDisplayIfIntervalElapsed();
void updateSystemNetworkTime();
void refreshEntireDisplay();
void displayApplicationTitle();
void displayCurrentSystemTime();
void displaySensorDataOrErrorMessage();
void displayCO2ConcentrationData();
void displayTHIComfortData();
void displayNoDataAvailableMessage();
void displayNetworkConnectionStatus();
void displayJSONParsingError(const char* errorDescription);
void showConnectionStatusMessage(const char* statusMessage);
void clearDisplayScreenWithColor(uint16_t backgroundColor);
void printMQTTSubscriptionDebugInfo();

// ========== メイン初期化関数 ==========
/*
 * setup()関数：Arduinoプログラムの開始点
 * 電源投入時に一度だけ実行される
 * 各種初期化処理を順序立てて実行
 */
void setup() {
  // シリアル通信を開始（デバッグ情報出力用）
  Serial.begin(115200);  // 115200は通信速度（ボーレート）
  Serial.println("========== M5StickCPlus2 Sensor Monitor 起動開始 ==========");

  // Step 1: ディスプレイシステムの初期化
  initializeDisplaySystem();
  showSystemStartupMessage();

  // Step 2: WiFiネットワークへの接続
  establishWiFiConnection();

  // Step 3: インターネット時刻との同期
  synchronizeSystemTimeWithNTP();

  // Step 4: MQTT通信の設定と接続
  configureMQTTConnection();
  establishMQTTBrokerConnection();

  // Step 5: 初期画面の表示
  refreshEntireDisplay();

  Serial.println("========== 初期化処理完了：システム稼働開始 ==========");
}

// ========== メインループ関数 ==========
/*
 * loop()関数：setup()の後に無限に繰り返し実行される
 * システムの主要な動作処理を行う
 */
void loop() {
  // 1. MQTT接続の維持（切断されていたら再接続）
  maintainMQTTBrokerConnection();

  // 2. 受信したMQTTメッセージの処理
  processIncomingMQTTMessages();

  // 3. 画面表示の定期更新（3秒毎にCO2/THI交互表示、データがなければNo Dataを表示）
  updateDisplayIfIntervalElapsed();

  // 4. システム時刻の更新
  updateSystemNetworkTime();

  // 5. 次のループまで少し待機（CPUリソース節約）
  delay(MAIN_LOOP_DELAY_MILLISECONDS);
}

// ========== ディスプレイ初期化関数群 ==========

/*
 * M5StickCPlus2のディスプレイシステムを初期化
 * 画面の向きや基本設定を行う
 */
void initializeDisplaySystem() {
  // M5ライブラリの初期化（ハードウェア全体の初期化）
  M5.begin();

  // 画面を横向きに設定（1 = 90度回転）
  M5.Display.setRotation(1);

  // 画面を黒色でクリア
  clearDisplayScreenWithColor(BLACK);

  // 文字色を白に設定
  M5.Display.setTextColor(WHITE);

  // 文字サイズを標準（2倍）に設定
  M5.Display.setTextSize(2);

  Serial.println("✅ ディスプレイシステム初期化完了");
}

/*
 * 起動時のメッセージを画面に表示
 * ユーザーにシステムの起動を知らせる
 */
void showSystemStartupMessage() {
  clearDisplayScreenWithColor(BLACK);
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y);
  M5.Display.println("Starting...");
  Serial.println("📱 起動メッセージを画面に表示");
}

// ========== WiFi接続関数群 ==========

/*
 * WiFiネットワークへの接続を実行
 * 接続完了まで待機し、結果を表示
 */
void establishWiFiConnection() {
  Serial.println("🌐 WiFi接続処理を開始");
  showConnectionStatusMessage("WiFi connecting...");

  // WiFi接続を開始（非同期処理）
  WiFi.begin(WIFI_NETWORK_NAME, WIFI_NETWORK_PASSWORD);

  // 接続完了まで待機（ドット表示で進行状況を示す）
  while (!checkWiFiConnectionStatus()) {
    delay(500);             // 0.5秒待機
    M5.Display.print(".");  // 進行状況を画面に表示
    Serial.print(".");      // 進行状況をシリアルに出力
  }

  // 接続成功時の処理
  displayWiFiConnectionSuccess();
  Serial.println("\n✅ WiFi接続成功");
  Serial.print("📡 割り当てられたIPアドレス: ");
  Serial.println(WiFi.localIP());
}

/*
 * WiFi接続状態をチェック
 * 戻り値：true = 接続済み、false = 未接続
 */
bool checkWiFiConnectionStatus() {
  return WiFi.status() == WL_CONNECTED;
}

/*
 * WiFi接続成功時の画面表示
 * IPアドレスも併せて表示
 */
void displayWiFiConnectionSuccess() {
  clearDisplayScreenWithColor(BLACK);
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y);
  M5.Display.println("WiFi Connected!");
  // IPアドレスをタイトルから少し下の位置に表示
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y + 20);  // Y座標を調整
  M5.Display.println(WiFi.localIP());                             // IPアドレスを表示
  delay(CONNECTION_SUCCESS_DISPLAY_TIME);
}

// ========== NTP時刻同期関数群 ==========

/*
 * NTPサーバとの時刻同期を実行
 * インターネット上の時刻サーバから正確な時刻を取得
 */
void synchronizeSystemTimeWithNTP() {
  Serial.println("🕐 NTP時刻同期処理を開始");
  showConnectionStatusMessage("NTP Sync...");

  // NTPクライアントを開始
  timeClient.begin();

  // 同期を試行し、結果を記録
  bool synchronizationSuccess = attemptNTPTimeSynchronization();

  // 結果に応じた画面表示
  displayNTPSynchronizationResult(synchronizationSuccess);
}

/*
 * NTP同期を指定回数まで試行
 * 戻り値：true = 同期成功、false = 同期失敗
 */
bool attemptNTPTimeSynchronization() {
  // 最大試行回数まで繰り返し
  for (int currentAttempt = 0; currentAttempt < MAXIMUM_NTP_RETRY_ATTEMPTS; currentAttempt++) {
    // 時刻更新を試行
    if (timeClient.update()) {
      Serial.println("✅ NTP時刻同期成功");
      return true;  // 成功時は即座に終了
    }

    // 失敗時は強制更新を試行
    timeClient.forceUpdate();
    delay(1000);            // 1秒待機
    M5.Display.print(".");  // 進行状況表示
    Serial.print(".");      // シリアルにも出力
  }

  Serial.println("\n❌ NTP時刻同期失敗");
  return false;  // 全試行失敗
}

/*
 * NTP同期結果の画面表示
 * 引数：wasSuccessful = 同期成功/失敗の結果
 */
void displayNTPSynchronizationResult(bool wasSuccessful) {
  clearDisplayScreenWithColor(BLACK);
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y);

  if (wasSuccessful) {
    // 成功時：同期した時刻を表示
    M5.Display.println("NTP Synced!");
    // 同期した時刻をタイトルから少し下の位置に表示
    M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y + 20);  // Y座標を調整
    M5.Display.println(timeClient.getFormattedTime());
    Serial.print("🕐 同期完了時刻: ");
    Serial.println(timeClient.getFormattedTime());
  } else {
    // 失敗時：エラーメッセージを表示
    M5.Display.println("NTP Failed!");
    Serial.println("⚠️ 時刻同期に失敗しました");
  }

  delay(CONNECTION_SUCCESS_DISPLAY_TIME);
}

// ========== MQTT接続設定関数群 ==========

/*
 * MQTT通信の基本設定を行う
 * ブローカアドレス、ポート、コールバック関数を設定
 */
void configureMQTTConnection() {
  // MQTTブローカの接続先を設定
  mqttCommunicationClient.setServer(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT);

  // メッセージ受信時に呼び出される関数を設定
  mqttCommunicationClient.setCallback(handleIncomingMQTTMessage);

  Serial.println("⚙️ MQTT通信設定完了");
}

/*
 * MQTTブローカへの接続を実行
 * 接続完了までリトライを継続
 */
void establishMQTTBrokerConnection() {
  Serial.println("📡 MQTTブローカ接続処理を開始");
  showConnectionStatusMessage("MQTT connecting...");

  // 接続完了まで繰り返し
  while (!mqttCommunicationClient.connected()) {
    // 一意のクライアントIDを生成
    String uniqueClientId = generateUniqueMQTTClientId();

    // 接続を試行
    if (attemptMQTTBrokerConnection(uniqueClientId)) {
      // 成功時：データトピックを購読
      subscribeToMQTTDataTopic();
      displayMQTTConnectionSuccess();
      break;  // 成功したらループを抜ける
    } else {
      // 失敗時：エラー表示して再試行
      displayMQTTConnectionFailure();
    }
  }
}

/*
 * MQTT接続用の一意なクライアントIDを生成
 * 同じIDでの重複接続を防ぐため、ランダム要素を追加
 */
String generateUniqueMQTTClientId() {
  return String(MQTT_CLIENT_ID_PREFIX) + String(random(0xffff), HEX);
}

/*
 * MQTTブローカへの接続を試行
 * 引数：clientIdentifier = 使用するクライアントID
 * 戻り値：true = 接続成功、false = 接続失敗
 */
bool attemptMQTTBrokerConnection(const String& clientIdentifier) {
  bool connectionEstablished = mqttCommunicationClient.connect(clientIdentifier.c_str());

  if (connectionEstablished) {
    Serial.println("✅ MQTT接続成功");
    Serial.print("🆔 使用クライアントID: ");
    Serial.println(clientIdentifier);
  } else {
    Serial.print("❌ MQTT接続失敗, エラーコード: ");
    Serial.println(mqttCommunicationClient.state());
  }

  return connectionEstablished;
}

/*
 * 指定されたMQTTトピックを購読
 * センサーデータが送信されるトピックを監視開始
 */
void subscribeToMQTTDataTopic() {
  mqttCommunicationClient.subscribe(MQTT_TOPIC_NAME);
  Serial.print("📬 MQTTトピック購読開始: ");
  Serial.println(MQTT_TOPIC_NAME);
}

/*
 * MQTT接続成功時の画面表示
 */
void displayMQTTConnectionSuccess() {
  M5.Display.println("MQTT Connected!");
  delay(1000);
}

/*
 * MQTT接続失敗時の画面表示とリトライ待機
 */
void displayMQTTConnectionFailure() {
  M5.Display.print("Failed, rc=");
  M5.Display.print(mqttCommunicationClient.state());
  M5.Display.println(" retry in 5s");
  delay(MQTT_RECONNECTION_DELAY_MILLISECONDS);
}

// ========== MQTT メッセージ処理関数群 ==========

/*
 * MQTTメッセージ受信時に自動的に呼び出される関数（コールバック関数）
 * 引数：
 * topicName = 受信したトピック名
 * messagePayload = 受信したデータ（バイト配列）
 * messageLength = データの長さ
 */
void handleIncomingMQTTMessage(char* topicName, byte* messagePayload, unsigned int messageLength) {
  // バイト配列を文字列に変換
  String jsonMessageString = convertRawPayloadToString(messagePayload, messageLength);

  // デバッグ情報をシリアルに詳細出力
  Serial.println("=== MQTT受信データ詳細分析 ===");
  Serial.printf("📬 受信トピック: %s\n", topicName);
  Serial.printf("📏 データサイズ: %d バイト\n", messageLength);
  Serial.printf("📄 受信内容: '%s'\n", jsonMessageString.c_str());
  Serial.printf("🔚 最終文字: '%c' (ASCII: %d)\n",
                jsonMessageString.charAt(jsonMessageString.length() - 1),
                (int)jsonMessageString.charAt(jsonMessageString.length() - 1));

  // JSONデータの妥当性をチェック
  if (!validateJSONDataIntegrity(jsonMessageString)) {
    Serial.println("❌ 無効なJSONデータを検出");
    displayJSONParsingError("Invalid JSON");
    return;  // エラー時は処理を中断
  }

  // JSONデータを解析してセンサーデータに変換
  SensorDataPacket parsedSensorData = parseJSONSensorData(jsonMessageString);

  // 解析結果をチェック
  if (parsedSensorData.hasValidData) {
    // 成功：データを更新
    updateCurrentSensorData(parsedSensorData);
    Serial.printf("✅ センサーデータ更新成功 - CO2: %d ppm, THI: %.1f\n",
                  parsedSensorData.carbonDioxideLevel,
                  parsedSensorData.thermalComfortIndex);
    // データが更新されたので、直ちに表示をリフレッシュ
    refreshEntireDisplay();
  } else {
    // 失敗：エラー表示
    Serial.println("❌ センサーデータ解析失敗");
    displayJSONParsingError("Parse Failed");
  }

  Serial.println("========================\n");
}

/*
 * JSONデータの基本的な妥当性をチェック
 * 引数：jsonData = 検証するJSON文字列
 * 戻り値：true = 有効、false = 無効
 */
bool validateJSONDataIntegrity(const String& jsonData) {
  String trimmedData = jsonData;
  trimmedData.trim();  // 前後の空白を除去

  // 空文字列チェック
  if (trimmedData.length() == 0) {
    Serial.println("⚠️ エラー: 空のJSONデータ");
    return false;
  }

  // JSON開始文字チェック
  if (!trimmedData.startsWith("{")) {
    Serial.println("⚠️ エラー: JSONが'{'で始まっていません");
    return false;
  }

  // JSON終了文字チェック
  if (!trimmedData.endsWith("}")) {
    Serial.println("⚠️ エラー: JSONが'}'で終わっていません");
    Serial.printf("📝 実際の終了文字: '%c'\n", trimmedData.charAt(trimmedData.length() - 1));
    return false;
  }

  // 中括弧のバランスチェック
  int openBraceCount = 0;
  for (int i = 0; i < trimmedData.length(); i++) {
    if (trimmedData.charAt(i) == '{') openBraceCount++;
    if (trimmedData.charAt(i) == '}') openBraceCount--;
  }

  if (openBraceCount != 0) {
    Serial.printf("⚠️ エラー: 中括弧のバランス不正 (不足/過多: %d)\n", openBraceCount);
    return false;
  }

  Serial.println("✅ JSON基本構造チェック合格");
  return true;
}

/*
 * バイト配列を文字列に変換
 * 制御文字をフィルタリングして読み取り可能な文字のみ抽出
 */
String convertRawPayloadToString(byte* rawPayload, unsigned int payloadLength) {
  String convertedMessage;
  convertedMessage.reserve(payloadLength + 1);  // メモリ効率化のため事前確保

  for (unsigned int i = 0; i < payloadLength; i++) {
    // 印刷可能文字（ASCII 32-126）のみを文字列に追加
    if (rawPayload[i] >= 32 && rawPayload[i] <= 126) {
      convertedMessage += (char)rawPayload[i];
    } else {
      // 制御文字を検出した場合はログに記録
      Serial.printf("🔍 制御文字検出: ASCII %d at position %d\n", rawPayload[i], i);
    }
  }

  return convertedMessage;
}

/*
 * JSON文字列を解析してセンサーデータ構造体に変換
 * ArduinoJsonライブラリを使用してJSONを解析
 */
SensorDataPacket parseJSONSensorData(const String& jsonString) {
  // 初期値でデータ構造体を初期化
  SensorDataPacket extractedData = { 0, 0.0, 0.0, 0.0, "", 0, false };

  // JSON解析用のドキュメントオブジェクトを作成
  DynamicJsonDocument jsonDocument(JSON_PARSING_MEMORY_SIZE);

  // JSON文字列を解析
  DeserializationError parseError = deserializeJson(jsonDocument, jsonString);

  // 解析エラーチェック
  if (parseError) {
    Serial.println("=== JSON解析エラー詳細 ===");
    Serial.printf("❌ エラー内容: %s\n", parseError.c_str());
    Serial.printf("📄 問題のあるJSON: '%s'\n", jsonString.c_str());
    Serial.println("========================");
    return extractedData;  // エラー時は初期値を返す
  }

  // 各データフィールドを抽出（ログ付き）
  Serial.println("=== JSONデータ抽出処理 ===");

  // CO2濃度データの抽出
  if (jsonDocument.containsKey("co2")) {
    extractedData.carbonDioxideLevel = jsonDocument["co2"];
    Serial.printf("📊 CO2濃度: %d ppm\n", extractedData.carbonDioxideLevel);
  } else {
    Serial.println("⚠️ 警告: CO2データが見つかりません");
  }

  // THI（温熱指標）データの抽出
  if (jsonDocument.containsKey("thi")) {
    extractedData.thermalComfortIndex = jsonDocument["thi"];
    Serial.printf("🌡️ THI値: %.1f\n", extractedData.thermalComfortIndex);
  } else {
    Serial.println("⚠️ 警告: THIデータが見つかりません");
  }

  // 温度データの抽出（オプション）
  if (jsonDocument.containsKey("temperature")) {
    extractedData.ambientTemperature = jsonDocument["temperature"];
    Serial.printf("🌡️ 温度: %.1f°C\n", extractedData.ambientTemperature);
  }

  // 湿度データの抽出（オプション）
  if (jsonDocument.containsKey("humidity")) {
    extractedData.relativeHumidity = jsonDocument["humidity"];
    Serial.printf("💧 湿度: %.1f%%\n", extractedData.relativeHumidity);
  }

  // 快適度レベルの抽出（オプション）
  if (jsonDocument.containsKey("comfort_level")) {
    extractedData.comfortLevelDescription = jsonDocument["comfort_level"].as<String>();
    Serial.printf("😊 快適度: %s\n", extractedData.comfortLevelDescription.c_str());
  }

  // タイムスタンプの抽出（オプション）
  if (jsonDocument.containsKey("timestamp")) {
    extractedData.dataTimestamp = jsonDocument["timestamp"];
    Serial.printf("⏰ タイムスタンプ: %lu\n", extractedData.dataTimestamp);
  }

  Serial.println("=======================");

  // データ有効フラグを立てる
  extractedData.hasValidData = true;
  return extractedData;
}

/*
 * 解析済みセンサーデータで現在のデータを更新
 * 引数：newSensorData = 新しいセンサーデータ
 */
void updateCurrentSensorData(const SensorDataPacket& newSensorData) {
  currentSensorReading = newSensorData;
}

// ========== メインループ処理関数群 ==========

/*
 * MQTT接続状態を監視し、切断時は自動再接続
 * 通信が不安定な環境での安定動作を確保
 */
void maintainMQTTBrokerConnection() {
  if (!mqttCommunicationClient.connected()) {
    Serial.println("⚠️ MQTT接続が切断されました。再接続を試行します...");
    establishMQTTBrokerConnection();
  }
}

/*
 * 受信したMQTTメッセージの処理
 * PubSubClientライブラリの内部処理を実行
 */
void processIncomingMQTTMessages() {
  mqttCommunicationClient.loop();
}

/*
 * 指定間隔（3秒）が経過していればCO2/THIを交互に表示を更新
 * データがない場合は「No Data」を表示
 */
void updateDisplayIfIntervalElapsed() {
  unsigned long currentSystemTime = millis();

  // 交互表示のタイミングをチェック
  if (currentSystemTime - lastInteractiveDisplayTime >= INTERACTIVE_DISPLAY_INTERVAL_MILLISECONDS) {
    clearDisplayScreenWithColor(BLACK);  // 表示切り替え前に画面をクリア
    displayApplicationTitle();
    displayCurrentSystemTime();
    displayNetworkConnectionStatus();

    if (currentSensorReading.hasValidData) {
      if (displayCO2) {
        displayCO2ConcentrationData();
      } else {
        displayTHIComfortData();
      }
      displayCO2 = !displayCO2;  // 次回のために表示フラグを反転
    } else {
      displayNoDataAvailableMessage();  // データがなければNo Data
    }
    lastInteractiveDisplayTime = currentSystemTime;  // 更新時刻を記録
  }
}


/*
 * システム時刻をNTPサーバと定期同期
 * 時刻のずれを防ぐため定期的に更新
 */
void updateSystemNetworkTime() {
  timeClient.update();
}

// ========== ディスプレイ表示関数群 ==========

/*
 * 画面全体を更新
 * タイトル、時刻、センサーデータ、接続状態を表示
 */
void refreshEntireDisplay() {
  // 画面をクリア
  clearDisplayScreenWithColor(BLACK);

  // 各表示要素を順次描画
  displayApplicationTitle();
  displayCurrentSystemTime();
  displayNetworkConnectionStatus();  // 接続状態を上部に移動して常に表示

  // 初期表示、またはデータ受信直後に呼び出された場合は、現在の表示フラグに基づいて表示
  if (currentSensorReading.hasValidData) {
    if (displayCO2) {
      displayCO2ConcentrationData();
    } else {
      displayTHIComfortData();
    }
  } else {
    displayNoDataAvailableMessage();
  }
}

/*
 * アプリケーションタイトルを画面上部に表示
 * 水色（CYAN）で目立たせる
 */
void displayApplicationTitle() {
  M5.Display.setTextSize(1);      // 小さい文字サイズ
  M5.Display.setTextColor(CYAN);  // 水色
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y);
  M5.Display.println("Sensor Monitor");
}

/*
 * 現在時刻を画面右上に表示
 * NTPで同期した正確な時刻を表示
 */
void displayCurrentSystemTime() {
  M5.Display.setTextColor(WHITE);  // 白色
  M5.Display.setCursor(TIME_DISPLAY_X, TIME_DISPLAY_Y);
  M5.Display.println(timeClient.getFormattedTime());
}

/*
 * ネットワーク接続状態を画面右上に表示
 * 接続状態に応じて色を変更（緑=OK、赤=NG）
 */
void displayNetworkConnectionStatus() {
  M5.Display.setTextSize(1);  // 小サイズ文字
  // 接続状態に応じて色を変更
  M5.Display.setTextColor(mqttCommunicationClient.connected() ? GREEN : RED);
  M5.Display.setCursor(CONNECTION_STATUS_X, CONNECTION_STATUS_Y);
  M5.Display.println(mqttCommunicationClient.connected() ? "MQTT:OK" : "MQTT:NG");
}


/*
 * センサーデータまたはエラーメッセージを表示
 * データの有無に応じて表示内容を切り替え
 * (この関数は updateDisplayIfIntervalElapsed() にロジックが移されたため、呼び出しはほぼ無くなりますが、
 * refreshEntireDisplay() で初期描画用として残します。)
 */
void displaySensorDataOrErrorMessage() {
  // この関数は主にrefreshEntireDisplay()からの初期表示用
  if (currentSensorReading.hasValidData) {
    if (displayCO2) {  // refreshEntireDisplayの初回呼び出し時はCO2から
      displayCO2ConcentrationData();
    } else {  // それ以外の場合は現在のdisplayCO2フラグに従う
      displayTHIComfortData();
    }
  } else {
    displayNoDataAvailableMessage();
  }
}

/*
 * CO2濃度データを大きく表示
 * 緑色で見やすく表示し、単位（ppm）も併記
 */
void displayCO2ConcentrationData() {
  // CO2ラベルを表示
  M5.Display.setTextSize(2);       // 中サイズ文字 (CO2:)
  M5.Display.setTextColor(GREEN);  // 緑色
  M5.Display.setCursor(LARGE_LABEL_X, LARGE_LABEL_Y);
  M5.Display.println("CO2:");

  // CO2数値を**最も大きく**表示 (サイズを6に変更)
  M5.Display.setTextSize(6);  // 極大サイズ文字
  M5.Display.setCursor(LARGE_VALUE_X, LARGE_VALUE_Y);
  M5.Display.printf("%d ppm", currentSensorReading.carbonDioxideLevel);
}

/*
 * THI（温熱指標）データを大きく表示
 * オレンジ色で見やすく表示し、小数点第1位まで表示
 */
void displayTHIComfortData() {
  // THIラベルを表示
  M5.Display.setTextSize(2);        // 中サイズ文字 (THI:)
  M5.Display.setTextColor(ORANGE);  // オレンジ色
  M5.Display.setCursor(LARGE_LABEL_X, LARGE_LABEL_Y);
  M5.Display.println("THI:");

  // THI数値を**最も大きく**表示 (サイズを6に変更)
  M5.Display.setTextSize(6);  // 極大サイズ文字
  M5.Display.setCursor(LARGE_VALUE_X, LARGE_VALUE_Y);
  M5.Display.printf("%.1f", currentSensorReading.thermalComfortIndex);
}

/*
 * データが取得できていない時のメッセージ表示
 * 赤色で警告的に表示
 */
void displayNoDataAvailableMessage() {
  M5.Display.setTextSize(2);     // 中サイズ文字
  M5.Display.setTextColor(RED);  // 赤色
  M5.Display.setCursor(NO_DATA_MESSAGE_X, NO_DATA_MESSAGE_Y);
  M5.Display.println("No Data");
}


/*
 * JSON解析エラー時の専用画面表示
 * エラーの種類も併せて表示
 */
void displayJSONParsingError(const char* errorDescription) {
  clearDisplayScreenWithColor(BLACK);

  // タイトル表示
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(CYAN);
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y);
  M5.Display.println("Sensor Monitor");

  // 現在時刻表示
  M5.Display.setTextColor(WHITE);
  M5.Display.setCursor(TIME_DISPLAY_X, TIME_DISPLAY_Y);
  M5.Display.println(timeClient.getFormattedTime());

  // 接続状態表示
  M5.Display.setTextSize(1);
  M5.Display.setTextColor(mqttCommunicationClient.connected() ? GREEN : RED);
  M5.Display.setCursor(CONNECTION_STATUS_X, CONNECTION_STATUS_Y);
  M5.Display.println(mqttCommunicationClient.connected() ? "MQTT:OK" : "MQTT:NG");

  // エラーメッセージ表示
  M5.Display.setTextSize(2);
  M5.Display.setTextColor(RED);
  M5.Display.setCursor(20, 50 + VERTICAL_OFFSET);  // Y座標を調整
  M5.Display.println("JSON Error");

  // エラー詳細表示
  M5.Display.setTextSize(1);
  M5.Display.setCursor(20, 80 + VERTICAL_OFFSET);  // Y座標を調整
  M5.Display.println(errorDescription);
}

// ========== ユーティリティ関数群 ==========

/*
 * 接続状態メッセージを画面に表示
 * 各種接続処理中に進行状況を表示
 */
void showConnectionStatusMessage(const char* statusMessage) {
  clearDisplayScreenWithColor(BLACK);
  M5.Display.setCursor(TITLE_POSITION_X, TITLE_POSITION_Y);
  M5.Display.println(statusMessage);
}

/*
 * 画面を指定色でクリア
 * 表示更新時の画面リセット処理
 */
void clearDisplayScreenWithColor(uint16_t backgroundColor) {
  M5.Display.fillScreen(backgroundColor);
}

/*
 * MQTT購読状況のデバッグ情報をシリアルに出力
 * トラブルシューティング用の詳細情報表示
 */
void printMQTTSubscriptionDebugInfo() {
  Serial.println("=== MQTT購読状況詳細確認 ===");
  Serial.printf("🏠 ブローカアドレス: %s:%d\n", MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT);
  Serial.printf("📬 購読トピック: %s\n", MQTT_TOPIC_NAME);
  Serial.printf("🔗 接続状態: %s\n", mqttCommunicationClient.connected() ? "接続中" : "切断中");
  Serial.printf("📊 クライアント状態コード: %d\n", mqttCommunicationClient.state());
  Serial.println("===========================");
}

/*
 * ================================================================================
 * プログラム終了
 * ================================================================================
 * * このプログラムの動作フロー：
 * 1. setup()で初期化（WiFi接続→NTP同期→MQTT接続）
 * 2. loop()で継続処理（MQTT監視→メッセージ処理→画面更新）
 * 3. センサーデータ受信時に自動的に画面更新
 * * 主要な特徴：
 * - 自動再接続機能（WiFi/MQTT切断時）
 * - 詳細なエラーハンドリング
 * - 直感的な画面表示
 * - 豊富なデバッグ情報出力
 * * カスタマイズポイント：
 * - WiFi設定（SSID/パスワード）
 * - MQTTブローカ設定（IPアドレス/ポート/トピック）
 * - 画面レイアウト（座標/色/サイズ）
 * - 更新間隔（表示更新/時刻同期）
 * * ================================================================================
 */