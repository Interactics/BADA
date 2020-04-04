# simple ros2d visualization app

## description

this app provides a static html file that connects to a rosbridge server via a websocket.
the html file is served by a simple express web server.

## installation

1. install `nodejs`
2. install node package manager: `npm` or `yarn`
3. install node_modules by running below command in `ros2d` folder:

npm:
```
npm install
```

yarn:
```
yarn
```

4. run express server:

npm:
```
npm run start
```

yarn:
```
yarn start
```

## connecting to a rosbridge websocket server

1. run rosbridge websocket server by running these commands on *separate* terminals:
> 📂 `terminal`
```
roscore
rosrun rosbridge_server rosbridge_websocket.py
```

2. run express server:
> 📂 `terminal:Visualization/ros2d`
```
# npm:
npm run start

# yarn:
yarn start
```

3. try running a rosbag file:
```
rosbag play <bagfile path>
```

## stack

- nodejs
- express
- roslib