# Raspberry Pi 環境構築方法
- [OSはraspbianをインストールする](https://qiita.com/tksnkym/items/31a237e27cbc51790cdd)
- [初期設定をする](https://qiita.com/HeRo/items/c1c30d7267faeb304538)
- [python をデフォルト3にする](https://www.ingenious.jp/articles/howto/raspberry-pi-howto/python-3-change/)

以降のプログラムに対して、ライブラリはpipコマンドでインストールする  
例) `pip install pybleno` 

# プログラム
全てのプログラムはpython3.5以降での実行を前提としている
## gateway.py
ArduinoからBT経由で動作判定結果と位置情報を受け取り、AWS IoT Coreに送る本番用プログラム  
ArdunoがCentral、Raspberry PiがPeripheralとして動く  
### ライブラリ
- pybleno
- awsiotsdk
### 使い方
```bash
sudo python gateway.py --cert certs/certificate.pem.crt --key certs/private.pem.key --root certs/root.pem --topic test/testing --ep endpoint.iot.ap-northeast-1.amazonaws.com
```
### 引数
- cert: AWS IoT デバイス証明書のファイルパス
- key: AWS IoT プライベートキーのファイルパス
- root: AWS IoT ルート CA 証明書のファイルパス
- ep: AWS IoT エンドポイント

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
- 証明書などを誤ってコミットしないこと  
テストする際は証明書などはgit管理下以外、もしくは.gitignoreされているcertsディレクトリ以下に保管した上でテストすること
- 事前に `aws configure` を[設定](https://docs.aws.amazon.com/ja_jp/cli/latest/userguide/cli-configure-quickstart.html)しておくこと
```bash
$ aws configure
AWS Access Key ID [None]: アクセスキー
AWS Secret Access Key [None]: シークレットアクセスキー
Default region name [None]: ap-northeast-1
Default output format [None]: json
