
#include <marker_publishing_utils.h>

namespace rviz_interface
{
    void init_point(geometry_msgs::Point & point, float x, float y, float z)
    {
        point.x = x;
        point.y = y;
        point.z = z;
    }

    void push_segment(visualization_msgs::Marker & marker, geometry_msgs::Point & start, geometry_msgs::Point & end)
    {
        marker.points.push_back(start);
        marker.points.push_back(end);
    }

    void build_cube_wire(visualization_msgs::Marker & marker, octomath::Vector3 const& geofence_min, octomath::Vector3 const& geofence_max, octomath::Vector3 color = octomath::Vector3(1, 1, 1))
    {
        uint32_t shape = visualization_msgs::Marker::LINE_LIST; 
        // Set the frame ID and timestamp.  See the TF tutorials for information on these. 
        marker.header.frame_id = "/uav_1/odom"; 
        marker.header.stamp = ros::Time::now(); 
        marker.type = shape; 
        marker.action = visualization_msgs::Marker::ADD; 
        marker.scale.x = 0.1; 
        marker.scale.y = 0.1; 
        marker.scale.z = 0.1; 
        marker.color.r = color.x(); 
        marker.color.g = color.y(); 
        marker.color.b = color.z(); 
        marker.color.a = 1.0; 
        geometry_msgs::Point A, B, C, D, E, F, G, H; 
        init_point( A, geofence_min.x(), geofence_min.y(), geofence_min.z()); 
        init_point( B, geofence_max.x(), geofence_min.y(), geofence_min.z()); 
        init_point( C, geofence_min.x(), geofence_max.y(), geofence_min.z()); 
        init_point( D, geofence_max.x(), geofence_max.y(), geofence_min.z()); 
        init_point( E, geofence_min.x(), geofence_max.y(), geofence_max.z()); 
        init_point( F, geofence_max.x(), geofence_max.y(), geofence_max.z()); 
        init_point( G, geofence_min.x(), geofence_min.y(), geofence_max.z()); 
        init_point( H, geofence_max.x(), geofence_min.y(), geofence_max.z()); 
        push_segment(marker, A, B); 
        push_segment(marker, A, G); 
        push_segment(marker, A, C); 
        push_segment(marker, B, H); 
        push_segment(marker, B, D); 
        push_segment(marker, G, H); 
        push_segment(marker, H, F); 
        push_segment(marker, C, D); 
        push_segment(marker, C, E); 
        push_segment(marker, F, D); 
        push_segment(marker, G, E); 
        push_segment(marker, E, F); 
    }

	void publish_geofence (octomath::Vector3 const& geofence_min, octomath::Vector3 const& geofence_max, visualization_msgs::MarkerArray & marker_array)
    { 
        visualization_msgs::Marker marker; 
        // Set the frame ID and timestamp.  See the TF tutorials for information on these. 
        marker.ns = "geofence"; 
        marker.id = 20; 
        marker.lifetime = ros::Duration(); 
        build_cube_wire(marker, geofence_min, geofence_max);
        marker_array.markers.push_back(marker);
    } 
    void publish_safety_margin(geometry_msgs::Point const& frontier, double safety_margin, visualization_msgs::MarkerArray marker_array, int id) 
    { 
        visualization_msgs::Marker marker;
        octomath::Vector3  max = octomath::Vector3(frontier.x - safety_margin, frontier.y - safety_margin, frontier.z - safety_margin);
        octomath::Vector3  min = octomath::Vector3(frontier.x + safety_margin, frontier.y + safety_margin, frontier.z + safety_margin);
        marker.lifetime = ros::Duration();
        marker.ns = "frontier_safety_margin";
        marker.id = id;
        build_cube_wire(marker, min, max);
        marker_array.markers.push_back(marker);
    } 

