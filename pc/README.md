# プログラム
全てのプログラムはpython3.5以降での実行を前提としている
## data_upload.py
Arduinoからシリアル経由でEMG/IMUのデータを受け取り、プロットするプログラム  
入力されたラベルを付与した上で、CSV形式でAWS S3にアップロードする
### ライブラリ
- boto3
- matplotlib.pyplot
- serial

### 使い方
```bash
sudo python data_upload.py label (samples)
```
### 引数
- label: ラベル名
- sample: サンプリング数、省略時は256 
### 注意
- 事前に `aws configure` を[設定](https://docs.aws.amazon.com/ja_jp/cli/latest/userguide/cli-configure-quickstart.html)しておくこと
```bash
$ aws configure
AWS Access Key ID [None]: アクセスキー
AWS Secret Access Key [None]: シークレットアクセスキー
Default region name [None]: ap-northeast-1
Default output format [None]: json
```
- /tmp/data.csv に一時ファイルを保存するため、書き込み権限を渡しておくこと
- プログラム内でシリアルデバイス名を直書しているため、デバイス名が異なる場合はプログラム内の `SERIAL_DEVICE_NAME` を所望のデバイス名に書き換えること

## serial_disp.py
Arduinoからのシリアル出力を表示するだけのプログラム  
### ライブラリ
- serial

### 使い方
```bash
sudo python serial_disp.py
```

## data_upload.py
Arduinoからシリアル経由でCO2/温湿度/気圧/ドアの状態のデータを受け取るプログラム
CSV形式でAWS S3にアップロードする
### ライブラリ
- boto3
- serial

### 使い方
```bash
sudo python co2_upload.py (conditioner) (ventilation) (fan)
```
### 引数
- conditioner: エアコンの状態(デフォルト:ON)
- ventilation: 機械換気の状態(デフォルト:ON)
- fan: 扇風機の状態(デフォルト:ON)
### 注意
- 事前に `aws configure` を[設定](https://docs.aws.amazon.com/ja_jp/cli/latest/userguide/cli-configure-quickstart.html)しておくこと
```bash
$ aws configure
AWS Access Key ID [None]: アクセスキー
AWS Secret Access Key [None]: シークレットアクセスキー
Default region name [None]: ap-northeast-1
Default output format [None]: json
```
- /tmp/data_air.csv に一時ファイルを保存するため、書き込み権限を渡しておくこと
- プログラム内でシリアルデバイス名を直書しているため、デバイス名が異なる場合はプログラム内の `SERIAL_DEVICE1_NAME`(ドアの状態側) および `SERIAL_DEVICE2_NAME`(CO2センサー側) を所望のデバイス名に書き換えること