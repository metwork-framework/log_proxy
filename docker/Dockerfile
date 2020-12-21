FROM metwork/centos6
MAINTAINER Fabien MARTY <fabien.marty@gmail.com>

RUN yum -y install gcc make valgrind glib2-devel glib2-static glibc-static

ADD entrypoint.sh /entrypoint.sh
CMD /entrypoint.sh
