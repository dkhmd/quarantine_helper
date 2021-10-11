# Raspberry Pi Zero 環境構築方法
T.B.D

# プログラム
全てのプログラムはpython3.5以降での実行を前提としている
## gateway.py
ArduinoからBT経由で動作判定結果と位置情報を受け取り、AWS IoT Coreに送る本番用プログラム  
ArdunoがCentral、Raspberry PiがPeripheralとして動く  
### ライブラリ
- pybleno
### 使い方
```bash
sudo python gateway.py
```
### 引数
なし

## publish.py
上記gateway.pyからモジュールとして呼び出されることを想定しており、AWS IoTにデータを送るプログラム  
単独での実行時はテストが可能
### ライブラリ
- awsiotsdk
### 使い方
```bash
python publish.py --cert certs/certificate.pem.crt --key certs/private.pem.key --root certs/root.pem --data sample.json  --topic test/testing --ep endpoint.iot.ap-northeast-1.amazonaws.com
```
### 引数
- cert: AWS IoT デバイス証明書のファイルパス
- key: AWS IoT プライベートキーのファイルパス
- root: AWS IoT ルート CA 証明書のファイルパス
- data: データ(JSON形式)のファイルパス
- topic: MQTTのトピック名
- ep: AWS IoT エンドポイント
### 注意
証明書などを誤ってコミットしないこと  
テストする際は証明書などはgit管理下以外、もしくは.gitignoreされているcertsディレクトリ以下に保管した上でテストすること

