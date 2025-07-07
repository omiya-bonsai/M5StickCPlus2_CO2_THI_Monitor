# M5StickCPlus2 MQTT Sensor Monitor

M5StickCPlus2で、室内のCO₂濃度とTHI（温熱指標）を可視化する高機能な室内環境モニターです。

MQTTで受信したデータを、見やすい大型フォントで3秒ごとに交互に表示。NTPで取得した正確な時刻やMQTTの接続状況もひと目で確認できます。

-----

## 主な機能

  - **センサーデータの可視化**: CO₂濃度とTHI（温熱指標）を交互に表示
  - **時刻・ステータス表示**: NTPで同期した時刻とMQTT接続状況を常に表示
  - **自動再接続機能**: WiFiやMQTTが切断されても自動で復帰
  - **簡単なセットアップ**: `config.h` ファイルにWi-FiとMQTTの情報を設定するだけで利用可能

-----

## 必要なもの

### ハードウェア

  - **M5StickCPlus2**: モニター本体
  - **MQTTブローカー**: Mosquittoなどをインストールしたサーバー（Raspberry Piなど）
  - **MQTTパブリッシャー**: センサーデータをJSON形式で送信するデバイス（ESP32, Raspberry Piなど）

### ソフトウェア

  - **Arduino IDE** または **PlatformIO**
  - **ライブラリ**:
      - `M5StickCPlus2` (M5GFX)
      - `PubSubClient` by Nick O'Leary
      - `ArduinoJson` by Benoît Blanchon
      - `NTPClient` by Fabrice Weinberg

-----

## セットアップ手順

1.  **リポジトリをクローン**

    ```bash
    git clone https://github.com/omiya-bonsai/M5StickCPlus2_CO2_THI_Monitor.git
    cd M5StickCPlus2_CO2_THI_Monitor
    ```

2.  **設定ファイルの作成**
    `config.example.h` をコピーし、`config.h` という名前で新しいファイルを作成します。

3.  **設定の編集**
    作成した `config.h` を開き、お使いの環境に合わせて以下の3項目を編集してください。

      - `WIFI_NETWORK_NAME`: Wi-FiのSSID
      - `WIFI_NETWORK_PASSWORD`: Wi-Fiのパスワード
      - `MQTT_BROKER_ADDRESS`: MQTTブローカーのIPアドレス

4.  **書き込み**
    Arduino IDEまたはPlatformIOでプロジェクトを開き、M5StickCPlus2にスケッチを書き込みます。

-----

## MQTTデータ形式

このモニターは、以下のJSON形式のペイロードを購読します。`config.h`で設定したトピックに、この形式でデータを送信してください。

  - **必須項目**:
      - `co2`: (整数) CO₂濃度 (ppm)
      - `thi`: (浮動小数点数) 温熱指標
  - **オプション項目**:
      - `temperature`: (浮動小数点数) 温度 (℃)
      - `humidity`: (浮動小数点数) 湿度 (%)
      - `comfort_level`: (文字列) 快適度の説明など
      - `timestamp`: (整数) Unixタイムスタンプ

**ペイロードの例:**

```json
{
  "co2": 784,
  "thi": 72.6,
  "temperature": 25.5,
  "humidity": 60.1
}
```

-----

## ライセンス

本プロジェクトは [MIT License](https://www.google.com/search?q=LICENSE) の下で公開されています。
