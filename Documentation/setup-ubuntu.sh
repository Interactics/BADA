
##  외부 조작 필요:
# 한글 입력기 설정(uim)
# - 모든 스텝(재부팅 포함) 따라야 함.
# https://snowdeer.github.io/linux/2018/07/11/ubuntu-18p04-install-korean-keyboard/

# * vscode install commandline 명령과 marketplace ui 연동되지 않음.
# 현재 아예 `vscode --install-extension <ext>` 안됨(멈춤) 
​
## target: ubuntu 18.04 bionic

testcmd () {
    command -v "$1" >/dev/null
}

sudo apt install curl -y
sudo apt install git -y
sudo apt install terminator -y

## vscode
if testcmd code; then
    echo 'vscode is already installed. skipping'
else
    sudo sh -c 'curl https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > /etc/apt/trusted.gpg.d/microsoft.gpg'​
    ​
    sudo sh -c 'echo "deb [arch=amd64] https://packages.microsoft.com/repos/vscode stable main" > /etc/apt/sources.list.d/vscode.list'​
    ​
    sudo apt-get update​
    ​
    sudo apt-get install code​

    # code --install-extension shan.code-settings-sync # not works

    # # gist id : 
    # # github access token : 

    # # keybinding setting:

    echo '
    [{
        "key": "ctrl+a ctrl+a",
        "command": "editor.action.selectAll"
    }, 
    {
        "key": "ctrl+a",
        "command": "-editor.action.selectAll"
    },
    {
        "key": "ctrl+t",
        "command": "-workbench.action.showAllSymbols"
    },
    {
        "key": "ctrl+t ctrl+t",
        "command": "workbench.action.showAllSymbols"
    }, 
    {
        "key": "ctrl+f ctrl+t",
        "command": "editor.action.goToTypeDefinition"
    },
    {
        "key": "ctrl+a ctrl+s",
        "command": "editor.action.addCommentLine",
        "when": "editorTextFocus && !editorReadonly"
    },
    {
        "key": "ctrl+e ctrl+w",
        "command": "editor.emmet.action.expandAbbreviation",
        // "when": "editorTextFocus && !editorReadonly && !editorTabMovesFocus"
    },
    {
        "key": "ctrl+w ctrl+w",
        "command": "workbench.action.closeWindow",
        "when": "!editorIsOpen"
    },
    {
        "key": "ctrl+w ctrl+w",
        "command": "workbench.action.closeActiveEditor"
    },
    {
        "key": "ctrl+k ctrl+c",
        "command": "-editor.action.addCommentLine",
        "when": "editorTextFocus && !editorReadonly"
    },
    {
        "key": "ctrl+s ctrl+a",
        "command": "editor.action.removeCommentLine",
        "when": "editorTextFocus && !editorReadonly"
    },
    {
        "key": "ctrl+k ctrl+u",
        "command": "-editor.action.removeCommentLine",
        "when": "editorTextFocus && !editorReadonly"
    },
    {
        "key": "ctrl+s ctrl+s",
        "command": "workbench.action.files.saveAll"
    },
    {
        "command": "editor.action.addSelectionToNextFindMatch",
        "key": "ctrl+d ctrl+d",
        "when": "editorFocus"
    },
    {
        "command": "editor.foldRecursively",
        "key": "ctrl+e ctrl+q",
        "when": "editorTextFocus"
    },
    {
        "command": "editor.unfoldRecursively",
        "key": "ctrl+q ctrl+e",
        "when": "editorTextFocus"
    },
    {
        "command": "actions.find",
        "key": "ctrl+f ctrl+f"
    },
    {
        "key": "ctrl+f",
        "command": "-actions.find"
    },
    {
        "key": "ctrl+q ctrl+q",
        "command": "workbench.action.quickOpenView"
    },
    {
        "key": "ctrl+q",
        "command": "-workbench.action.quickOpenView"
    },
    {
        "key": "ctrl+e ctrl+e",
        "command": "workbench.action.quickOpen"
    },
    {
        "key": "ctrl+e",
        "command": "-workbench.action.quickOpen"
    },
    {
        "key": "ctrl+h ctrl+h",
        "command": "editor.action.startFindReplaceAction"
    },
    {
        "key": "ctrl+h",
        "command": "-editor.action.startFindReplaceAction"
    },
    {
        "key": "ctrl+a ctrl+c",
        "command": "workbench.action.closeOtherEditors"
    },
    {
        "key": "ctrl+f ctrl+s",
        "command": "workbench.action.gotoSymbol",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+f ctrl+d",
        "command": "editor.action.formatDocument",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+n ctrl+u",
        "command": "workbench.action.files.newUntitledFile"
    },
    {
        "key": "ctrl+n ctrl+n",
        "command": "explorer.newFile"
    },
    {
        "key": "ctrl+n ctrl+f",
        "command": "explorer.newFolder"
    },
    {
        "key": "ctrl+n",
        "command": "-workbench.action.files.newUntitledFile"
    },
    {
        "key": "ctrl+n ctrl+d",
        "command": "editor.action.dirtydiff.next",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+d",
        "command": "-editor.action.addSelectionToNextFindMatch",
        "when": "editorFocus"
    },
    {
        "key": "ctrl+pagedown",
        "command": "-workbench.action.nextEditor"
    },
    {
        "key": "ctrl+r ctrl+r",
        "command": "workbench.action.openRecent"
    },
    {
        "key": "ctrl+r",
        "command": "-workbench.action.openRecent"
    },
    {
        "key": "ctrl+r ctrl+c",
        "command": "editor.debug.action.runToCursor"
    },
    {
        "key": "ctrl+a ctrl+d",
        "command": "workbench.action.navigateForward"
    },
    {
        "key": "ctrl+d ctrl+a",
        "command": "workbench.action.navigateBack"
    },
    {
        "key": "ctrl+w",
        "command": "-workbench.action.closeWindow",
        "when": "!editorIsOpen"
    },
    {
        "key": "ctrl+w",
        "command": "-workbench.action.closeActiveEditor"
    },
    {
        "key": "ctrl+r ctrl+w",
        "command": "workbench.action.reloadWindow"
    },
    {
        "key": "ctrl+f ctrl+a",
        "command": "editor.action.previousMatchFindAction",
        "when": "editorFocus"
    },
    {
        "key": "ctrl+a ctrl+f",
        "command": "editor.action.nextMatchFindAction",
        "when": "editorFocus"
    },
    {
        "key": "ctrl+b ctrl+b",
        "command": "bookmarks.toggle",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+b ctrl+n",
        "command": "bookmarks.jumpToNext",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+b ctrl+p",
        "command": "bookmarks.jumpToPrevious",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+b ctrl+d",
        "command": "bookmarks.jumpToNext",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+b ctrl+a",
        "command": "bookmarks.jumpToPrevious",
        "when": "editorTextFocus"
    },
    {
        "key": "ctrl+f ctrl+t",
        "command": "workbench.action.terminal.focus"
    },
    {
        "key": "f6",
        "command": "workbench.action.navigateEditorGroups"
    },
    {
        "key": "ctrl+j",
        "command": "-workbench.action.togglePanel",
    },
    {
        "key": "ctrl+j ctrl+j",
        "command": "extension.jumpy-word",
        "when": "editorTextFocus"
    }
    ]' > ~/.config/Code/User/keybindings.json
