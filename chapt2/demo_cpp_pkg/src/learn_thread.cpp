#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <cpp-httplib/httplib.h>

class DownLoad
{
public:
    void download(const std::string &host, const std::string &path,
                  const std::function<void(const std::string &, const std::string &)> &callback)
    {
        std::cout << "线程ID:" << std::this_thread::get_id() << std::endl;
        httplib::Client client(host);
        auto response = client.Get(path);
        if (response && response->status == 200)
        {
            callback(path, response->body);
        }
        else
        {
            std::cout << "下载错误" << std::endl;
            std::cout << "status:" << response->status << std::endl;
        }
    }

    void start_download(const std::string &host, const std::string &path,
                        const std::function<void(const std::string &, const std::string &)> &callback)
    {
        using namespace std::placeholders;
        auto download_func = std::bind(&DownLoad::download, this, _1, _2, _3);
        std::thread download_thread(download_func, host, path, callback);
        download_thread.detach();
    }
};

int main(int argc, char **agrv)
{
    DownLoad d;
    auto callback_func = [](const std::string &path, const std::string &res) -> void
    { std::cout << "下载完成：" << path << " 共 " << res.length() << "字 , 内容为:" << res.substr(0, 5) << std::endl; };

    d.start_download("http://0.0.0.0:8000", "/nove11.txt", callback_func);
    d.start_download("http://0.0.0.0:8000", "/nove12.txt", callback_func);
    d.start_download("http://0.0.0.0:8000", "/nove13.txt", callback_func);

    std::this_thread::sleep_for(std::chrono::microseconds(1000 * 10));

    return 0;
}