#include <orthogonal_planes.h>
#include <gtest/gtest.h>

namespace LazyThetaStarOctree{
	void testCoordinateFrame(octomath::Vector3 start, octomath::Vector3 goal)
	{
		CoordinateFrame coordinate_frame = generateCoordinateFrame(start, goal);

		// All vectors are orthogonal
		ASSERT_NEAR(coordinate_frame.orthogonalA.dot(coordinate_frame.orthogonalB), 0, 0.0000001)  << "All unitary vector must be orthogonal. (dot product equals 0)";
		ASSERT_NEAR(coordinate_frame.orthogonalA.dot(coordinate_frame.direction), 0, 0.0000001) << "All unitary vector must be orthogonal. (dot product equals 0)";
		ASSERT_NEAR(coordinate_frame.orthogonalB.dot(coordinate_frame.direction), 0, 0.0000001) << "All unitary vector must be orthogonal. (dot product equals 0)";
		
		// All vectors are unitary
		ASSERT_NEAR(coordinate_frame.direction.norm(),   1, 0.0001) << "Unitary vector should have size 1";
		ASSERT_NEAR(coordinate_frame.orthogonalA.norm(), 1, 0.0001) << "Unitary vector should have size 1";
		ASSERT_NEAR(coordinate_frame.orthogonalB.norm(), 1, 0.0001) << "Unitary vector should have size 1";
	}


	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToYaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (0, 1, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToNegatativeYaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (0, -1, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToXaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (1, 0, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToNegatativeXaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (-1, 0, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToZaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (0, 0, 1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_closeToOriginToZaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (0.1, 0, 1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToNegatativeZaxis)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (0, 0, -1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToXaxisZero)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (0, 1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(0, -1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(0, -1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(0, 1, -1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToYaxisZero)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (1, 0, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 0, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 0, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, 0, -1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_originToAllDiagonals)
	{
		octomath::Vector3 start (0, 0, 0);
		octomath::Vector3 goal  (1, 1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, 1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, -1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, -1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, -1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, -1, -1);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToYaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (0, 1, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToNegatativeYaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (0, -1, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToXaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (1, 0, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToNegatativeXaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (-1, 0, 0);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToZaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (0, 0, 1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_closeToOriginToZaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (0.1, 0, 1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToNegatativeZaxis)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (0, 0, -1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToXaxisZero)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (0, 1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(0, -1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(0, -1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(0, 1, -1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToYaxisZero)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (1, 0, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 0, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 0, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, 0, -1);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_offset_originToAllDiagonals)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (1, 1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, 1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, 1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, -1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, -1, 1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(-1, -1, -1);
		testCoordinateFrame(start, goal);
		goal = octomath::Vector3(1, -1, -1);
		testCoordinateFrame(start, goal);
	}



	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (20, -31, 40);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest1)
	{
		octomath::Vector3 start (2, 5, -634);
		octomath::Vector3 goal  (20, -31, 40);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest2)
	{
		octomath::Vector3 start (-2, 785, -634);
		octomath::Vector3 goal  (20, -31, 40);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest3)
	{
		octomath::Vector3 start (-2, 5, -634);
		octomath::Vector3 goal  (20, -31, -40);
		testCoordinateFrame(start, goal);
	}
	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest4)
	{
		octomath::Vector3 start (2, 25, -634);
		octomath::Vector3 goal  (20, -31, 40);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest5)
	{
		octomath::Vector3 start (2, -5, -634);
		octomath::Vector3 goal  (20, -31, 40);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest6)
	{
		octomath::Vector3 start (-2, 785, -634);
		octomath::Vector3 goal  (-20, -31, 40);
		testCoordinateFrame(start, goal);
	}

	TEST(OrthogonalPlanesTest, generateCoordinateFrame_monkeyTest7)
	{
		octomath::Vector3 start (-2, -5, -634);
		octomath::Vector3 goal  (-20, -31, -40);
		testCoordinateFrame(start, goal);
	}

	// TEST(OrthogonalPlanesTest, generategoalWithDistance_)
	// {
	// 	octomath::Vector3 start (1, 1, 1);
	// 	octomath::Vector3 goal  (-1, -1, -1);
	// 	double margin = 2 * (start.norm() );
	// 	ASSERT_TRUE (   equal( calculateGoalWithMargin(start, goal, margin), octomath::Vector3(-2, -2, -2) )   );
	// }

	// TEST(OrthogonalPlanesTest, generatePlaneIndexes_16)
	// {
	// 	double margin = 1;
	// 	double resolution = 0.5;
	// 	std::vector<octomath::Vector3> plane = {};
	// 	generateRectanglePlaneIndexes(margin, resolution, plane);
	// 	ASSERT_EQ(16, plane.size());
	// }

	// TEST(OrthogonalPlanesTest, generatePlaneIndexes_25)
	// {
	// 	double margin = 1.25;
	// 	double resolution = 0.5;
	// 	std::vector<octomath::Vector3> plane = {};
	// 	generateRectanglePlaneIndexes(margin, resolution, plane);
	// 	ASSERT_EQ(25, plane.size());
	// }

	// TEST(OrthogonalPlanesTest, generateCircle)
	// {
	// 	double margin = 1;
	// 	double resolution = 0.5;
	// 	std::vector<octomath::Vector3> plane = {};
	// 	generateCirclePlaneIndexes(margin, resolution, plane);
	// 	ASSERT_EQ(21, plane.size());
	// 	for (std::vector<octomath::Vector3>::iterator i = plane.begin(); i != plane.end(); ++i)
	// 	{
	// 		ROS_WARN_STREAM(*i);	
	// 	}
	// }

	// TEST(OrthogonalPlanesTest, generateCircle)
	// {
	// 	double margin = 1;
	// 	double resolution = 0.5;
	// 	std::vector<octomath::Vector3> plane = {};
	// 	generateCirclePlaneIndexes(margin, resolution, plane);
	// 	ASSERT_EQ(21, plane.size());
	// 	for (std::vector<octomath::Vector3>::iterator i = plane.begin(); i != plane.end(); ++i)
	// 	{
	// 		ROS_WARN_STREAM(*i);	
	// 	}
	// }

	// TEST(OrthogonalPlanesTest, generateSemiSphere)
	// {
	// 	double margin = 1;
	// 	double resolution = 0.5;
	// 	std::vector<octomath::Vector3> plane = {};
	// 	std::vector<octomath::Vector3> semiSphere = {};
	// 	generateCirclePlaneIndexes(margin, resolution, plane);
	// 	generateSemiSphereOut(margin, resolution, plane, semiSphere);
	// 	ASSERT_EQ(21, plane.size());
	// 	ASSERT_EQ(21, semiSphere.size());
	// 	for (std::vector<octomath::Vector3>::iterator i = semiSphere.begin(); i != semiSphere.end(); ++i)
	// 	{
	// 		ROS_WARN_STREAM(*i);	
	// 	}
	// }

	
	TEST(OrthogonalPlanesTest, generateCoordinateFrame)
	{
		octomath::Vector3 start (0, 0,0 );
		octomath::Vector3 goal (-0.333, 2, 40);
	    CoordinateFrame coord = generateCoordinateFrame(start, goal);
	    // ROS_WARN_STREAM("direction " << coord.direction);
	    // ROS_WARN_STREAM("orthogonalA " << coord.orthogonalA);
	    // ROS_WARN_STREAM("orthogonalB " << coord.orthogonalB);
	    testCoordinateFrame(start, goal);
	}


	TEST(OrthogonalPlanesTest, rotation)
	{
		octomath::Vector3 start (0, 0,0 );
		octomath::Vector3 goal (-0.333, 2, 40);
		CoordinateFrame coordinate_frame = generateCoordinateFrame(start, goal);

		double margin = 1;
		double resolution = 0.5;
		Eigen::MatrixXd points(3, 3);

		points( 0, 0) = 1;
		points( 1, 0) = 0;
		points( 2, 0) = 0;
		
		points(0, 1) = 0;
		points(1, 1) = 1;
		points(2, 1) = 0;

		points(0, 2) = 0;
		points(1, 2) = 0;
		points(2, 2) = 1;

		ROS_WARN_STREAM(points);
	    ROS_WARN_STREAM( rotate_many(coordinate_frame, points) );
	}



	TEST(OrthogonalPlanesTest, rotationTranslation)
	{
		octomath::Vector3 start (-0.333, 2, 40 );
		octomath::Vector3 goal (1, 1, 1);
		CoordinateFrame coordinate_frame = generateCoordinateFrame(start, goal);

		double margin = 1;
		double resolution = 0.5;
		
		std::vector<Eigen::Vector3d> plane = {};
		generateCirclePlaneIndexes(margin, resolution, plane);
		
		Eigen::MatrixXd point_matrix (3, plane.size());
		int index = 0;
		for (std::vector<Eigen::Vector3d>::iterator i = plane.begin(); i != plane.end(); ++i)
		{
			point_matrix(0, index) = i->x();
			point_matrix(1, index) = i->y();
			point_matrix(2, index) = i->z();
			index++;
		}

		// ROS_WARN_STREAM(point_matrix);

	    Eigen::MatrixXd rotated_points = rotate_many(coordinate_frame, point_matrix);
	    Eigen::Vector3d start_eigen ((double)start.x(), (double) start.y(), (double) start.z());
	    for (int i = 0; i < plane.size(); ++i)
	    {
	    	Eigen::Vector3d point(rotated_points(0, i), rotated_points(1, i), rotated_points(2, i) ) ;



	    	ROS_WARN_STREAM("Translation and rotation: " << translateStartGoal_many(point, start_eigen));
	    }

	    // Eigen::MatrixXd  result = translateStartGoal_many(rotated_points, start_eigen);
	    // ROS_WARN_STREAM(result);
	}
}

int main(int argc, char **argv){
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}