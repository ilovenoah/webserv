import os
import telnetlib
import subprocess
import time
import select

def send_chunked_request(host, port, request_data):
    with telnetlib.Telnet(host, port) as tn:
        for chunk in request_data.split('\n'):
            size = f"{len(chunk):X}\r\n"
            tn.write(size.encode('utf-8'))
            tn.write(chunk.encode('utf-8'))
            tn.write(b"\r\n")
            time.sleep(0.5)  # チャンク間の遅延
        tn.write(b"0\r\n\r\n")  # 終了チャンク
        response = tn.read_all().decode('utf-8')
    return response

def send_request(host, port, request_data, is_chunked=False):
    with telnetlib.Telnet(host, port) as tn:
        tn.write(request_data.encode('utf-8'))

        response = []
        while True:
            ready_to_read, _, _ = select.select([tn], [], [], 5.0)  # 5秒のタイムアウト
            if ready_to_read:
                data = tn.read_very_eager().decode('utf-8')
                if not data:
                    break
                response.append(data)
            else:
                break  # タイムアウトまたは他の理由で読み取り終了

        return ''.join(response)

def save_response_to_file(response, filename):
    with open(filename, 'w') as file:
        file.write(response)

def compare_responses(file1, file2):
    print("compare")
    result = subprocess.run(['diff', file1, file2], capture_output=True, text=True)
    return result.stdout

def print_colored(text, color):
    colors = {"green": "\033[92m", "red": "\033[91m", "reset": "\033[0m"}
    print(f"{colors[color]}{text}{colors['reset']}")

def process_request_file(file_path):
    with open(file_path, 'r') as file:
        request_data = file.read()

    # チャンク化されているかどうかをチェック
    is_chunked = "chunked" in request_data.lower()

    print("before webserv")
    response_webserv = send_request('localhost', '2020', request_data, is_chunked)
    print("before nginx")
    response_nginx = send_request('localhost', '9090', request_data, is_chunked)

    webserv_file = f'response_webserv_{os.path.basename(file_path)}.txt'
    nginx_file = f'response_nginx_{os.path.basename(file_path)}.txt'
    save_response_to_file(response_webserv, webserv_file)
    save_response_to_file(response_nginx, nginx_file)

    diff_result = compare_responses(webserv_file, nginx_file)

    if diff_result == '':
        print_colored(f"OK: No differences found for {file_path}.", "green")
        return True
    else:
        print_colored(f"KO: Differences found for {file_path}.", "red")
        print(diff_result)
        return False

requests_dir = './testdatafile'
total_tests = 0
successful_tests = 0

for file in os.listdir(requests_dir):
    file_path = os.path.join(requests_dir, file)
    if os.path.isfile(file_path):
        total_tests += 1
        if process_request_file(file_path):
            successful_tests += 1

print(f"\nTotal tests conducted: {total_tests}")
print_colored(f"Successful tests: {successful_tests}", "green" if successful_tests == total_tests else "red")