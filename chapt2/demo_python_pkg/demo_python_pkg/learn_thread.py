import threading
import requests

class DownLoad:
    def download(self,url,callback):
        print(f'线程：{threading.get_ident()},开始下载:{url}')
        response = requests.get(url)
        response.encoding = 'UTF-8'
        print(f'内容：{response.text}')
        callback(url,response.text)
        
    def start_download(self,url,callback):
        thread = threading.Thread(target=self.download,args=(url,callback))
        thread.start()
        
def download_finish_callback(url,result):
        print(f'{url}:下载完成，共{len(result)}字，内容为：{result[:5]}…')
        
def main():
    d = DownLoad()
    d.start_download('http://0.0.0.0:8000/nove11.txt',download_finish_callback)
    d.start_download('http://0.0.0.0:8000/nove12.txt',download_finish_callback)
    d.start_download('http://0.0.0.0:8000/nove13.txt',download_finish_callback)