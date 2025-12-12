import socket
import sys

def main():
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    
    try:
        server_socket.bind(("localhost", 8080))
        server_socket.listen(1)
        print(f"Сервер запущен и ожидает подключений...")
        
        while True:
            client_socket, client_address = server_socket.accept()
            print(f"\nПодключение установлено с {client_address}")
            
            try:
                data = client_socket.recv(1024)
                if data:
                    print(f"Получено от клиента: {data.decode('utf-8')}")

                    response = "Hello from Python Server!"
                    client_socket.sendall(response.encode('utf-8'))

                else:
                    print("Соединение с клиентом потеряно")
            
            except Exception as e:
                print(f"Ошибка с клиентом {client_address}: {e}")
            
            finally:
                client_socket.close()
                print(f"Соединение с {client_address} закрыто")
    
    except KeyboardInterrupt:
        print("\nServer shutting down...")
    
    except Exception as e:
        print(f"Server error: {e}")
    
    finally:
        server_socket.close()
        print("Server socket closed")

if __name__ == "__main__":
    main()