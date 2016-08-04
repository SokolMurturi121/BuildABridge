

// for csv_parser
#include "E:\Tools and Middleware Bridge\octet-master\octet\src\resources\CSV_test_parser.h"

namespace octet {
  class hinge_joint_bridge : public app {

    // scene for drawing objects
    ref<visual_scene> app_scene;

    // dynamics world
    btDynamicsWorld *dynamicsWorld;

    // store shape's mesh instances and their rigid bodies so they can be used within the scene
    dynarray<mesh_instance*> meshInstances;
    dynarray<btRigidBody*> rigidBodies;

    // in this array we will store all the information required to build the bridge from the CSV file
    dynarray<vec4> data;

    // materials that will be used to give the scene some color
    material *groundMaterial;
    material *platformMaterial;
    material *hingePlankMaterial;
    material *springPlankMaterial;
    material *ballMaterial;

    // random spawn point location for falling planks and balls
    random rand;

    // approximate frame counte used to drop objects onto bridge for testing
    int frame;

    enum {
      // data array containing all information required to build the bridge from the csv file
	  bridgeHeight = 0,
      plankNumber,
      ballNumber,
	  slabNumber,
	  hingeGap,
	  springGap,
	  hingePlatformPos,
      springPlatformPos,
	  plankSize,
      platformSize,
	  boxSize,
	  cameraPos,
      cameraRot,
	 // mesh and rigid body array indices that do not require csv data to calculate
      ground = 0,
      firstPlatform,
      lastPlatform,
    };

	// these indicies will be used in the meshs and rigid bodies based on csv data 
	//and will determin all start and enpoints for bridges and objects
    int firstHingePlatform, lastHingePlatform;
    int firstSpring, lastSpring;
    int firstSpringPlank, lastSpringPlank;
    int firstBall, lastBall;
    int firstSlab, lastSlab;

    // second platform x coordinates
    int hingePlatform2Pos;
    int springPlatform2Pos;

  public:
	  hinge_joint_bridge(int argc, char **argv) : app(argc, argv) {
    }

    ~hinge_joint_bridge() {
    }

  private:
    /// Called once OpenGL is initialised.
    void app_init() {
      loadData();

      app_scene = new visual_scene();
      app_scene->create_default_camera_and_lights();
      app_scene->get_camera_instance(0)->get_node()->translate(vec3(data[cameraPos][0], data[cameraPos][1],
                                                               data[cameraPos][2]));
      app_scene->get_camera_instance(0)->get_node()->rotate(data[cameraRot][0], vec3(data[cameraRot][1],
                                                            data[cameraRot][2], data[cameraRot][3]));
	  dynamicsWorld = app_scene->get_dynamics_world();

	  //define materials using Hexcolor
      groundMaterial = new material(vec4(0.5, 0.5, 0.5, 1));
	  platformMaterial = new material(vec4(0.102, 1, 1, 1));
	  hingePlankMaterial = new material(vec4(0, 0.102, 0.153, 1));
	  springPlankMaterial = new material(vec4(1, 0, 0.102, 1));
	  ballMaterial = new material(vec4(0, 0.51, 0, 1));
      IndicieCalculations();
      // ground
      mat4t mat;
      mat.loadIdentity();
      app_scene->add_shape(mat, new mesh_box(vec3(200, 1, 200)), groundMaterial, false);
      add_to_arrays(ground);
      // calls functions used to build bridges check definition
      CreateHingeBridge();
	  CreateSpringBridge();
      frame = 0;
    }

    /// Add a shape's mesh instance and rigid body to the corresponding arrays.
    void add_to_arrays(int index) {
		meshInstances.push_back(app_scene->get_mesh_instance(index));
		rigidBodies.push_back(meshInstances[index]->get_node()->get_rigid_body());
    }

    /// Creates a box with a certain size, color, and location.
    void BuildBox(vec3 boxSize, mat4t mat, vec3 location, material *color, bool isDynamic, int array_index) {
      mat.loadIdentity();
      mat.translate(location);
      app_scene->add_shape(mat, new mesh_box(boxSize), color, isDynamic);
      add_to_arrays(array_index);
    }

