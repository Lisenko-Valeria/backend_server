'''
pip install pyzmq'''


import zmq
import json
import time
from datetime import datetime
import threading

class ZMQServer:
    def __init__(self, port=1234, log_file="android_messages.log"):
        self.port = port
        self.log_file = log_file
        self.packet_count = 0
        self.is_running = False
        self.context = zmq.Context()
        self.socket = self.context.socket(zmq.REP)
        
    def start(self):
        self.socket.bind(f"tcp://*:{self.port}")
        self.is_running = True
        
        try:
            while self.is_running:
                message = self.socket.recv_string()
                self.packet_count += 1
                
                timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
                log_entry = {
                    "timestamp": timestamp,
                    "packet_number": self.packet_count,
                    "message": message
                }
                
                self._save_to_file(log_entry)
                
                print(f"[{timestamp}] Пакет #{self.packet_count}: {message}")
                
                response = f"Hello from Server! Получено пакетов: {self.packet_count}"
                self.socket.send_string(response)
                
        except KeyboardInterrupt:
            print("\n[SERVER] Остановка по запросу пользователя")
        except Exception as e:
            print(f"[SERVER] Ошибка: {e}")
        finally:
            self.stop()
    
    def _save_to_file(self, log_entry):
        try:
            with open(self.log_file, "a", encoding="utf-8") as f:
                json.dump(log_entry, f, ensure_ascii=False)
                f.write("\n")
        except Exception as e:
            print(f"[SERVER] Ошибка записи в файл: {e}")
    
    def show_all_data(self):
        try:
            print(f"\n{'='*50}")
            print("ВСЕ СОХРАНЕННЫЕ ДАННЫЕ:")
            print('='*50)
            
            with open(self.log_file, "r", encoding="utf-8") as f:
                for line in f:
                    if line.strip():
                        data = json.loads(line.strip())
                        print(f"Время: {data['timestamp']}")
                        print(f"Пакет №: {data['packet_number']}")
                        print(f"Сообщение: {data['message']}")
                        print("-" * 30)

        except FileNotFoundError:
            print("[SERVER] Файл с данными не найден")
        except Exception as e:
            print(f"[SERVER] Ошибка чтения файла: {e}")
    
    def get_stats(self):
        return {
            "total_packets": self.packet_count,
            "log_file": self.log_file,
            "is_running": self.is_running
        }
    
    def stop(self):
        self.is_running = False
        self.socket.close()
        self.context.term()
        print("[SERVER] Остановлен")

def main():
    server = ZMQServer(port=1234)
    
    server_thread = threading.Thread(target=server.start)
    server_thread.daemon = True
    server_thread.start()
    
    try:
        while True:
            print("\nКоманды:")
            print("1 - Показать все данные")
            print("2 - Показать статистику")
            print("3 - Выход")
            
            choice = input("\nВыберите команду: ").strip()
            
            if choice == "1":
                server.show_all_data()
            elif choice == "2":
                stats = server.get_stats()
                print(f"\nСтатистика:")
                print(f"Всего пакетов: {stats['total_packets']}")
                print(f"Файл логов: {stats['log_file']}")
                print(f"Сервер работает: {'Да' if stats['is_running'] else 'Нет'}")
            elif choice == "3":
                print("[SERVER] Выход...")
                server.stop()
                break
            else:
                print("[SERVER] Неверная команда")
                
            time.sleep(1)
            
    except KeyboardInterrupt:
        print("\n[SERVER] Завершение работы...")
        server.stop()

if __name__ == "__main__":
    main()