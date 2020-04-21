#!/usr/bin/env python
import rospy
import json
from std_msgs.msg import String

pub=''
signals={}

def callback(data):
    global pub
    # rospy.loginfo(rospy.get_caller_id() + "I heard %s", data.data)
    dat=json.loads(data.data)
    rospy.loginfo("hello world %s" % rospy.get_time())

    # hello_str = "hello world %s" % rospy.get_time()
    keys=['Speech','Alarm','Door','Television']
    picked=[]
    for i,v in enumerate(dat):

        if(v[0][0] in keys):
            rospy.loginfo(v[0][0] in keys)
            picked.append(v[0])
    
    # algorithm: 
    # for all keys;
    #  if the key is picked
    #  update 
    for i, v in enumerate(picked):
        if(v[0] not in signals):
            rospy.loginfo(v[0])
            print('d',v[1])
            signals[v[0]]=float(v[1])
        else:
            rospy.loginfo(signals[v[0]])
            rospy.loginfo(v[1])

            signals[v[0]]=(float(signals[v[0]])*0.5+float(v[1])*0.5)

    rospy.loginfo(signals)
    hello_str=json.dumps(dat)
    pub.publish(hello_str)
    
def signal_node():
    global pub
    rospy.init_node('signal_node', anonymous=True)
    rospy.loginfo('starting')
    # rate = rospy.Rate(10) # 10hz

    # while not rospy.is_shutdown():
    #     rate.sleep()

    # In ROS, nodes are uniquely named. If two nodes with the same
    # name are launched, the previous one is kicked off. The
    # anonymous=True flag means that rospy will choose a unique
    # name for our 'signal_node' node so that multiple signal_nodes can
    # run simultaneously.

    rospy.Subscriber("/audio", String, callback)
    pub = rospy.Publisher('/signal', String, queue_size=10)

    # spin() simply keeps python from exiting until this node is stopped
    rospy.spin()

if __name__ == '__main__':
    signal_node()

    # except rospy.ROSInterruptException:
    #     pass


# #!/usr/bin/env python
# # license removed for brevity
# import rospy
# from std_msgs.msg import String

# def talker():
#     rospy.init_node('talker', anonymous=True)
#     rate = rospy.Rate(10) # 10hz
#     while not rospy.is_shutdown():
#         hello_str = "hello world %s" % rospy.get_time()
#         rospy.loginfo(hello_str)
#         pub.publish(hello_str)
#         rate.sleep()

# if __name__ == '__main__':
#     try:
#         talker()
#     except rospy.ROSInterruptException:
#         pass
