#include <QApplication>
#include <QLabel>
#include <QString>

#include "rclcpp/rclcpp.hpp"
#include "status_interfaces/msg/system_status.hpp"

using SystemStatus = status_interfaces::msg::SystemStatus;
class SysStatusDisplay : public rclcpp::Node
{
public:
    SysStatusDisplay(const std::string node_name) : Node(node_name)
    {
        RCLCPP_INFO(this->get_logger(), "启动了一个节点，节点名：%s", node_name.c_str());
        sub_ = this->create_subscription<SystemStatus>("sys_status", 10, std::bind(&SysStatusDisplay::call_back, this, std::placeholders::_1));
        label_ = new QLabel(get_qstr_from_msg(std::make_shared<SystemStatus>()));
        label_->show();
    }

private:
    rclcpp::Subscription<SystemStatus>::SharedPtr sub_;
    QLabel *label_;

private:
    void call_back(const SystemStatus::SharedPtr msg)
    {
        label_->setText(get_qstr_from_msg(msg));
    }

    QString get_qstr_from_msg(const SystemStatus::SharedPtr msg)
    {
        std::stringstream str;
        str << "==========系统状态可视化显示工具==========\n"
            << "数据时间：\t" << msg->stamp.sec << "\ts\n"
            << "用户名：\t" << msg->host_name << "\ts\n"
            << "CPU使用率:\t" << msg->cpu_percent << "\ts\n"
            << "内存使用率:\t" << msg->memory_percent << "\ts\n"
            << "内存总大小:\t" << msg->memory_total << "\ts\n"
            << "剩余有效内存:\t" << msg->memory_available << "\ts\n"
            << "网络发送量:\t" << msg->net_sent << "\ts\n"
            << "网络接受量:\t" << msg->net_recv << "\ts\n"
            << "========================================";

        return QString::fromStdString(str.str());
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    QApplication app(argc, argv);
    auto node = std::make_shared<SysStatusDisplay>("sys_status_display");
    std::thread spin_thread([&]() -> void
                            { rclcpp::spin(node); });
    spin_thread.detach();
    app.exec();
    rclcpp::shutdown();

    return 0;
}