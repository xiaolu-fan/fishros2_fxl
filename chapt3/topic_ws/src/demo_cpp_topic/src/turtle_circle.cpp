#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include <chrono>

using namespace std::chrono_literals;
class TurtleCircle : public rclcpp::Node
{
public:
    TurtleCircle(const std::string node_name) : Node(node_name)
    {
        RCLCPP_INFO(this->get_logger(), "启动一个节点，名字是：%s", node_name.c_str());
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        timer_ = this->create_wall_timer(1000ms, std::bind(&TurtleCircle::timer_callback, this));
    }

private:
    rclcpp::TimerBase::SharedPtr timer_;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;

private:
    void timer_callback()
    {
        geometry_msgs::msg::Twist msg;
        msg.linear.x = 1.0;
        msg.angular.z = 0.5;
        pub_->publish(msg);
    }
};

int main(int argc, char **argv)
{
    rclcpp::init(argc, argv);
    auto node = std::make_shared<TurtleCircle>("turtle_square");

    rclcpp::spin(node);
    rclcpp::shutdown();
    return 0;
}