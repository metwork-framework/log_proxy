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
if test "${RELEASE}" = ""; then
    echo "ERROR: can't get the latest release version. Please retry in a few minutes."
    exit 1
fi
echo "=> Found release: ${RELEASE}"
echo "=> Found download url: ${DOWNLOAD_URL}"

echo "Removing old releases..."
rm -Rf /opt/log_proxy-linux64-v* >/dev/null 2>&1
rm -Rf /opt/log_proxy >/dev/null 2>&1
rm -Rf /opt/metwork-framework-log_proxy* >/dev/null 2>&1
rm -f "${PREFIX}/bin/log_proxy" >/dev/null 2>&1
rm -f "${PREFIX}/bin/log_proxy_wrapper" >/dev/null 2>&1

cd "${TMPDIR:-/tmp}" || exit 1
echo "Downloading ${DOWNLOAD_URL} into $(pwd)/${ORG}-${REPO}-${RELEASE}.tar.gz..."
curl "${CURL_OPTS}" "${DOWNLOAD_URL}" >"${ORG}-${REPO}-${RELEASE}.tar.gz"
echo "Installing..."
zcat "${ORG}-${REPO}-${RELEASE}.tar.gz" |tar xf -
mkdir -p "${PREFIX}/bin"
for F in log_proxy log_proxy_wrapper; do
  cp -f "log_proxy-linux64-${RELEASE}/${F}" "${PREFIX}/bin/"
  chmod a+rx "${PREFIX}/bin/${F}"
done
echo "Cleaning..."
rm -f "log_proxy-linux64-${RELEASE}.tar.gz"
rm -Rf "log_proxy-linux64-${RELEASE}"
echo "Done"
