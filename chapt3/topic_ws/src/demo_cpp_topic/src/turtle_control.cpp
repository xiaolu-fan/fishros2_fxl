#include "rclcpp/rclcpp.hpp"
#include "geometry_msgs/msg/twist.hpp"
#include "turtlesim/msg/pose.hpp"

class TurtleControl : public rclcpp::Node
{
public:
    TurtleControl(const std::string node_name) : Node(node_name)
    {
        RCLCPP_INFO(this->get_logger(), "启动一个节点,名字是：%s", node_name.c_str());
        pub_ = this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);
        sub_ = this->create_subscription<turtlesim::msg::Pose>("turtle1/pose", 10, std::bind(&TurtleControl::on_pose_received, this, std::placeholders::_1));
    }

private:
    void on_pose_received(const turtlesim::msg::Pose::SharedPtr pose)
    {
        double cur_x = pose->x;
        double cur_y = pose->y;
        RCLCPP_INFO(this->get_logger(), "当前位置：%f, %f", cur_x, cur_y);
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

private:
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr pub_;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_;

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