    void publish_markerArray_safety_margin(geometry_msgs::Point const& frontier, double safety_margin, ros::Publisher const& marker_pub, int id)
    {   
        visualization_msgs::Marker marker;
        octomath::Vector3  max = octomath::Vector3(frontier.x - safety_margin, frontier.y - safety_margin, frontier.z - safety_margin);
        octomath::Vector3  min = octomath::Vector3(frontier.x + safety_margin, frontier.y + safety_margin, frontier.z + safety_margin);
        marker.lifetime = ros::Duration();
        marker.ns = "frontier_safety_margin";
        marker.id = id;
        build_cube_wire(marker, min, max);
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array); 
    }

    void publish_deleteAll(ros::Publisher const& marker_pub)
    {
        visualization_msgs::MarkerArray marker_array;
        visualization_msgs::Marker marker;
        marker.action = visualization_msgs::Marker::DELETEALL;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array); 
    }

    void publish_voxel_free_occupied(octomath::Vector3 & candidate, bool is_occupied, ros::Publisher const& marker_pub, int id, double size, visualization_msgs::Marker & marker)
    {
        uint32_t shape = visualization_msgs::Marker::CUBE;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "frontier_neighborhood";
        marker.id = id;
        marker.type = shape;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = candidate.x();
        marker.pose.position.y = candidate.y();
        marker.pose.position.z = candidate.z();
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = size;
        marker.scale.y = size;
        marker.scale.z = size;
        if(is_occupied)
        {
            marker.color.r = 1.0f;
            marker.color.g = 0.0f;
            marker.color.b = 0.0f;
            marker.color.a = 0.8;
        }
        else
        {
            marker.color.r = 0.0f;
            marker.color.g = 1.0f;
            marker.color.b = 0.0f;
            marker.color.a = 0.8;
        }
        marker.lifetime = ros::Duration(4);
    }

    void build_small_marker(octomath::Vector3 const& candidate, visualization_msgs::Marker & marker, float red, float green, float blue, std::string ns, int id, double size/* = 0.2f*/, double alpha/* = 1*/)
    {
        uint32_t shape = visualization_msgs::Marker::CUBE;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = ns;
        marker.id = id;
        marker.type = shape;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = candidate.x();
        marker.pose.position.y = candidate.y();
        marker.pose.position.z = candidate.z();
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = size;
        marker.scale.y = size;
        marker.scale.z = size;
        marker.color.r = red;
        marker.color.g = green;
        marker.color.b = blue;
        marker.color.a = alpha;
        marker.lifetime = ros::Duration();
    }

    void publish_small_marker(octomath::Vector3 const& candidate, ros::Publisher const& marker_pub, float red, float green, float blue, std::string ns, int id)
    {
        visualization_msgs::Marker marker;
        build_small_marker(candidate, marker, red, green, blue, ns, id);
        marker_pub.publish(marker);
    }

    void publish_current_position(octomath::Vector3 & candidate, visualization_msgs::MarkerArray marker_array)
    {
        float red = 0.0f;
        float green = 1.0f;
        float blue = 1.0f;
        visualization_msgs::Marker marker;
        build_small_marker(candidate, marker, red, green, blue, "current_position", 21);
        marker_array.markers.push_back(marker);
    }

    void build_startOPP_outsideGeofence(geometry_msgs::Point const& oppairStart, visualization_msgs::MarkerArray & marker_array, int oppair_id)
    {
        // 165,0,38
        std::string ns = "oppair_start_outside";
        double red   = 0.65;
        double green = 0;
        double blue  = 0.15;
        build_sphere_basic(oppairStart, marker_array, ns, red, green, blue, oppair_id);
    }

    void build_endOPP_outsideGeofence(geometry_msgs::Point const& oppairEnd, visualization_msgs::MarkerArray & marker_array, int oppair_id)
    {
        // 244,109,67
        std::string ns = "oppair_end_outside";
        double red   = 0.96;
        double green = 0.43;
        double blue  = 0.26;
        build_sphere_basic(oppairEnd, marker_array, ns, red, green, blue, oppair_id);
    }

    void build_stateManager(geometry_msgs::Point const& frontier,geometry_msgs::Point const& oppairStart, geometry_msgs::Point const& oppairEnd, geometry_msgs::Point const& start,   visualization_msgs::MarkerArray & marker_array, double diameter)
    {
        // oppair end   253,174,97     0.99    0.68    0.2
        // oppair start 255,255,191    1       1       0.2
        // uav_position 255,255,191    1       1       0.75
        // unknown      173,214,232    1       1       0.75
        std::string ns = "oppair_end";
        double red   = 0.99;
        double green = 0.68;
        double blue  = 0.2;
        build_sphere_basic(oppairEnd, marker_array, ns, red, green, blue);
        build_safetyzone_flybyEnd(oppairEnd, marker_array, diameter);

        ns = "oppair_start";
        red   = 1;
        green = 1;
        blue  = 0.2;
        build_sphere_basic(oppairStart, marker_array, ns, red, green, blue);
        build_safetyzone_flybyStart(oppairStart, marker_array, diameter);

        visualization_msgs::Marker marker;
        octomath::Vector3 start_v(oppairStart.x, oppairStart.y, oppairStart.z);
        octomath::Vector3 goal_v(oppairEnd.x, oppairEnd.y, oppairEnd.z);
        build_arrow_path(start_v, goal_v, 100, marker, 9, "oppair_path" );
        marker_array.markers.push_back(marker);

        ns = "uav_position";
        red   = 1;
        green = 1;
        blue  = 0.75;
        build_sphere_basic(start, marker_array, ns, red, green, blue);
        publish_startSafetyZone(start, marker_array, diameter);

        ns = "unknown";
        red   = 0.68;
        green = 0.84;
        blue  = 0.91;
        build_sphere_basic(frontier, marker_array, ns, red, green, blue);
    }
    
    void build_sphere(octomath::Vector3 & candidate, double size, int green_base, int marker_id, visualization_msgs::Marker & marker, int red_base, std::string ns)
    {   
        uint32_t shape = visualization_msgs::Marker::SPHERE;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = ns;
        marker.id = marker_id;
        marker.type = shape;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = candidate.x();
        marker.pose.position.y = candidate.y();
        marker.pose.position.z = candidate.z();
        marker.pose.orientation.w = 1.0;
        marker.scale.x = size;
        marker.scale.y = size;
        marker.scale.z = size;
        marker.color.r = red_base * 0.1;//0.9f;
        marker.color.g = green_base+0.4;
        marker.color.b = 1.0f;
        // ROS_WARN_STREAM("[RVIZ PUB] color " << marker.color.r << ", " << marker.color.g << ", " << marker.color.b << " i: " << waypoint_id);
        marker.color.a = 1.0f;
        
        marker.lifetime = ros::Duration();
    }

    void build_sphere_basic(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, std::string ns, double red, double green, double blue, int id /*= 30*/, double alpha /*= 1 */, double diameter /*= 0.2*/)
    {
        visualization_msgs::Marker marker;
        marker.color.r = red;
        marker.color.g = green;
        marker.color.b = blue;
        marker.ns = ns;
        marker.id = id;
        marker.type = visualization_msgs::Marker::SPHERE;
        // Basic
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position = candidate;
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = diameter;
        marker.scale.y = diameter;
        marker.scale.z = diameter;
        marker.color.a = alpha;
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }

    void build_safetyzone_flybyStart(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double diameter)
    {
        double red = 1.0f;
        double green = 1.0f;
        double blue = 0.38f;
        int id = 55;
        std::string ns = "safetyzone_flybyStart";
        double alpha = 0.4;
        build_sphere_basic(candidate, marker_array, ns, red, green, blue, id, alpha, diameter);
    }

    void build_safetyzone_flybyEnd(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double diameter)
    {
        double red = 1.0f;
        double green = 1.0f;
        double blue = 0.38f;
        int id = 57;
        std::string ns = "safetyzone_flybyEnd";
        double alpha = 0.4;
        build_sphere_basic(candidate, marker_array, ns, red, green, blue, id, alpha, diameter);
    }

    void build_safetyzone_unknown(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double diameter)
    {
        double red = 1.0f;
        double green = 1.0f;
        double blue = 0.38f;
        int id = 56;
        std::string ns = "safetyzone_unknown";
        double alpha = 0.4;
        build_sphere_basic(candidate, marker_array, ns, red, green, blue, id, alpha, diameter);
    }

    void publish_start(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array)
    {
        visualization_msgs::Marker marker;
        marker.color.r = 1.0f;
        marker.color.g = 1.0f;
        marker.color.b = 0.38f;
        marker.ns = "start";
        marker.id = 22;
        marker.type = visualization_msgs::Marker::SPHERE;
        // Basic
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position = candidate;
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.2f;
        marker.scale.y = 0.2f;
        marker.scale.z = 0.2f;
        marker.color.a = 1.0f;
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }

    void publish_startSafetyZone(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double diameter)
    {
        visualization_msgs::Marker marker;
        marker.color.r = 1.0f;
        marker.color.g = 1.0f;
        marker.color.b = 0.38f;
        marker.ns = "start_safetyZone";
        marker.id = 52;
        marker.type = visualization_msgs::Marker::SPHERE;
        // Basic
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position = candidate;
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = diameter;
        marker.scale.y = diameter;
        marker.scale.z = diameter;
        marker.color.a = 0.4f;
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }

    void publish_goal(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array)
    {
        visualization_msgs::Marker marker;
        marker.color.r = 1.0f;
        marker.color.g = 0.68f;
        marker.color.b = 0.38f;
        marker.ns = "goal";
        marker.id = 23;
        marker.type = visualization_msgs::Marker::SPHERE;
        // Basic
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position = candidate;
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.2f;
        marker.scale.y = 0.2f;
        marker.scale.z = 0.2f;
        marker.color.a = 1.0f;
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }

    void publish_goalSafetyZone(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double diameter)
    {
        visualization_msgs::Marker marker;
        marker.color.r = 1.0f;
        marker.color.g = 1.0f;
        marker.color.b = 0.38f;
        marker.ns = "goal_safetyZone";
        marker.id = 53;
        marker.type = visualization_msgs::Marker::SPHERE;
        // Basic
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position = candidate;
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = diameter;
        marker.scale.y = diameter;
        marker.scale.z = diameter;
        marker.color.a = 0.4f;
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }

    void publish_sensing_position(octomath::Vector3 const& position, int id, visualization_msgs::MarkerArray & marker_array)
    {
        float red = 0.0f;
        float green = 0.0f;
        float blue = 0.4f;
        // publish_small_marker(position, marker_pub,red,  green,  blue, "unknown_neighbor", 24);
        visualization_msgs::Marker marker;
        build_small_marker(position, marker, red, green, blue, "unknown_target", id, 0.1);
        marker_array.markers.push_back(marker);
    }

    void publish_start_voxel(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double size)
    {
        visualization_msgs::Marker marker;
        octomath::Vector3 candidate_vec3 (candidate.x, candidate.y, candidate.z);
        float red = 1.0f;
        float green = 1.0f;
        float blue = 0.0f;
        build_small_marker(candidate_vec3, marker, red, green,  blue, "start_voxel", 25, size, 0.3  );
        marker_array.markers.push_back(marker);
    }

    void publish_goal_voxel(geometry_msgs::Point const& candidate, visualization_msgs::MarkerArray & marker_array, double size)
    {
        visualization_msgs::Marker marker;
        octomath::Vector3 candidate_vec3 (candidate.x, candidate.y, candidate.z);
        float red = 1.0f;
        float green = 0.5f;
        float blue = 0.0f;
        build_small_marker(candidate_vec3, marker, red, green,  blue, "goal_voxel", 26, size, 0.3);
        marker_array.markers.push_back(marker);
    }

    void publish_random_important_cube(octomath::Vector3 const& candidate_vec3, ros::Publisher const& marker_pub)
    {
        visualization_msgs::Marker marker;
        float red = 0.0f;
        float green = 0.0f;
        float blue = 1.0f;
        build_small_marker(candidate_vec3, marker, red, green,  blue, "random_important_cube", 27, 0.2, 0.8 );
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }

    void publish_s(geometry_msgs::Point const& candidate, ros::Publisher const& marker_pub,
        visualization_msgs::MarkerArray & marker_array, int id, float size)
    {
        octomath::Vector3 candidate_vec3 (candidate.x, candidate.y, candidate.z);
        float red = 0.9f;
        float green = 0.5f;
        float blue = 1.0f;
        visualization_msgs::Marker marker;
        build_small_marker(candidate_vec3, marker, red,  green,  blue, "s", id, size, 0.8f);
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);   
    }

    void publish_rejected_neighbor(geometry_msgs::Point const& candidate, ros::Publisher const& marker_pub,
        visualization_msgs::MarkerArray & marker_array, int id, float size)
    {
        octomath::Vector3 candidate_vec3 (candidate.x, candidate.y, candidate.z);
        float red = 1.f;
        float green = 0.f;
        float blue = 0.f;
        visualization_msgs::Marker marker;
        build_small_marker(candidate_vec3, marker, red,  green,  blue, "rejected_neighors", id, size, 0.4f);
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);   
    }

    void publish_visible_neighbor(geometry_msgs::Point const& candidate, ros::Publisher const& marker_pub,
        visualization_msgs::MarkerArray & marker_array, int id, float size)
    {
        octomath::Vector3 candidate_vec3 (candidate.x, candidate.y, candidate.z);
        float red = 0.f;
        float green = 1.f;
        float blue = 0.f;
        visualization_msgs::Marker marker;
        build_small_marker(candidate_vec3, marker, red,  green,  blue, "usable_neighors", id, size, 0.4f);
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);  
    }

    void publish_closed(octomath::Vector3 const& candidate_vec3, ros::Publisher const& marker_pub,
        visualization_msgs::MarkerArray & marker_array, int id)
    {
        float red = 0.f;
        float green = 1.f;
        float blue = 0.f;
        float size = 0.1f;
        uint32_t shape = visualization_msgs::Marker::SPHERE;
        visualization_msgs::Marker marker;
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "closed";
        marker.id = id;
        marker.type = shape;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = candidate_vec3.x();
        marker.pose.position.y = candidate_vec3.y();
        marker.pose.position.z = candidate_vec3.z();
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = size;
        marker.scale.y = size;
        marker.scale.z = size;
        marker.color.r = red;
        marker.color.g = green;
        marker.color.b = blue;
        marker.color.a = 1;
        marker.lifetime = ros::Duration();

        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);   
    }

    void publish_frontier_marker(geometry_msgs::Point const& candidate, bool is_frontier, ros::Publisher const& marker_pub)
    {
        octomath::Vector3 candidate_vec3 (candidate.x, candidate.y, candidate.z);
        publish_frontier_marker(candidate_vec3, is_frontier, marker_pub);
    }
    
    void publish_frontier_marker(octomath::Vector3 const& candidate, bool is_frontier, ros::Publisher const& marker_pub)
    {
        uint32_t shape = visualization_msgs::Marker::CUBE;
        visualization_msgs::Marker marker;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "frontier_candidate";
        marker.id = 10;
        marker.type = shape;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = candidate.x();
        marker.pose.position.y = candidate.y();
        marker.pose.position.z = candidate.z();
        marker.pose.orientation.x = 0.0;
        marker.pose.orientation.y = 0.0;
        marker.pose.orientation.z = 0.0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.2;
        marker.scale.y = 0.2;
        marker.scale.z = 0.2;
        if(is_frontier)
        {
            marker.color.r = 0.0f;
            marker.color.g = 0.0f;
            marker.color.b = 1.0f;
            marker.color.a = 1.0;
        }
        else
        {
            marker.color.r = 0.5f;
            marker.color.g = 0.5f;
            marker.color.b = 0.75f;
            marker.color.a = 1.0;
        }
        marker.lifetime = ros::Duration();
        // ROS_WARN_STREAM("[RVIZ PUB] Frontier at " << marker.pose.position << ". Color: " << marker.color.r << ", " << marker.color.g << ", " << marker.color.b);
        marker_pub.publish(marker);
    }

    void build_arrow_path(octomath::Vector3 & start, octomath::Vector3 & goal, int request_id, visualization_msgs::Marker & marker, int series, std::string ns )
    {
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = ns;
        marker.id = request_id;
        marker.type = shape;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.01;
        marker.scale.y = 0.03;
        marker.scale.z = 0;
        marker.color.r = series * 0.1;
        marker.color.g = 1;
        marker.color.b = 0;
        marker.color.a = 1;
        marker.action = visualization_msgs::Marker::ADD;
        
        marker.lifetime = ros::Duration();
    }

    void publish_arrow_path_occupancyState(octomath::Vector3 const& start, octomath::Vector3 const& goal, visualization_msgs::MarkerArray & marker_array, bool free, int id)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.id = id;
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.1;
        marker.scale.y = 0.3;
        marker.scale.z = 0;
        if(free)
        {
            marker.ns = "corridor_free";
            marker.color.r = 0;
            marker.color.g = 255;   
        }
        else
        {
            marker.ns = "corridor_occupied";
            marker.color.r = 255;
            marker.color.g = 0;   
        }
        marker.color.b = 0;
        marker.color.a = 1;
        
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }


    void publish_arrow_path_visibility(octomath::Vector3 const& start, octomath::Vector3 const& goal, ros::Publisher const& marker_pub, bool free, int id)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.id = id;
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.1;
        marker.scale.y = 0.3;
        marker.scale.z = 0;
        if(free)
        {
            marker.ns = "has_visibility";
            marker.color.r = 0;
            marker.color.g = 255;   
        }
        else
        {
            marker.ns = "no_visibility";
            marker.color.r = 255;
            marker.color.g = 0;   
        }
        marker.color.b = 0;
        marker.color.a = 1;
        
        marker.lifetime = ros::Duration();
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }

    void publish_arrow_path_unreachable(octomath::Vector3 const& start, octomath::Vector3 const& goal, ros::Publisher const& marker_pub, int id)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "path_unreachable";
        marker.id = 4000 + id;
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.1;
        marker.scale.y = 0.3;
        marker.scale.z = 0;
        marker.color.r = 255;
        marker.color.g = 0;   
        marker.color.b = 0;
        marker.color.a = 1;
        
        marker.lifetime = ros::Duration();
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }


    void publish_arrow_path_father(octomath::Vector3 const& start, octomath::Vector3 const& goal, ros::Publisher const& marker_pub)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "parent";
        marker.id = 20000 + ( std::rand() % ( 9999 + 1 ) );;
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.01;
        marker.scale.y = 0.03;
        marker.scale.z = 0;
        marker.color.r = 0;
        marker.color.g = 255;   
        marker.color.b = 255;
        marker.color.a = 1;
        
        marker.lifetime = ros::Duration();
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }

    void publish_arrow_corridor(octomath::Vector3 const& start, octomath::Vector3 const& goal, ros::Publisher const& marker_pub)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.id = 50000 + ( std::rand() % ( 9999 + 1 ) );;
        marker.ns = "corridor_";
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.01;
        marker.scale.y = 0.03;
        marker.scale.z = 0;
        marker.color.r = 255;
        marker.color.g = 255;   
        marker.color.b = 255;
        marker.color.a = 1;
        
        marker.lifetime = ros::Duration();
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }


    void build_arrow_type(octomath::Vector3 const& start, octomath::Vector3 const& goal, visualization_msgs::MarkerArray & marker_array, int id, bool occupied)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.id = 500 + id;
        marker.ns = "corridor_";
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.02;
        marker.scale.y = 0.06;
        marker.scale.z = 0;
        if(!occupied)
        {
            marker.color.r = 0;
            marker.color.g = 0;   
            marker.color.b = 255;
        }
        else
        {
            marker.color.r = 255;
            marker.color.g = 0;   
            marker.color.b = 255;
        }
        marker.color.a = 1;
        marker.lifetime = ros::Duration();
        marker_array.markers.push_back(marker);
    }

    void publish_arrow_straight_line(geometry_msgs::Point const& start, geometry_msgs::Point const& goal, ros::Publisher const& marker_pub, bool found_safe_alternative, int id)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.id = 500+id;
        marker.ns = "straight_line";
        marker.type = shape;
        marker.points.push_back(start);
        marker.action = visualization_msgs::Marker::ADD;
        marker.points.push_back(goal);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.01;
        marker.scale.y = 0.03;
        marker.scale.z = 0;
        if(found_safe_alternative)
        {
            // lilac
            marker.color.r = 228;
            marker.color.g = 70;   
            marker.color.b = 255;
        }
        else
        {
            // white
            marker.color.r = 255;
            marker.color.g = 255;   
            marker.color.b = 255;
        }
        marker.color.a = 1;
        
        marker.lifetime = ros::Duration();
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }

    void publish_arrow_corridor_center(octomath::Vector3 const& start, octomath::Vector3 const& goal, ros::Publisher const& marker_pub)
    {
        visualization_msgs::Marker marker;
        uint32_t shape = visualization_msgs::Marker::ARROW;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.id = 60000 + ( std::rand() % ( 9999 + 1 ) );;
        marker.ns = "corridor_center";
        marker.type = shape;
        geometry_msgs::Point goal_point;
        goal_point.x = goal.x();
        goal_point.y = goal.y();
        goal_point.z = goal.z();
        marker.points.push_back(goal_point);
        marker.action = visualization_msgs::Marker::ADD;
        geometry_msgs::Point start_point;
        start_point.x = start.x();
        start_point.y = start.y();
        start_point.z = start.z();
        marker.points.push_back(start_point);
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 0.01;
        marker.scale.y = 0.03;
        marker.scale.z = 0;
        marker.color.r = 255;
        marker.color.g = 255;   
        marker.color.b = 0;
        marker.color.a = 1;
        marker.lifetime = ros::Duration();
        visualization_msgs::MarkerArray marker_array;
        marker_array.markers.push_back(marker);
        marker_pub.publish(marker_array);
    }

    // for purple is 9
    // Series can be 2, 3, 7, 8, 9
    void build_waypoint(octomath::Vector3 & candidate, double size, int color, int waypoint_id, visualization_msgs::Marker & marker, int series)
    {   
        uint32_t shape = visualization_msgs::Marker::CUBE;
        // Set the frame ID and timestamp.  See the TF tutorials for information on these.
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "lazy_theta_star_waypoint";
        marker.id = waypoint_id + (series * 1000);
        marker.type = shape;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = candidate.x();
        marker.pose.position.y = candidate.y();
        marker.pose.position.z = candidate.z();
        marker.pose.orientation.w = 1.0;
        marker.scale.x = size;
        marker.scale.y = size;
        marker.scale.z = size;
        marker.color.r = series * 0.1;//0.9f;
        marker.color.g = color+0.4;
        marker.color.b = 1.0f;
        // ROS_WARN_STREAM("[RVIZ PUB] color " << marker.color.r << ", " << marker.color.g << ", " << marker.color.b << " i: " << waypoint_id);
        marker.color.a = 0.8;
        
        marker.lifetime = ros::Duration();
    }

    // void build_neighbor_array(std::unordered_set<std::shared_ptr<octomath::Vector3>> & neighbors, visualization_msgs::MarkerArray & marker_array)
    // {
    //     visualization_msgs::Marker marker;
    //     int id = 6000;
    //     for (std::unordered_set<std::shared_ptr<octomath::Vector3>>::iterator i = neighbors.begin(); i != neighbors.end(); ++i)
    //     {
    //         double size = 0.5;
    //         // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    //         marker.header.frame_id = "/uav_1/odom";
    //         marker.header.stamp = ros::Time::now();
    //         marker.ns = "neighbor";
    //         marker.id =  id;
    //         marker.type = visualization_msgs::Marker::CUBE;
    //         marker.action = visualization_msgs::Marker::ADD;
    //         marker.pose.position.x = (*i)->x();
    //         marker.pose.position.y = (*i)->y();
    //         marker.pose.position.z = (*i)->z();
    //         marker.pose.orientation.w = 1.0;
    //         marker.scale.x = size;
    //         marker.scale.y = size;
    //         marker.scale.z = size;
    //         marker.color.r = 0.9f;
    //         marker.color.g = 0.4;
    //         marker.color.b = 1.0f;
    //         // ROS_WARN_STREAM("[RVIZ PUB] color " << marker.pose.position.x << ", " << marker.pose.position.y << ", " << marker.pose.position.z );
    //         marker.color.a = 0.8;
            
    //         marker.lifetime = ros::Duration();
    //         marker_array.markers.push_back(marker);
    //         id++;
    //     }
    // }

    visualization_msgs::Marker createEmptyLineStrip(int id)
    {
        double step = 0.2;
        visualization_msgs::Marker marker;
        marker.header.frame_id = "/uav_1/odom";
        marker.header.stamp = ros::Time::now();
        marker.ns = "position_log";
        marker.id = id;
        marker.type = visualization_msgs::Marker::LINE_STRIP;
        marker.action = visualization_msgs::Marker::ADD;
        marker.pose.position.x = 0;
        marker.pose.position.y = 0;
        marker.pose.position.z = 0;
        marker.pose.orientation.w = 1.0;
        marker.scale.x = 1.0*step;
        marker.scale.y = 1.0*step;
        marker.scale.z = 1.0*step;
        marker.color.a = 1.0;
        marker.color.r = 248;
        marker.color.g = 50;
        marker.color.b = 50;
        marker.header.seq++;
        return marker;
    }

}
