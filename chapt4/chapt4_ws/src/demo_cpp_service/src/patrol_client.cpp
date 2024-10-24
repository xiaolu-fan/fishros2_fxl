#include "rclcpp/rclcpp.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include "chrono"

using namespace std::chrono_literals;
using namespace std::placeholders;
using Patrol = chapt4_interfaces::srv::Patrol;
class PatrolClient : public rclcpp::Node
{
public:
    PatrolClient(const std::string node_name) : Node(node_name)
    {
        RCLCPP_INFO(this->get_logger(), "启动了一个节点，节点名称是:%s", node_name.c_str());
        client_ = this->create_client<Patrol>("patrol");
        timer_ = this->create_wall_timer(10s, std::bind(&PatrolClient::timer_callback, this));
        srand(time(NULL)); // 初始化随机数种子，使用当前时间作为种子；
    }

private:
    rclcpp::Client<Patrol>::SharedPtr client_;
    rclcpp::TimerBase::SharedPtr timer_;

private:
    void client_callback(const rclcpp::Client<Patrol>::SharedFuture result_future)
    {
        auto response = result_future.get();
        if (response->result == Patrol::Response::FALT)
        {
            RCLCPP_INFO(this->get_logger(), "目标处理点失败");
        }
        else if (response->result == Patrol::Response::SUCCESS)
        {
            RCLCPP_INFO(this->get_logger(), "目标处理点成功");
        }
    }
    void timer_callback()
    {
        while (!client_->wait_for_service(std::chrono::seconds(1)))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_INFO(this->get_logger(), "等待服务端上线过程中被打断……");
                return;
            }
            RCLCPP_INFO(this->get_logger(), "等待服务端上线中");
        }

        auto request = std::make_shared<Patrol::Request>();
        request->target_x = rand() % 15;
        request->target_y = rand() % 15;
        RCLCPP_INFO(this->get_logger(), "请求巡逻:(%f,%f)", request->target_x, request->target_y);
        client_->async_send_request(request, std::bind(&PatrolClient::client_callback, this, _1));
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PatrolClient>("turtle_patrol_client");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}