import socket

def main():
    # Создаем TCP-сокет (SOCK_STREAM)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Настраиваем опцию для повторного использования адреса (чтобы избежать ошибки "Address already in use")
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Определяем адрес и порт для привязки
    host = '127.0.0.1'  # localhost (INADDR_LOOPBACK эквивалент)
    port = 8080

    # Привязываем сокет к адресу (bind)
    server_socket.bind((host, port))
    print(f"Сервер привязан к {host}:{port}")

    # Начинаем слушать входящие соединения (listen)
    server_socket.listen(1)
    print("Сервер ожидает подключений...")

    # Принимаем входящее соединение (accept)
    client_socket, client_address = server_socket.accept()
    print(f"Подключение установлено с клиентом: {client_address}")

    try:
        # Получаем данные от клиента (recv)
        data = client_socket.recv(1024)
        if data:
            print(f"Получено от клиента: {data.decode('utf-8')}")

            # Отправляем ответ клиенту (send)
            response = "Hello World from Server!"
            client_socket.sendall(response.encode('utf-8'))
            print(f"Отправлено клиенту: {response}")
        else:
            print("Клиент отключился")

    except Exception as e:
        print(f"Ошибка при обмене данными: {e}")

    finally:
        # Закрываем соединения (close)
        client_socket.close()
        server_socket.close()
        print("Соединения закрыты")


if __name__ == "__main__":
    main()
