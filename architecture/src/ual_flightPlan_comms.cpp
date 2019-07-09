//----------------------------------------------------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2018 Hector Perez Leon
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
// Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//----------------------------------------------------------------------------------------------------------------------

#include <ual_flightPlan_comms.h>

namespace flight_plan_comms {

UALCommunication::UALCommunication() : nh_(), pnh_("~") {
    // Parameters
    pnh_.getParam("uav_id", uav_id_);
    pnh_.getParam("path", init_path_name_);
    pnh_.getParam("pkg_name", pkg_name_);
    pnh_.getParam("reach_tolerance", reach_tolerance_);
    pnh_.getParam("generator_mode", generator_mode_);
    pnh_.getParam("take_off_height", take_off_height);


    // === Final data flow ===
    follower_ = upat_follower::Follower(uav_id_);
    sub_flight_plan_ = nh_.subscribe("/uav_" + std::to_string(uav_id_) + "/fligh_plan", 0, &UALCommunication::flightPlanCallback, this);
    comms_state = starting;
    flight_plan_received = false;
    // =======================




    // Subscriptions
    sub_pose_ = nh_.subscribe("/uav_" + std::to_string(uav_id_) + "/ual/pose", 0, &UALCommunication::ualPoseCallback, this);
    sub_state_ = nh_.subscribe("/uav_" + std::to_string(uav_id_) + "/ual/state", 0, &UALCommunication::ualStateCallback, this);
    sub_velocity_ = nh_.subscribe("/upat_follower/follower/uav_" + std::to_string(uav_id_) + "/output_vel", 0, &UALCommunication::velocityCallback, this);
    // Publishers
    pub_set_pose_ = nh_.advertise<geometry_msgs::PoseStamped>("/uav_" + std::to_string(uav_id_) + "/ual/set_pose", 1000);
    pub_set_velocity_ = nh_.advertise<geometry_msgs::TwistStamped>("/uav_" + std::to_string(uav_id_) + "/ual/set_velocity", 1000);
    // Services
    client_take_off_ = nh_.serviceClient<uav_abstraction_layer::TakeOff>("/uav_" + std::to_string(uav_id_) + "/ual/take_off");
    client_land_ = nh_.serviceClient<uav_abstraction_layer::Land>("/uav_" + std::to_string(uav_id_) + "/ual/land");
    client_visualize_ = nh_.serviceClient<upat_follower::Visualize>("/upat_follower/visualization/uav_" + std::to_string(uav_id_) + "/visualize");
    // Flags
    on_path_ = false;
    end_path_ = false;
    // Initialize path
    init_path_ = csvToPath("/" + init_path_name_ + ".csv");
    times_ = csvToVector("/times.csv");
    // Save data
    if (save_test_) {
        std::string pkg_name_path = ros::package::getPath(pkg_name_);
        folder_data_name_ = pkg_name_path + "/tests/splines";
    }
}

UALCommunication::~UALCommunication() {
}

nav_msgs::Path UALCommunication::constructPath(std::vector<double> _wps_x, std::vector<double> _wps_y, std::vector<double> _wps_z, std::string frame_id) 
{
    nav_msgs::Path out_path;
    std::vector<geometry_msgs::PoseStamped> poses(_wps_x.size());
    out_path.header.frame_id = frame_id;
    for (int i = 0; i < _wps_x.size(); i++) {
        poses.at(i).pose.position.x = _wps_x[i];
        poses.at(i).pose.position.y = _wps_y[i];
        poses.at(i).pose.position.z = _wps_z[i];
        poses.at(i).pose.orientation.x = 0;
        poses.at(i).pose.orientation.y = 0;
        poses.at(i).pose.orientation.z = 0;
        poses.at(i).pose.orientation.w = 1;
    }
    out_path.poses = poses;
    return out_path;
}

nav_msgs::Path UALCommunication::csvToPath(std::string _file_name) 
{
    nav_msgs::Path out_path;
    std::string pkg_name_path = ros::package::getPath(pkg_name_);
    std::string folder_name = pkg_name_path + "/config" + _file_name;
    std::fstream read_csv;
    read_csv.open(folder_name);
    std::vector<double> list_x, list_y, list_z;
    if (read_csv.is_open()) {
        while (read_csv.good()) {
            std::string x, y, z;
            double dx, dy, dz;
            getline(read_csv, x, ',');
            getline(read_csv, y, ',');
            getline(read_csv, z, '\n');
            std::stringstream sx(x);
            std::stringstream sy(y);
            std::stringstream sz(z);
            sx >> dx;
            sy >> dy;
            sz >> dz;
            list_x.push_back(dx);
            list_y.push_back(dy);
            list_z.push_back(dz);
        }
    }

    return constructPath(list_x, list_y, list_z, "uav_" + std::to_string(uav_id_) + "_home");
}

std::vector<double> UALCommunication::csvToVector(std::string _file_name) 
{
    std::vector<double> out_vector;
    std::string pkg_name_path = ros::package::getPath(pkg_name_);
    std::string folder_name = pkg_name_path + "/config" + _file_name;
    std::fstream read_csv;
    read_csv.open(folder_name);
    if (read_csv.is_open()) {
        while (read_csv.good()) {
            std::string x;
            double dx;
            getline(read_csv, x, '\n');
            std::stringstream sx(x);
            sx >> dx;
            out_vector.push_back(dx);
        }
    }

    return out_vector;
}

void UALCommunication::flightPlanCallback(const nav_msgs::Path::ConstPtr &_flight_plan) {
    flight_plan = *_flight_plan;
    flight_plan_received = true;
}

void UALCommunication::ualPoseCallback(const geometry_msgs::PoseStamped::ConstPtr &_ual_pose) {
    ual_pose_ = *_ual_pose;
}

void UALCommunication::ualStateCallback(const uav_abstraction_layer::State &_ual_state) {
    ual_state_.state = _ual_state.state;
    
    if(comms_state != finished_mission)
    {
        comms_state = take_off;
    }
}

void UALCommunication::velocityCallback(const geometry_msgs::TwistStamped &_velocity) {
    velocity_ = _velocity;
}


void UALCommunication::callVisualization() {
    upat_follower::Visualize visualize;
    visualize.request.init_path = init_path_;
    visualize.request.generated_path = target_path_;
    visualize.request.current_path = current_path_;
    client_visualize_.call(visualize);
}

void UALCommunication::runMission() {
    // static upat_follower::Follower follower_(uav_id_);

    uav_abstraction_layer::TakeOff take_off;
    uav_abstraction_layer::Land land;
    upat_follower::PreparePath prepare_path;
    upat_follower::PrepareTrajectory prepare_trajectory;
    if (target_path_.poses.size() < 1) {
        prepare_path.request.init_path = init_path_;
        prepare_path.request.generator_mode.data = 2;
        prepare_path.request.look_ahead.data = 1.2;
        prepare_path.request.cruising_speed.data = 1.0;
        target_path_ = follower_.preparePath(init_path_, generator_mode_, 0.4, 1.0);
    }

    Eigen::Vector3f current_p, path0_p, path_end_p;
    current_p = Eigen::Vector3f(ual_pose_.pose.position.x, ual_pose_.pose.position.y, ual_pose_.pose.position.z);
    path0_p = Eigen::Vector3f(target_path_.poses.front().pose.position.x, target_path_.poses.front().pose.position.y, target_path_.poses.front().pose.position.z);
    path_end_p = Eigen::Vector3f(target_path_.poses.back().pose.position.x, target_path_.poses.back().pose.position.y, target_path_.poses.back().pose.position.z);
    switch (ual_state_.state) {
        case 2:  // Landed armed
            if (!end_path_) {
                take_off.request.height = 12.5;
                take_off.request.blocking = true;
                client_take_off_.call(take_off);
            }
            break;
        case 3:  // Taking of
            break;
        case 4:  // Flying auto
            if (!end_path_) {
                if (!on_path_) {
                    if ((current_p - path0_p).norm() > reach_tolerance_ * 2) {
                        pub_set_pose_.publish(target_path_.poses.at(0));
                    } else if (reach_tolerance_ > (current_p - path0_p).norm() && !flag_hover_) {
                        pub_set_pose_.publish(target_path_.poses.front());
                        on_path_ = true;
                    }
                } else {
                    if (reach_tolerance_ * 2 > (current_p - path_end_p).norm()) {
                        pub_set_pose_.publish(target_path_.poses.back());
                        on_path_ = false;
                        end_path_ = true;
                    } else {
                        follower_.updatePose(ual_pose_);
                        velocity_ = follower_.getVelocity();
                        velocity_.twist.angular.z = 1;
                        pub_set_velocity_.publish(velocity_);
                        current_path_.header.frame_id = ual_pose_.header.frame_id;
                        current_path_.poses.push_back(ual_pose_);
                    }
                }
            } else {
                if (reach_tolerance_ * 2 > (current_p - path_end_p).norm() && (current_p - path_end_p).norm() > reach_tolerance_) {
                    pub_set_pose_.publish(target_path_.poses.back());
                } else {
                    land.request.blocking = true;
                    client_land_.call(land);
                }
            }
            break;
        case 5:  // Landing
            break;
    }
}

void UALCommunication::prepare()
{
    upat_follower::PreparePath prepare_path;
    upat_follower::PrepareTrajectory prepare_trajectory;
    if (target_path_.poses.size() < 1) {
        prepare_path.request.init_path = init_path_;
        prepare_path.request.generator_mode.data = 2;
        prepare_path.request.look_ahead.data = 1.2;
        prepare_path.request.cruising_speed.data = 1.0;
        target_path_ = follower_.preparePath(init_path_, generator_mode_, 0.4, 1.0);
    }

}

void UALCommunication::runMission_try2() {

    uav_abstraction_layer::TakeOff take_off;
    uav_abstraction_layer::Land land;

    prepare();
    switch (ual_state_.state) {
        case 2:  // Landed armed
            if (!end_path_) {
                take_off.request.height = 12.5;
                take_off.request.blocking = true;
                client_take_off_.call(take_off);
            }
            break;
        case 3:  // Taking of
            break;
        case 4:  // Flying auto
            followFlightPlan();
            break;
        case 5:  // Landing
            break;
    }
}

void UALCommunication::followFlightPlan()
{
    Eigen::Vector3f current_p, path0_p, path_end_p;
    current_p = Eigen::Vector3f(ual_pose_.pose.position.x, ual_pose_.pose.position.y, ual_pose_.pose.position.z);
    path0_p = Eigen::Vector3f(target_path_.poses.front().pose.position.x, target_path_.poses.front().pose.position.y, target_path_.poses.front().pose.position.z);
    path_end_p = Eigen::Vector3f(target_path_.poses.back().pose.position.x, target_path_.poses.back().pose.position.y, target_path_.poses.back().pose.position.z);
    if (!end_path_) {
        if (!on_path_) {
            if ((current_p - path0_p).norm() > reach_tolerance_ * 2) {
                pub_set_pose_.publish(target_path_.poses.at(0));
            } else if (reach_tolerance_ > (current_p - path0_p).norm() && !flag_hover_) {
                pub_set_pose_.publish(target_path_.poses.front());
                on_path_ = true;
            }
        } else {
            if (reach_tolerance_ * 2 > (current_p - path_end_p).norm()) {
                pub_set_pose_.publish(target_path_.poses.back());
                on_path_ = false;
                end_path_ = true;
            } else {
                follower_.updatePose(ual_pose_);
                velocity_ = follower_.getVelocity();
                velocity_.twist.angular.z = 1;
                pub_set_velocity_.publish(velocity_);
                current_path_.header.frame_id = ual_pose_.header.frame_id;
                current_path_.poses.push_back(ual_pose_);
            }
        }
    } else {
        if (reach_tolerance_ * 2 > (current_p - path_end_p).norm() && (current_p - path_end_p).norm() > reach_tolerance_) {
            pub_set_pose_.publish(target_path_.poses.back());
        } 
    }
}

void UALCommunication::executeFlightPlan() 
{
    switch(comms_state)
    {
        case take_off:
            {
            uav_abstraction_layer::TakeOff take_off_msg;
            take_off_msg.request.height = take_off_height;
            take_off_msg.request.blocking = true;
            client_take_off_.call(take_off_msg);
            }
            break;
        case wait_for_flight:
            break;
        case init_flight:

            break;
        case execute_flight:
            followFlightPlan();
            if (end_path_)
            {
                comms_state = wait_for_flight;
            }
            break;
        case finished_mission:
            uav_abstraction_layer::Land land;
            land.request.blocking = true;
            client_land_.call(land);
            break;
    }
}

}  // namespace upat_follower