#include "rclcpp/rclcpp.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include "chrono"
#include "rcl_interfaces/msg/parameter.hpp"
#include "rcl_interfaces/msg/parameter_type.hpp"
#include "rcl_interfaces/msg/parameter_value.hpp"
#include "rcl_interfaces/srv/set_parameters.hpp"

using namespace std::chrono_literals;
using namespace std::placeholders;
using Patrol = chapt4_interfaces::srv::Patrol;
using SetP = rcl_interfaces::srv::SetParameters;
class PatrolClient : public rclcpp::Node
{
public:
    PatrolClient(const std::string node_name) : Node(node_name)
    {
        RCLCPP_INFO(this->get_logger(), "启动了一个节点，节点名称是:%s", node_name.c_str());
        client_ = this->create_client<Patrol>("patrol");
        timer_ = this->create_wall_timer(5s, std::bind(&PatrolClient::timer_callback, this));
        srand(time(NULL)); // 初始化随机数种子，使用当前时间作为种子；
    }

    void update_server_param_k(double k)
    {
        auto param = rcl_interfaces::msg::Parameter();
        param.name = "k"; // 更改参数名，即返回失败"kk"，kk没有声明；
        auto param_value = rcl_interfaces::msg::ParameterValue();
        param_value.type = rcl_interfaces::msg::ParameterType::PARAMETER_DOUBLE;
        param_value.double_value = k;
        param.value = param_value;

        auto response = call_set_parameters(param);
        if (response == nullptr)
        {
            RCLCPP_WARN(this->get_logger(), "参数设置失败");
            return;
        }
        else
        {
            for (auto res : response->results)
            {
                if (res.successful)
                {
                    RCLCPP_INFO(this->get_logger(), "client参数 k 已经修改为 %f", k);
                }
                else
                {
                    RCLCPP_WARN(this->get_logger(), "参数 k 更改失败，失败原因%s", res.reason.c_str());
                }
            }
        }
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

    std::shared_ptr<SetP::Response> call_set_parameters(rcl_interfaces::msg::Parameter &parameter)
    {
        auto param_client = this->create_client<SetP>("/turtle_control/set_parameters");
        while (!param_client->wait_for_service(std::chrono::seconds(1)))
        {
            if (!rclcpp::ok())
            {
                RCLCPP_INFO(this->get_logger(), "等待参数服务端上线中被打断……");
                return nullptr;
            }
            RCLCPP_INFO(this->get_logger(), "等待参数服务端上线中……");
        }
        auto request = std::make_shared<SetP::Request>();
        request->parameters.push_back(parameter);
        auto future = param_client->async_send_request(request);
        rclcpp::spin_until_future_complete(this->get_node_base_interface(), future);
        auto response = future.get();
        return response;
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<PatrolClient>("turtle_patrol_client");
    node->update_server_param_k(3.5);
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}