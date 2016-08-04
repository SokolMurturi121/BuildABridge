# Tools and middleware Build A Bridge project
# Hinge constraints by Sokol Murturi


![Physics Demo]( https://github.com/SokolMurturi121/BuildABridge/blob/master/Tools%20and%20Middleware%20Bridge/octet-master/octet/src/examples/example_shapes/images/bridges.PNG)

### Introduction
This is a small Bullet Physics demo written in OpenGL and C++.  The aim of this demo is to generate two different types of bridges, one made with hinge constraints and the other using springs constraints. I test the constraints by dropping a number of objects onto the bridges themselves to see how they would react.
In this demonstration my main focus was to understand several different key concepts. Such as: Settings up objects by parsing data from a csv file, Bullet hinge constraints, Bullet spring constraints and finally Bullet collision callbacks.
### Reading CSV Data 
The  information for: size, position and number of objects in the scene are determined by a CSV file .  the function parse_file () takes the file path of the csv file. This then creates an array of vector fours. Rows of data that have less than four values are padded with zeros 
In the CSV file some of the values stored rely on one another, an example of this is the width of the planks. To increase the width of the planks we must fist incease the value of the bridge width so the planks will increase aswell.
### Hinge Constraints

#### Hinge Bridge Construction 
The bridge constructions are largely dependent on the first and second position of the platform. This information is parsed from the CSV file. The first platform is placed using its x coordinates this information is then passed on to the second platform. After this has been done we can then hinge the first bridge plank to the bridge platform. After these two have been connected to one another each consecutive plank is hinged to the one before it using a for loop where the gap is added to the x position of the upcoming plank. The final plank is slightly different. At the end of the for loop the last plank is then attached to the second platform.

![Hinge Bridge]( https://github.com/SokolMurturi121/BuildABridge/blob/master/Tools%20and%20Middleware%20Bridge/octet-master/octet/src/examples/example_shapes/images/hinge%20bridge.PNG)


### Spring Constraints

#### Spring Bridge Construction 
The springy bridge is very similarly constructed to the hinge bridge, the first and second platforms are constructed and placed then the process is almost identical to the hinge bridge. The spring effect is made by anchoring each of the planks to act as fixed points on the spring. After this we set the limits to keep the planks uniform and then the platforms are allowed to move up and down in the y axis. 
![Spring Bridge]( https://github.com/SokolMurturi121/BuildABridge/blob/master/Tools%20and%20Middleware%20Bridge/octet-master/octet/src/examples/example_shapes/images/spring%20bridge.PNG)


### Collision Callback

#### Handle Collisions 
I check the collision callbacks by finding the number of points of contact on each manifold.
![Collision Objects]( https://github.com/SokolMurturi121/BuildABridge/blob/master/Tools%20and%20Middleware%20Bridge/octet-master/octet/src/examples/example_shapes/images/collision%20object.PNG)


### Other Functions

#### Dropping the Objects - .
This function drops random balls and boxes on the hinge and spring bridge at set intervals. The intervals are set In the draw_world() function  this is done by counting the approximate number of frames which go buy as the scene is updated. This frame integer is then used to determine how long between frames we should wait to drop more objects on the bridge. The original objects are just translated above the bridge when the frame count is correct. 
![Demo Youtube Video]( https://www.youtube.com/watch?v=Fmc4ynu2lRo)

