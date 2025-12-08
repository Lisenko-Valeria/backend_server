import socket

def main():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)    
    
    try:
        client_socket.connect(('localhost', 8080))
        print("Соединение с сервером установлено")

        message = "Hello from Python Client!"
        client_socket.sendall(message.encode('utf-8'))

        response = client_socket.recv(1024)
        if response:
            print(f"Получено от сервера: {response.decode('utf-8')}")
        else:
            print("Сервер отключился")
    
    except ConnectionRefusedError:
        print("Не удалось подключиться к серверу. Убедитесь, что сервер запущен.")
    
    except Exception as e:
        print(f"Ошибка при обмене данными: {e}")
    
    finally:
        client_socket.close()
        print("Соединение закрыто")

if __name__ == "__main__":
    main()