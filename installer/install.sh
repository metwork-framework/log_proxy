#!/bin/bash

# paranoid mode
set -eu

# some vars
ORG=metwork-framework
REPO=log_proxy
CURL_OPTS="-fsSLk"
PREFIX=/usr/local

# Check if we are root (or if we used FORCE argument)
IDU=$(id -u)
if test "${IDU}" != "0"; then
  if test "${1:-}" != "FORCE"; then
    echo "ERROR: you must run this script as root user"
    echo "       (or use FORCE argument if you know exactly what you are doing)"
    exit 1
  fi
fi

GITHUB_URL="https://api.github.com/repos/${ORG}/${REPO}/releases/latest"
echo "Getting latest release on ${GITHUB_URL}..."
DOWNLOAD_URL=$(curl "${CURL_OPTS}" "${GITHUB_URL}" |grep "browser_download_url.:..https.*tar.gz" |cut -d : -f 2,3 | tr -d \" |tr -d ' ')
RELEASE=$(echo "${DOWNLOAD_URL}" |sed 's~.*/\(v[0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\)/.*$~\1~g')
echo "=> Found release: ${RELEASE}"
echo "=> Found download url: ${DOWNLOAD_URL}"

cd /opt
echo "Downloading ${DOWNLOAD_URL}..."
curl "${CURL_OPTS}" "${DOWNLOAD_URL}" >"${ORG}-${REPO}-${RELEASE}.tar.gz"
echo "Installing..."
zcat "${ORG}-${REPO}-${RELEASE}.tar.gz" |tar xf -
mkdir -p "${PREFIX}/bin"
for F in log_proxy log_proxy_wrapper; do
  cp -f "log_proxy-linux64-${RELEASE}/${F}" "${PREFIX}/bin/"
  chmod a+rx "${PREFIX}/bin/${F}"
done
rm -f "log_proxy-linux64-${RELEASE}.tar.gz"
echo "Done"
