# プログラム
全てのプログラムはpython3.5以降での実行を前提としている
## data_upload.py
Arduinoからシリアル経由でEMG/IMUのデータを受け取り、CSV形式でAWS S3にアップロードするプログラム
### ライブラリ
- serial
- boto3
### 使い方
```bash
python data_upload.py
```
### 引数
なし
### 注意
プログラム内でシリアルデバイス名を直書しているため、デバイス名が異なる場合は以下の箇所を所望のデバイス名に書き換えること
```python
ser = serial.Serial('/dev/tty.usbmodem1141301', 115200, timeout=None)
```

## data_plot.py
Arduinoからシリアル経由でEMG/IMUのデータを受け取り、プロットするプログラム
### ライブラリ
- serial
- matplotlib.pyplot
### 使い方
```bash
python data_plot.py (sample)
```
### 引数
sample: サンプリング数、省略時は256 
### 注意
プログラム内でシリアルデバイス名を直書しているため、デバイス名が異なる場合は以下の箇所を所望のデバイス名に書き換えること
```python
ser = serial.Serial('/dev/tty.usbmodem1141301', 115200, timeout=None)
```
