#include <TFmini.h>

int main(int argc, char **argv){
  ros::init(argc, argv, "tfmini_ros_node");
  ros::NodeHandle nh("~");
  std::string id = "TFmini"; // TF Frame id
  std::string topic_name = "range"; // ROS Topic name
  std::string portName;
  int baud_rate;
  benewake::TFmini *tfmini_obj;

  nh.param("serial_port", portName, std::string("/dev/ttyUSB0"));
  nh.param("baud_rate", baud_rate, 115200);

  // init TFmini variable & ros publisher
  tfmini_obj = new benewake::TFmini(portName, baud_rate); // create TFmini object
  ros::Publisher pub_range = nh.advertise<sensor_msgs::Range>(topic_name, 1000, true);

  // sensor msgs init
  sensor_msgs::Range TFmini_range;
  TFmini_range.radiation_type = sensor_msgs::Range::INFRARED;
  TFmini_range.field_of_view = 0.04;
  TFmini_range.min_range = 0.3;
  TFmini_range.max_range = 12;
  TFmini_range.header.frame_id = id;
  float dist = 0;

  ROS_INFO_STREAM("Start processing TFmini...");

  while(ros::master::check() && ros::ok()){
    ros::spinOnce();
    dist = tfmini_obj->getDist();
    if(dist > 0 && dist < TFmini_range.max_range) {
      TFmini_range.range = dist;
    }
    else if(dist == 0.0) {
      TFmini_range.range = TFmini_range.max_range;
    }
    TFmini_range.header.stamp = ros::Time::now();
    pub_range.publish(TFmini_range); // publish data

    if(dist == -1.0) {
      ROS_ERROR_STREAM("Failed to read data. TFmini node stopped!");
      break;
    }
  }

  tfmini_obj->closePort();
}
