# Raspberry Pi Zero WH 環境構築方法
T.B.D

# プログラム
全てのプログラムはpython3.5以降での実行を前提としている
## 本番運用用
### gateway/gateway.py
ArduinoからBT経由で動作判定結果と位置情報を受け取り、AWS IoT Coreに送る本番用プログラム  
ArdunoがCentral、Raspberry PiがPeripheralとして動く  
#### ライブラリ
- pybleno
#### 使い方
```bash
sudo python gateway.py
```
#### 引数
なし

### gateway/publish.py
上記gateway.pyからモジュールとして呼び出されることを想定しており、AWS IoTにデータを送るプログラム  
単独での実行時はテストが可能
#### ライブラリ
- awsiotsdk
#### 使い方
```bash
python publish.py --cert certs/certificate.pem.crt --key certs/private.pem.key --root certs/root.pem --data sample.json  --topic test/testing --ep endpoint.iot.ap-northeast-1.amazonaws.com
```
#### 引数
- cert: AWS IoT デバイス証明書のファイルパス
- key: AWS IoT プライベートキーのファイルパス
- root: AWS IoT ルート CA 証明書のファイルパス
- data: データ(JSON形式)のファイルパス
- topic: MQTTのトピック名
- ep: AWS IoT エンドポイント
#### 注意
証明書などを誤ってコミットしないこと  　
テストする際は証明書などはgit管理下以外、もしくは.gitignoreされているcertsディレクトリ以下に保管した上でテストすること
```

## デバッグ用
### debug/data_serial.py
Arduinoからシリアル経由でEMG/IMUのデータを受け取り、プロットするプログラム
#### ライブラリ
- serial
- matplotlib.pyplot
#### 使い方
```bash
python data_serial.py (sample)
```
#### 引数
sample: サンプリング数、省略時は2048  
#### 注意
プログラム内でシリアルデバイス名を直書しているため、デバイス名が異なる場合は以下の箇所を所望のデバイス名に書き換えること
```python
ser = serial.Serial('/dev/tty.usbmodem1141301', 115200, timeout=None)
```