fi

## hangul related packages

# hyphen-en-gb
# gnome-getting-started-docs-ko
# thunderbird-locale-en-gb
# ibus-hangul
# fonts-noto-cjk-extra
# libreoffice-help-ko
# thunderbird-locale-ko
# gnome-user-docs-ko
# hunspell-en-za
# libreoffice-l10n-en-gb
# libreoffice-l10n-ko
# hunspell-ko
# hunspell-en-au
# mythes-en-au
# hunspell-en-gb
# language-pack-gnome-ko
# libreoffice-l10n-en-za
# language-pack-ko
# hyphen-en-ca
# libreoffice-help-en-gb
# firefox-locale-ko
# hunspell-en-ca

## setup ros melodic
## ref: http://wiki.ros.org/melodic/Installation/Ubuntu

if testcmd rosrun; then
    echo 'ros is already installed. skipping'
else
    sudo sh -c 'echo "deb http://packages.ros.org/ros/ubuntu $(lsb_release -sc) main" > /etc/apt/sources.list.d/ros-latest.list'
    sudo apt-key adv --keyserver 'hkp://keyserver.ubuntu.com:80' --recv-key C1CF6E31E6BADE8868B172B4F42ED6FBAB17C654
    curl -sSL 'http://keyserver.ubuntu.com/pks/lookup?op=get&search=0xC1CF6E31E6BADE8868B172B4F42ED6FBAB17C654' | sudo apt-key add -
    sudo apt update
    sudo apt install ros-melodic-desktop-full -y
    echo "source /opt/ros/melodic/setup.bash" >> ~/.bashrc
    source ~/.bashrc

    # build packages
    sudo apt install python-rosdep python-rosinstall python-rosinstall-generator python-wstool build-essential -y
    sudo apt install python-rosdep -y
    sudo rosdep init
    rosdep update
