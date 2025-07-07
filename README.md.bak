# M5StickCPlus2 CO2/THIセンサーモニター

このプロジェクトは、M5StickCPlus2デバイスを使用して、指定されたMQTTブローカーからCO2濃度とTHI（温熱指標）データを受信し、視認性の高いディスプレイに交互に表示するものです。Wi-Fi接続、NTPによる時刻同期、MQTT通信の機能を備えています。

## 🚀 プロジェクトの目的

  * **CO2/THIデータのリアルタイム表示**: 3秒ごとにCO2濃度とTHI値を交互に大きく表示し、現在の環境状態を直感的に把握できます。
  * **ネットワーク連携**: Wi-Fi経由でMQTTブローカーからセンサーデータを受信します。
  * **正確な時刻表示**: NTPサーバーと同期し、正確な現在時刻を画面上部に表示します。
  * **高視認性UI**: Apple社チーフデザイナー監修による、数値の大きさと配置にこだわったユーザーインターフェース。
  * **設定の匿名性**: Wi-FiのSSIDやパスワードなどの機密情報は`config.h`ファイルに分離され、Gitから除外されます。

## ✨ 特徴

  * **自動再接続**: Wi-FiやMQTT接続が切断された場合でも、自動的に再接続を試みます。
  * **エラーハンドリング**: JSONデータの解析エラーなどを検出し、画面にメッセージを表示します。
  * **デバッグ出力**: シリアルモニターを通じて詳細な動作状況やエラー情報を出力します。
  * **カスタマイズ可能**: 設定ファイル`config.h`を編集することで、Wi-Fi情報、MQTTブローカー、表示間隔などを簡単に変更できます。

## 📦 必要なもの

### ハードウェア

  * **M5StickCPlus2**
  * USB-Cケーブル (プログラムの書き込みと給電用)

### ソフトウェア / 開発環境

  * **Arduino IDE** (推奨) または **Visual Studio Code + PlatformIO / Arduino拡張機能**
  * **M5Stack ESP32 ボードパッケージ**: Arduino IDEのボードマネージャーからインストールします。
      * `m5stack:esp32:m5stack_stickc_plus2`
  * **必要なライブラリ**: Arduino IDEのライブラリマネージャーからインストールします。
      * **M5StickCPlus2** ([GitHubリポジトリ](https://github.com/m5stack/M5StickCPlus2))
      * **PubSubClient** ([GitHubリポジトリ](https://github.com/knolleary/pubsubclient))
      * **ArduinoJson** ([GitHubリポジトリ](https://github.com/bblanchon/ArduinoJson))
      * **NTPClient** ([GitHubリポジトリ](https://github.com/arduino-libraries/NTPClient))
      * `WiFi.h` と `WiFiUdp.h` はESP32ボードパッケージに付属しています。

## 🛠️ セットアップ方法

1.  **このリポジトリをクローンする**:

    ```bash
    git clone https://github.com/omiya-bonsai/M5StickCPlus2_CO2_THI_Monitor.git
    cd M5StickCPlus2_CO2_THI_Monitor
    ```

2.  **`config.h` ファイルを作成する**:
    `config.example.h` をコピーして、**`config.h`** という名前で新しいファイルを作成します。

    ```bash
    cp config.example.h config.h
    ```

    *`config.h`は`.gitignore`によってGitの追跡から除外されるため、個人情報が公開される心配はありません。*

3.  **`config.h` を編集する**:
    作成した`config.h`ファイルを開き、以下のプレースホルダーをあなたの環境に合わせて編集・保存してください。

    ```c++
    // ========== ネットワーク設定 ==========
    const char* WIFI_NETWORK_NAME = "YOUR_WIFI_SSID";     // あなたのWi-Fi SSID
    const char* WIFI_NETWORK_PASSWORD = "YOUR_WIFI_PASSWORD";  // あなたのWi-Fiパスワード

    // ========== MQTT設定 ==========
    const char* MQTT_BROKER_ADDRESS = "YOUR_MQTT_BROKER_IP";      // MQTTブローカーのIPアドレス
    // ... その他必要に応じて編集 ...
    ```

4.  **Arduino IDE でスケッチを開く**:
    `M5StickCPlus2_CO2_THI_Monitor.ino` をArduino IDEで開きます。

5.  **ボードとポートを選択**:
    ツール \> ボード \> M5Stack ESP32 Boards \> `M5Stick-C Plus2` を選択します。
    ツール \> シリアルポート からM5StickCPlus2が接続されているポートを選択します。

6.  **プログラムを書き込む**:
    Arduino IDEの「スケッチ」\>「検証・コンパイル」でコンパイルし、エラーがなければ「スケッチ」\>「マイコンボードに書き込む」でM5StickCPlus2にプログラムを書き込みます。

## 💡 使い方

  * プログラムがM5StickCPlus2に書き込まれると、自動的にWi-Fiに接続し、NTPサーバーと時刻を同期し、MQTTブローカーに接続します。
  * MQTTトピック`sensor_data`（`config.h`で設定）からJSON形式のデータを受信すると、CO2濃度とTHI値が3秒ごとに交互に大きく表示されます。
  * 画面上部には、`Sensor Monitor`というタイトル、現在の時刻、そしてMQTT接続の状態（`MQTT:OK`または`MQTT:NG`）が表示されます。
  * データが受信されていない場合やJSON解析エラーが発生した場合は、それを示すメッセージが表示されます。

## 📊 受信するJSONデータ形式

このスケッチは、以下のJSON形式のデータを受信することを想定しています。

```json
{
  "co2": 800,           // CO2濃度 (整数値, ppm)
  "thi": 72.5,          // THI (温熱指標, 浮動小数点数)
  "temperature": 25.3,  // (オプション) 環境温度
  "humidity": 55.0,     // (オプション) 相対湿度
  "comfort_level": "快適", // (オプション) 快適度レベル
  "timestamp": 1678886400 // (オプション) データのUNIXタイムスタンプ
}
```

## 🤝 貢献

このプロジェクトは、M5StickCPlus2を使った環境モニタリングのシンプルな例として提供されています。機能追加や改善の提案があれば、お気軽にIssueやPull Requestを作成してください。

-----

## 📜 ライセンス

このプロジェクトは、MITライセンスの下で公開されています。詳細については`LICENSE`ファイルをご確認ください。

-----
