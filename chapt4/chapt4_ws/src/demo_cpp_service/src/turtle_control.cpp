#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"
#include "chapt4_interfaces/srv/patrol.hpp"
#include "rcl_interfaces/msg/set_parameters_result.hpp"

using SetParametersResult = rcl_interfaces::msg::SetParametersResult;
using Patrol = chapt4_interfaces::srv::Patrol;
using namespace std::placeholders;
class TurtleControl : public rclcpp::Node
{
public:
    TurtleControl(const std::string node_name) : Node(node_name)
    {
        RCLCPP_INFO(this->get_logger(), "启动一个节点,名字是：%s", node_name.c_str());

        this->declare_parameter("k", 1.0);
        this->declare_parameter("max_speed", 3.0);
        this->get_parameter("k", this->k_);
        this->get_parameter("max_speed", this->max_speed_);

        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        sub_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, std::bind(&TurtleControl::on_pose_received, this, std::placeholders::_1));
        patrol_service_ = this->create_service<Patrol>("patrol", std::bind(&TurtleControl::service_callback, this, _1, _2));
        parameters_callback_handle_ = this->add_on_set_parameters_callback(std::bind(&TurtleControl::Parameters_callback, this, _1));
    }

private:
    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {
        double cur_x = pose->x;
        double cur_y = pose->y;
        // RCLCPP_INFO(this->get_logger(), "当前位置：%f, %f", cur_x, cur_y);
        double distance = std::sqrt(std::pow((target_x_ - cur_x), 2) + std::pow((target_y_ - cur_y), 2));
        double angle_err = std::atan2((target_y_ - cur_y), (target_x_ - cur_x)) - pose->theta;

        auto msg = geometry_msgs::msg::Twist();
        // 控制策略
        if (distance > 0.1)
        {
            if (fabs(angle_err) > 0.2)
            {
                msg.angular.z = fabs(angle_err);
            }
            else
            {
                msg.linear.x = k_ * distance;
            }
        }
        if (msg.linear.x > max_speed_)
        {
            msg.linear.x = max_speed_;
        }

        // 发布速度
        pub_->publish(msg);
    }

    void service_callback(const std::shared_ptr<Patrol::Request> request, const std::shared_ptr<Patrol::Response> response)
    {
        if ((0 < request->target_x && request->target_x < 12.0f) && (0 < request->target_y && request->target_y < 12.0f))
        {
            target_x_ = request->target_x;
            target_y_ = request->target_y;
            response->result = Patrol::Response::SUCCESS;
        }
        else
        {
            response->result = Patrol::Response::FALT;
        }
    }

    SetParametersResult Parameters_callback(const std::vector<rclcpp::Parameter> &params)
    {
        for (auto param : params)
        {
            RCLCPP_INFO(this->get_logger(), "server根新参数 %s 值为 %f", param.get_name().c_str(), param.as_double());
            if (param.get_name() == "k")
            {
                k_ = param.as_double();
            }
            else if (param.get_name() == "max_speed")
            {
                max_speed_ = param.as_double();
            }
        }

        auto res = SetParametersResult();
        res.successful = true;
        return res;
    }

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;
    rclcpp::Service<Patrol>::SharedPtr patrol_service_;
    OnSetParametersCallbackHandle::SharedPtr parameters_callback_handle_;

    double target_x_{1.0};
    double target_y_{1.0};
    double k_{1.0};
    double max_speed_{3.0};
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleControl>("turtle_control");
    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}