fi


## catkin
## http://wiki.ros.org/catkin/Tutorials
if testcmd cm; then
    echo 'catkin is already installed. skipping'
else
    source /opt/ros/melodic/setup.bash  
    mkdir -p ~/catkin_ws/src
    cd ~/catkin_ws/
    catkin_make
    # catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3
    source devel/setup.bash
    # automatically source setup when sourcing bashrc
    echo 'source ~/catkin_ws/devel/setup.bash' >> ~/.bashrc
    # add aliases
    echo 'alias cm="catkin_make"' >> ~/.bashrc
    echo 'alias sb="source ~/.bashrc"' >> ~/.bashrc
    source ~/.bashrc
fi

## generic programming languages
sudo apt install python-pip -y

## personal project & dev ws setup

### RobotCapstone
if testcmd va; then
    echo 'RobotCapstone package is already installed. skipping'
else
    cd ~/catkin_ws/src
    git clone http://github.com/Interactics/RobotCapstone.git

    #### pigpiod setup
    wget https://github.com/joan2937/pigpio/archive/master.zip
    unzip master.zip
    cd pigpio-master 
    make
    sudo make install

    cd ~/catkin_ws

    #### build package
    rosdep update
    rosdep install --from-paths src --ignore-src -r -y

    cm
    sb

    #### install python dependencies
    sudo apt install virtualenv -y
    virtualenv --system-site-packages -p python3 ~/dev/venv
    echo 'alias va="source ~/dev/venv/bin/activate"' >> ~/.bashrc
    source ~/.bashrc
    # source ~/dev/venv/bin/activate  # sh, bash, ksh, or zsh
    va
    pip install --upgrade pip

    sudo apt-get install libasound-dev portaudio19-dev libportaudio2 libportaudiocpp0 -y
    sudo apt-get install ffmpeg libav-tools -y

    pip install pyaudio soundfile tensorflow==1.15 matplotlib roslibpy rospkg

    cd ~/catkin_ws/src/RobotCapstone/Sound/signal_node/scripts
    wget http://file.epikem.com/yamnet.h5

    ### respeaker_ros setup : https://github.com/furushchev/respeaker_ros
    cd ~/catkin_ws/src
    git clone https://github.com/furushchev/respeaker_ros
    rosdep install --from-paths src -i -r -n -y #amnet
    cm # build respeaker_ros
    sb

    roscd respeaker_ros
    cd scripts
    sudo chmod +x ./*
    sudo cp -f $(rospack find respeaker_ros)/config/60-respeaker.rules /etc/udev/rules.d/60-respeaker.rules
    sudo systemctl restart udev

    roscd respeaker_ros
    sudo pip install -r requirements.txt

    git clone https://github.com/respeaker/usb_4_mic_array.git
    cd usb_4_mic_array
    sudo python dfu.py --download 6_channels_firmware.bin  # The 6 channels version

    # # # usage:
    # roslaunch respeaker_ros respeaker.launch
    # rostopic echo /sound_direction     # Result of DoA
    # rostopic echo /sound_localization  # Result of DoA as Pose
    # rostopic echo /is_speeching        # Result of VAD
    # rostopic echo /audio               # Raw audio
    # rostopic echo /speech_audio        # Audio data while speeching
fi


## test : rosrun signal_node signal_node

## TODO: yamnet_class_map.csv도 scripts 폴더에 있어야 한다!

#deactivate  # don't exit until you're done using TensorFlow

# sudo apt-get install python3-pyaudio
# sudo apt-get install python3-soundfile

# python3 -m pip install --upgrade tensorflow # venv? or not venv? https://www.tensorflow.org/install/pip


# sudo apt-get install python3-pyaudio

#### TODO:
### ros packages: teleop_twist keybord, 
### 



## install node js 
## https://github.com/nodesource/distributions/blob/master/README.md#deb

# Using Ubuntu
curl -sL https://deb.nodesource.com/setup_14.x | sudo -E bash -
sudo apt-get install -y nodejs

# install npm
curl -L https://npmjs.org/install.sh | sudo sh

# ## You may also need development tools to build native addons:
#      sudo apt-get install gcc g++ make
# ## To install the Yarn package manager, run:
#      curl -sL https://dl.yarnpkg.com/debian/pubkey.gpg | sudo apt-key add -
#      echo "deb https://dl.yarnpkg.com/debian/ stable main" | sudo tee /etc/apt/sources.list.d/yarn.list
#      sudo apt-get update && sudo apt-get install yarn
