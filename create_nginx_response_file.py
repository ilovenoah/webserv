import os
import subprocess
import time
import socket

def send_raw_request_from_file(file_path):
	# ファイルからホスト、ポート、リクエストを読み込む
	with open(file_path, 'r') as file:
		content = file.read()

	# 各セクションを分割
	sections = content.split('\n\n', 2)
	host_port_section = sections[0].split('\n')
	request_section = sections[1] if len(sections) > 1 else ''
	request_section += "\r\n\r\n"

	# ホストとポートを抽出
	host = ''
	port = 80
	for line in host_port_section:
		if line.startswith('host,'):
			host = line.split(',', 1)[1].strip()
		elif line.startswith('port,'):
			port = int(line.split(',', 1)[1].strip())

	with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
		s.connect((host, port))
		s.sendall(request_section.encode())
		response = ''
		while True:
			part = s.recv(100000)
			if not part:
				break
			response += part.decode()

	return response


def save_response_to_file(response, filename, dir):
	file_path = os.path.join(dir, filename)
	if not os.path.exists(dir):
		os.makedirs(dir)
	with open(file_path, 'w') as file:
		file.write(response)

def process_request_file(file_path, dir, config_file):
	response_nginx = send_raw_request_from_file(file_path)

	nginx_file = config_file.split('.')[0] + os.path.basename(file_path)
	
	save_response_to_file(response_nginx, nginx_file, dir)

def start_nginxserver(server_command):
	"""サーバーを指定された設定ファイルで起動する"""
	process = subprocess.Popen(server_command.split(),
								stdout=subprocess.PIPE,
								stderr=subprocess.PIPE)
	print(process.pid)
	return process

def stop_nginx_server(process):
	"""Nginxサーバープロセスを停止するためのコマンドを実行する"""
	command = "/usr/local/nginx/sbin/nginx -s stop"
	killcommand = "kill " + str(process.pid)
	try:
		subprocess.run(command, check=True, shell=True)
		subprocess.run(killcommand, check=True, shell=True)
		print("Nginx server has been stopped successfully.")
	except subprocess.CalledProcessError as e:
		print(f"Failed to stop Nginx server: {e}")


def main():
	nginx_config_dir = './nginxconf'
	tests_dir = './testdatafile'
	nginxresultdir = './nginxresultdir'
	for config_file in os.listdir(nginx_config_dir):
		nginx_config_path = os.path.join(nginx_config_dir, config_file)
		destination_path = '/usr/local/nginx/conf.d/default.conf'
		try:
			with open(nginx_config_path, 'r') as src_file:
				config_content = src_file.read()
			with open(destination_path, 'w') as dest_file:
				dest_file.write(config_content)
			print("ファイルの内容が正常にコピーされました。")
		except IOError as e:
			print(f"ファイルのコピー中にエラーが発生しました: {e}")
		nginx_process = start_nginxserver("/usr/local/nginx/sbin/nginx")
		time.sleep(0.5)
		for testfile in os.listdir(tests_dir):
			file_path = os.path.join(tests_dir, testfile)
			process_request_file(file_path, nginxresultdir, config_file)
		stop_nginx_server(nginx_process)
		time.sleep(0.5)

if __name__ == "__main__":
	main()