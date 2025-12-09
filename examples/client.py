import socket

def main():
    # Создаем TCP-сокет (SOCK_STREAM)
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Определяем адрес и порт сервера
    server_host = '127.0.0.1'  # localhost
    server_port = 8080

    print(f"Подключаемся к серверу {server_host}:{server_port}...")

    try:
        # Подключаемся к серверу (connect)
        client_socket.connect((server_host, server_port))
        print("Соединение с сервером установлено")

        # Отправляем сообщение серверу (send)
        message = "Hello World from Client!"
        client_socket.sendall(message.encode('utf-8'))
        print(f"Отправлено серверу: {message}")

        # Получаем ответ от сервера (recv)
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
        # Закрываем соединение (close)
        client_socket.close()
        print("Соединение закрыто")


if __name__ == "__main__":
    main()
