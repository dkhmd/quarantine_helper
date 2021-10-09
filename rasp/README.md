# Raspberry Pi Zero WH 環境構築方法
T.B.D

# プログラム
## 本番運用用
### gateway/gateway.py
ArduinoからBT経由で動作判定結果と位置情報を受け取り、AWS IoT Coreに送るプログラム  
ArdunoがCentral、Raspberry PiがPeripheralとして動く  
#### ライブラリ
- pybleno
#### 使い方
```bash
sudo python gateway.py
```
#### 引数
なし

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
