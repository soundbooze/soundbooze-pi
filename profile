export PATH="\
/bin:\
/sbin:\
/usr/bin:\
/usr/sbin:\
/opt/vc/bin\
"

export MANPATH="\
/usr/share/man\
"

export PKG_CONFIG_PATH="\
/usr/lib/pkgconfig:\
/opt/vc/lib/pkgconfig\
"

export LANG=en_US.UTF-8
export INPUTRC=/etc/inputrc
export PS1="# "
export EDITOR="/usr/bin/joe"
export PAGER="/usr/bin/less -isR"
export GCC_COLORS=auto

umask 022
shopt -s cdspell
shopt -s dotglob
shopt -s checkwinsize

function qfind() { find . -iname "*$@*" ;}
function cfind() { find . -type f -exec grep -Hin --color=auto "$@" {} \; ;}

alias dir='dir --color'
alias ls='ls -hAl --color=auto'
alias ll='ls'
#alias ps="ps -Awwo pid,user,%cpu,%mem,command"
alias df='df -h'
alias free='free -m'
alias cp='cp -i'
alias mv='mv -i'
alias rm='rm -i'
alias vim='vim -i NONE'
alias lsusb='/usr/local/bin/lsusb && lsusb -t'
alias heat='vcgencmd measure_volts && vcgencmd measure_temp'
alias dropcache='echo 1 > /proc/sys/vm/drop_caches && echo 2 > /proc/sys/vm/drop_caches && echo 3 > /proc/sys/vm/drop_caches'
alias wifi='ifup wlan0'
alias ntpdate='ntpdate -s 0.id.pool.ntp.org'
alias uname='uname -ap'
alias weston='weston --xwayland -i0 > /dev/null 2> /dev/null'

export HISTFILE=/dev/null
export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:/usr/local/lib/pkgconfig:/usr/X11/lib/pkgconfig:/usr/X11/share/pkgconfig:/opt/vc/lib/pkgconfig
export LD_LIBRARY_PATH=/usr/X11/lib:/usr/local/lib:/root/misc/gcc-7.2.0/gcc-build/lib
export PATH=$PATH:/usr/local/bin:/usr/local/sbin:/usr/X11/bin:/opt/jdk/bin:/opt/ant/bin
export MANPATH=$MANPATH:/usr/local/share/man
export XDG_RUNTIME_DIR=/run/shm/wayland
export JAVA_HOME=/opt/jdk
export ANT_HOME=/opt/ant

# init
setterm --blank 0
setfont Lat2-Terminus16
