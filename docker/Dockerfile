FROM centos:centos6
MAINTAINER Fabien MARTY <fabien.marty@gmail.com>

RUN yum -y install gcc make valgrind

ADD entrypoint.sh /entrypoint.sh
CMD /entrypoint.sh
