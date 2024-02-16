import os
import telnetlib
import subprocess
import time
import select
import socket

def send_chunked_request(host, port, request_data):
	response = ""
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((host, int(port)))
		for chunk in request_data.split('\n'):
			if chunk:
				chunk_data = chunk + "\r\n"
				chunk_size_header = f"{len(chunk_data):X}\r\n"
				s.sendall(chunk_size_header.encode('utf-8'))
				s.sendall(chunk_data.encode('utf-8'))
				s.sendall(b"\r\n")
				time.sleep(0.5)  # チャンク間の遅延
		s.sendall(b"0\r\n\r\n")  # 終了チャンク
		
		# レスポンスの受信
		s.settimeout(5.0)  # タイムアウトを設定
		while True:
			try:
				data = s.recv(32768)
				if not data:
					break
				response += data.decode('utf-8')
			except socket.timeout:
				break  # タイムアウトまたは他の理由で読み取り終了
	return response

def send_request(host, port, request_data, is_chunked=False):
	response = ""
	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((host, int(port)))
		s.sendall(request_data.encode('utf-8'))
		
		# レスポンスの受信
		s.settimeout(5.0)  # タイムアウトを設定
		while True:
			try:
				data = s.recv(32768)
				response += data.decode('utf-8')
				if not data:
					break
			except socket.timeout:
				break  # タイムアウトまたは他の理由で読み取り終了
	return response


def save_response_to_file(response, filename):
	with open(filename, 'w') as file:
		file.write(response)

def compare_responses(file1, file2):
	print("compare")
	result = subprocess.run(['diff', file1, file2], capture_output=True, text=True)
	os.remove(file1)
	os.remove(file2)
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


def start_myserver(server_command, config_path):
	"""サーバーを指定された設定ファイルで起動する"""
	process = subprocess.Popen(server_command.split() + [config_path],
								stdout=subprocess.PIPE,
								stderr=subprocess.PIPE)
	return process

def start_nginxserver(server_command):
	"""サーバーを指定された設定ファイルで起動する"""
	process = subprocess.Popen(server_command.split(),
								stdout=subprocess.PIPE,
								stderr=subprocess.PIPE)
	return process

def stop_server(process):
	"""サーバープロセスを停止する"""
	process.terminate()
	try:
		process.wait(timeout=5)
	except subprocess.TimeoutExpired:
		process.kill()

def run_tests(tests_dir):
	"""テストを実行し、結果を返す"""
	total_tests = successful_tests = 0
	for file in os.listdir(tests_dir):
		file_path = os.path.join(tests_dir, file)
		if os.path.isfile(file_path):
			total_tests += 1
			if process_request_file(file_path):
				successful_tests += 1
	return total_tests, successful_tests

def main():
	config_dir = './testconf'
	tests_dir = './testdatafile'
	nginx_config_dir = './nginxconf'

	for config_file in os.listdir(config_dir):
		config_path = os.path.join(config_dir, config_file)
		nginx_config_path = os.path.join(nginx_config_dir, config_file)  # nginx用の設定ファイルパス
		destination_path = '/etc/nginx/conf.d/default.conf'
		try:
			with open(nginx_config_path, 'r') as src_file:
				config_content = src_file.read()
			with open(destination_path, 'w') as dest_file:
				dest_file.write(config_content)
			print("ファイルの内容が正常にコピーされました。")
		except IOError as e:
			print(f"ファイルのコピー中にエラーが発生しました: {e}")
		
		# Webservを起動
		webserv_process = start_myserver("./webserv", config_path)
		# Nginxを起動（nginxの設定ファイルパスが必要な場合は適宜修正）
		nginx_process = start_nginxserver("sudo nginx")
		
		# テストを実行
		total_tests, successful_tests = run_tests(tests_dir)
		print(f"Total tests conducted: {total_tests}")
		print_colored(f"Successful tests: {successful_tests}", "green" if successful_tests == total_tests else "red")
		
		# サーバーを停止
		stop_server(webserv_process)
		stop_server(nginx_process)

		print("\n" + "="*50 + "\n")

if __name__ == "__main__":
	main()
