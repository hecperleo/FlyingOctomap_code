#include <ros/ros.h>
#include <ltStar_temp.h>
#include <octomap_msgs/Octomap.h>
#include <octomap_msgs/conversions.h>
#include <visualization_msgs/Marker.h>
#include <marker_publishing_utils.h>
#include <std_srvs/Empty.h>

#define SAVE_CSV 1

namespace LazyThetaStarOctree
{
	std::string folder_name;


    octomap::OcTree* octree;
	ros::Publisher ltstar_reply_pub;
	ros::Publisher marker_pub;
		
	bool octomap_init;
	bool publish_free_corridor_arrows;

	bool check_status(lazy_theta_star_msgs::LTStarNodeStatus::Request  &req,
        lazy_theta_star_msgs::LTStarNodeStatus::Response &res)
	{
		res.is_accepting_requests = octomap_init;
	  	return true;
	}

	void ltstar_callback(const lazy_theta_star_msgs::LTStarRequest::ConstPtr& path_request)
	{
		rviz_interface::publish_deleteAll(marker_pub);
		lazy_theta_star_msgs::LTStarReply reply;
		reply.waypoint_amount = 0;
		reply.success = false;
		if(octomap_init)
		{
			// std::stringstream ss;
			// ss << folder_name << "/(" << path_request->start.x << "; " << path_request->start.y << "; " << path_request->start.z << ")_(" 
			// 	<<  path_request->goal.x << "; " << path_request->goal.y << "; " << path_request->goal.z << ").bt";
			// octree->writeBinary(ss.str());
			// ROS_INFO_STREAM("[LTStar] Request message " << *path_request);
			// if(path_request->request_id > 5)
			// {
			// 	publish_free_corridor_arrows = true;
			// }
			// else
			// {
			// 	publish_free_corridor_arrows = false;
			// }
			LazyThetaStarOctree::processLTStarRequest(*octree, *path_request, reply, marker_pub, false);
			if(reply.waypoint_amount == 1)
			{
				ROS_ERROR_STREAM("[LTStar] The resulting path has only one waypoint. Request: " << *path_request);
			}
			// ROS_INFO_STREAM("[LTStar] Reply " << reply);

			// octree->writeBinary(folder_name + "/octree_after_processing_request.bt");
		}
		else
		{
			reply.success=false;
			reply.request_id = path_request->request_id;
			reply.waypoint_amount = 0;
		}
		ltstar_reply_pub.publish(reply);

		visualization_msgs::MarkerArray waypoint_array;
		visualization_msgs::MarkerArray arrow_array;
		visualization_msgs::Marker marker_temp;
		// Publish to rviz
		for (int i = 0; i < reply.waypoint_amount; ++i)
		{

			octomath::Vector3 candidate (reply.waypoints[i].x, reply.waypoints[i].y, reply.waypoints[i].z);
			std::unordered_set<std::shared_ptr<octomath::Vector3>> neighbors;
	        octomap::OcTreeKey key = octree->coordToKey(candidate);
	        double depth = getNodeDepth_Octomap(key, *octree);
	        double side_length = findSideLenght(*octree, depth);
	        octomath::Vector3 cell_center = octree->keyToCoord(key, depth);
	        if( cell_center.distance(candidate) < 0.001 )
	        {
		        rviz_interface::build_waypoint(candidate, side_length, (0.3*i)/reply.waypoint_amount, i, marker_temp);
		        waypoint_array.markers.push_back( marker_temp );
	        }
	        if(i !=0)
	        {

				visualization_msgs::Marker marker_temp;
				octomath::Vector3 prev_candidate (reply.waypoints[i-1].x, reply.waypoints[i-1].y, reply.waypoints[i-1].z);
				rviz_interface::build_arrow_path(candidate, prev_candidate, i, marker_temp);
	        	// ROS_WARN_STREAM("[LTStar] " << i << " Publish arrow from " << candidate << " to " << prev_candidate << marker_temp);
				arrow_array.markers.push_back( marker_temp );
	        }
		}
		marker_pub.publish(arrow_array);
		marker_pub.publish(waypoint_array);
	}
	
	void octomap_callback(const octomap_msgs::Octomap::ConstPtr& octomapBinary){
		delete octree;
		octree = (octomap::OcTree*)octomap_msgs::binaryMsgToMap(*octomapBinary);
		octomap_init = true;
	}
}

int main(int argc, char **argv)
{
	LazyThetaStarOctree::folder_name = "/ros_ws/src/data/current";
#ifdef SAVE_CSV
	std::ofstream csv_file;
	csv_file.open ("/ros_ws/src/data/current/lazyThetaStar_computation_time.csv", std::ofstream::app);
	csv_file << "computation_time_millis,path_lenght_straight_line_meters,path_lenght_total_meters,has_obstacle" << std::endl;
	csv_file.close();
#endif
	LazyThetaStarOctree::publish_free_corridor_arrows = true;
	ros::init(argc, argv, "ltstar_async_node");
	ros::NodeHandle nh;
	ros::ServiceServer ltstar_status_service = nh.advertiseService("ltstar_status", LazyThetaStarOctree::check_status);
	ros::Subscriber octomap_sub = nh.subscribe<octomap_msgs::Octomap>("/octomap_binary", 10, LazyThetaStarOctree::octomap_callback);
	ros::Subscriber ltstars_sub = nh.subscribe<lazy_theta_star_msgs::LTStarRequest>("ltstar_request", 10, LazyThetaStarOctree::ltstar_callback);
	LazyThetaStarOctree::ltstar_reply_pub = nh.advertise<lazy_theta_star_msgs::LTStarReply>("ltstar_reply", 10);
	LazyThetaStarOctree::marker_pub = nh.advertise<visualization_msgs::MarkerArray>("ltstar_path", 1);

	ros::spin();
}