    /// Create a bridge using the hinge constraints.
    void CreateHingeBridge() {
      mat4t mat;
      BuildBox(data[platformSize], mat, vec3(data[hingePlatformPos][0], data[hingePlatformPos][1], data[hingePlatformPos][2]),
		  platformMaterial, false, firstPlatform);
	  
      /// calculate where the second platform should be placed along its X coordinate
      hingePlatform2Pos = data[hingePlatformPos][0] + ((data[plankNumber][0] + 1) * data[hingeGap][0]);
      BuildBox(data[platformSize], mat, vec3(hingePlatform2Pos, data[hingePlatformPos][1], data[hingePlatformPos][2]),
		  platformMaterial, false, lastPlatform);

      vec3 plankLocation = vec3(data[hingePlatformPos][0] + data[hingeGap][0],
                                 data[hingePlatformPos][1] + (data[bridgeHeight][0] - 0.5f), data[hingePlatformPos][2]);
      BuildBox(data[plankSize], mat, plankLocation, hingePlankMaterial, true, firstHingePlatform);

      btHingeConstraint *hinge = new btHingeConstraint(*rigidBodies[firstPlatform], *rigidBodies[firstHingePlatform],
                                                        btVector3(data[hingeGap][0] * 0.5f,
                                                                  data[bridgeHeight][0] - (data[plankSize][1] * 0.5f), 0),
                                                        btVector3(data[hingeGap][0] * - 0.5f, 0, 0),
                                                        btVector3(0, 0, 1), btVector3(0, 0, 1), false);
	  dynamicsWorld->addConstraint(hinge);

      // place and hinge the remaining planks
      int i = firstHingePlatform + 1;
      for (i; i <= lastHingePlatform; i++) {
        plankLocation = vec3(plankLocation[0] + data[hingeGap][0], plankLocation[1], plankLocation[2]);
        BuildBox(data[plankSize], mat, plankLocation, hingePlankMaterial, true, i);

        hinge = new btHingeConstraint(*rigidBodies[i-1], *rigidBodies[i], btVector3(data[hingeGap][0] * 0.5f, 0, 0),
                                       btVector3(data[hingeGap][0] * -0.5f, 0, 0), btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		dynamicsWorld->addConstraint(hinge);

        // deal with the last plank to platform hinge
        if (i == lastHingePlatform) {
          hinge = new btHingeConstraint(*rigidBodies[i], *rigidBodies[lastPlatform],
                                         btVector3(data[hingeGap][0] * 0.5f, 0, 0),
                                         btVector3(data[hingeGap][0] * -0.5f, data[bridgeHeight][0] - 0.5f, 0),
                                         btVector3(0, 0, 1), btVector3(0, 0, 1), false);
		  dynamicsWorld->addConstraint(hinge);
        }
      }
    }

    /// Assemble spring bridge.
    /// Reference: http://bullet.googlecode.com/svn/trunk/Demos/ConstraintDemo/ConstraintDemo.cpp
    void CreateSpringBridge() {
      // place the first and last platforms
      mat4t mat;
      BuildBox(data[platformSize], mat, vec3(data[springPlatformPos][0], data[springPlatformPos][1], data[springPlatformPos][2]),
		  platformMaterial, false, firstSpring);
      // calculate the second platform's x coordinate
      springPlatform2Pos = data[springPlatformPos][0] + ((data[plankNumber][0] + 1) * data[springGap][0]);
      BuildBox(data[platformSize], mat, vec3(springPlatform2Pos, data[springPlatformPos][1], data[springPlatformPos][2]),
		  platformMaterial, false, lastSpring);

      btTransform frame_in_a, frame_in_b;
      btGeneric6DofSpringConstraint *spring;
      vec3 plankLocation = vec3(data[springPlatformPos][0], data[springPlatformPos][1], data[springPlatformPos][2]);
      for (int i = 0; i <= lastSpringPlank - firstSpringPlank; i += 2) {
        // create the spring planks
        float new_x_location = plankLocation[0] + data[springGap][0];
        plankLocation = vec3(new_x_location, plankLocation[1], plankLocation[2]);
        BuildBox(data[plankSize], mat, plankLocation, springPlankMaterial, true, firstSpringPlank + i);

        // create the spring anchors
        BuildBox(data[plankSize], mat, vec3(plankLocation[0], plankLocation[1] + 30, plankLocation[2]), springPlankMaterial, false,
                   firstSpringPlank + i + 1);

        frame_in_a = btTransform::getIdentity();
        frame_in_a.setOrigin(btVector3(btScalar(0), btScalar(0), btScalar(0)));
        frame_in_b = btTransform::getIdentity();
        frame_in_b.setOrigin(btVector3(btScalar(0), btScalar(25), btScalar(0)));
        spring = new btGeneric6DofSpringConstraint(*rigidBodies[firstSpringPlank+i+1], *rigidBodies[firstSpringPlank+i],
                                                    frame_in_a, frame_in_b, true);

        spring->setLinearUpperLimit(btVector3(0, 0, 0));
        spring->setLinearLowerLimit(btVector3(0, -10, 0));
        spring->setAngularLowerLimit(btVector3(0, 0, 0));
        spring->setAngularUpperLimit(btVector3(0, 0, 0));

        spring->enableSpring(1, true);
        spring->setStiffness(1, 19);
        spring->setDamping(1, 0.5f);
        //spring->setEquilibriumPoint();
        app_scene->get_dynamics_world()->addConstraint(spring);
      }
    }

    /// Detect collisions.
    /// Reference: http://hamelot.co.uk/programming/using-bullet-only-for-collision-detection/
    void handle_collisions() {
      // perform collision detection
		dynamicsWorld->performDiscreteCollisionDetection();
      int num_manifolds = dynamicsWorld->getDispatcher()->getNumManifolds();

      // for each contact manifold
      for (int i = 0; i < num_manifolds; i++) {
        btPersistentManifold *contact_manifold = dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        const btCollisionObject *object_a = static_cast<const btCollisionObject*>(contact_manifold->getBody0());
        const btCollisionObject *object_b = static_cast<const btCollisionObject*>(contact_manifold->getBody1());
        contact_manifold->refreshContactPoints(object_a->getWorldTransform(), object_b->getWorldTransform());
        int numberContacts = contact_manifold->getNumContacts();
      }
    }


    /// Create a ball.
    void create_ball(mat4t mat, vec3 location, material *color, int array_index) {
      mat.loadIdentity();
      mat.translate(location);
      app_scene->add_shape(mat, new mesh_sphere(), ballMaterial, true);
      add_to_arrays(array_index);
    }

    /// Spawn ball and slab objects above the bridges.
    void spawn_objects() {
      mat4t mat;

      float random_x, random_y;
      int spawn_height = data[bridgeHeight][0] + (data[bridgeHeight][0] * 3);
      int max_spawn_height = spawn_height + 10;

      int hinge_min = data[hingePlatformPos][0] + data[hingeGap][0];
      int hinge_max = hingePlatform2Pos - data[hingeGap][0];
      int spring_min = data[springPlatformPos][0] + data[springGap][0];
      int spring_max = springPlatform2Pos - data[springGap][0];

      if (frame == 0) {
        // balls
        for (int i = 0; i < data[ballNumber][0]; i++) {
          // generate randomish positions above the bridge
          random_x = rand.get(hinge_min, hinge_max);
          random_y = rand.get(spawn_height, max_spawn_height);
          create_ball(mat, vec3(random_x, random_y, data[hingePlatformPos][2]), ballMaterial, firstBall + i);
        }

        //slabs
        for (int i = 0; i < data[slabNumber][0]; i++) {
          // generate randomish positions above the bridge
          random_x = rand.get(spring_min, spring_max);
          random_y = rand.get(spawn_height, max_spawn_height);
          BuildBox(data[boxSize], mat, vec3(random_x, random_y, data[springPlatformPos][2]), ballMaterial, true, firstSlab + i);
        }
      } else if (frame % 125 == 0) {
        // balls
        for (int i = 0; i < data[ballNumber][0]; i++) {
          // generate randomish positions above each bridge
          random_x = rand.get(hinge_min, hinge_max);
          random_y = rand.get(spawn_height, max_spawn_height);

          mesh_instance *ball = meshInstances[firstBall+i];
          ball->get_node()->set_linear_velocity(vec3(0, 0, 0));
          ball->get_node()->set_position(vec3(random_x, random_y, data[hingePlatformPos][2]));
        }

        // slabs
        for (int i = 0; i < data[slabNumber][0]; i++) {
          // generate randomish positions above each bridge
          random_x = rand.get(spring_min, spring_max);
          random_y = rand.get(spawn_height, max_spawn_height);

          mesh_instance *slab = meshInstances[firstSlab+i];
          slab->get_node()->set_linear_velocity(vec3(0, 0, 0));
          slab->get_node()->set_position(vec3(random_x, random_y, data[springPlatformPos][2]));
        }

      }
      frame++;
    }

    /// Loads bridge data from a csv file.
    void loadData() {
		CSV_test_parser parser;
      parser.parse_file("data2.csv", data);

      for (int i = 0; i < data.size(); i++) {
        for (int j = 0; j < 4; j++) {
          printf(" %f ", data[i][j]);
        }
        printf("\n");
      }
    }

    /// Calculate remaining shape mesh and rigid body indicies based on csv data
    void IndicieCalculations() {
		firstHingePlatform = lastPlatform + 1;
		lastHingePlatform = firstHingePlatform + data[plankNumber][0] - 1;

		firstSpring = lastHingePlatform + 1;
		lastSpring = firstSpring + 1;

      firstSpringPlank = lastSpring + 1;
      lastSpringPlank = firstSpringPlank + (data[plankNumber][0] * 2) - 1;

      firstBall = lastSpringPlank + 1;
      lastBall = firstBall + data[ballNumber][0] - 1;

      firstSlab = lastBall + 1;
      lastSlab = firstSlab + data[slabNumber][0] - 1;
    }

    /// Called to draw the world.
    void draw_world(int x, int y, int w, int h) {
      int vx = 0, vy = 0;
      get_viewport_size(vx, vy);
      app_scene->begin_render(vx, vy);

      // update matrices, assume 30fps
      app_scene->update(1.0f / 30);

      // draw the scene
      app_scene->render((float)vx / vy);

      spawn_objects();

      handle_collisions();
    }
  };
}